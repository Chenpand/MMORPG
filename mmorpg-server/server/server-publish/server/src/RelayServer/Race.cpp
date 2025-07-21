#include "Race.h"
#include <AvalonPacket.h>
#include <CLChatProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLErrorCode.h>

#include "RaceMgr.h"
#include "RLPlayerMgr.h"
#include "RLApplication.h"
#include "RelaySvrRouter.h"
#include "VerifyServerMgr.h"

Race::Race(RaceType type, RaceInstanceType subType)
{
	m_raceType = type;
    m_type = subType;
    m_lastStatus = m_status = RACE_STATUS_INIT;
    m_nextLogicFrameSequence = 1;
    m_nextRealFrame = 0;
    m_checkedSyncFrame = 0;
	m_createTime = (UInt32)CURRENT_TIME.Sec();
	m_isUnsync = false;
	m_destoryTime = 0;
	m_reconnNum = 0;
	m_reviveNum = 0;
	m_levelUpNum = 0;
	m_firstUnsyncFrame = 0;
	m_InitPacket = NULL;
	m_StartVerifyTime = 0;
	m_VSId = 0;
	m_VerifyRaceEndPacket = NULL;
}

Race::~Race()
{
	// 这里再清一次rlplayer对象
	GameSessionEvent event;
	event.id = gamesessionEventId_dismiss;
	_BroadcastEvent(event);

	for (auto& gamer : m_gamers)
	{
		InfoLogStream << "race(" << GetID() << ") type(" << (UInt8)GetType() << ") gamer(" << gamer.GetID()
			<< ") sent(" << gamer.GetSentPacketNum() << ") recv(" << gamer.GetRecvPacketNum() << ")." << LogStream::eos;
	}

	if (m_InitPacket)
	{
		Avalon::Packet::Destroy(m_InitPacket);
		m_InitPacket = NULL;
	}

	if (_IsVerifying())
	{
		CLProtocol::VerifyReportRaceDismiss report;
		report.raceId = GetID();
		_SendToVerifyServer(report);
	}

	if (m_VerifyRaceEndPacket)
	{
		Avalon::Packet::Destroy(m_VerifyRaceEndPacket);
		m_VerifyRaceEndPacket = NULL;
	}
}

void Race::Init()
{
    RegisterStatusTimers();
}

void Race::OnTick(const Avalon::Time& now)
{
    if (GetStatus() == RACE_STATUS_WAIT_LOGIN)
    {
        OnTickWaitLoginFinish(now);
    }
    else if (GetStatus() == RACE_STATUS_WAIT_LOAD)
    {
        OnTickWaitLoadFinish(now);
    }
    else if (GetStatus() == RACE_STATUS_RACING)
    {
        OnTickRacing(now);
    }
    else if (GetStatus() == RACE_STATUS_WAIT_RACE_END)
    {
        OnTickWaitRaceEnd(now);
	}

	OnTickReconnecting(now);
	OnTickObserving(now);

	if (GetStatus() != RACE_STATUS_DESTORY)
	{
		_OnTickStatusTimer(now);
	}

	if (GetStatus() != RACE_STATUS_DESTORY && (UInt32)now.Sec() - m_createTime > GetRaceMaxTime())
	{
		OnRaceTimeout();
	}
}

void Race::RegisterStatusTimers()
{
	REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_LOAD, 110, &Race::_OnLoadTimeout);
}

void Race::OnTickWaitLoginFinish(const Avalon::Time& now)
{
    if (_IsAllGamerLogin())
    {
        OnLoginFinish();
        ChangeStatus(RACE_STATUS_WAIT_LOAD);
    }
}

void Race::OnTickWaitLoadFinish(const Avalon::Time& now)
{
    if (_IsAllGamerLoadFinish())
    {
        OnLoadFinish();
        ChangeStatus(RACE_STATUS_RACING);
    }
}

void Race::OnTickRacing(const Avalon::Time& now)
{
    OnTickFrames(now);

	for (auto& gamer : m_gamers)
	{
		if (gamer.IsOffline() || gamer.IsReconnecting())
		{
			continue;
		}

		// 心跳检测
		do 
		{
			UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
			if (!RLApplication::Instance()->IsOpenHeartbeat())
			{
				break;
			}

			// 发包间隔，或者超时时间为0， 就不需要处理了
			if (RLApplication::Instance()->GetHeartbeatIntervalSec() == 0 || RLApplication::Instance()->GetHeartbeatTimeoutSec() == 0)
			{
				break;
			}

			// 先检测发包间隔，超过了就先发包
			if (curTime > gamer.GetLastSendHeartbeatTime() + RLApplication::Instance()->GetHeartbeatIntervalSec())
			{
				// 构建心跳包
				auto packet = Avalon::Packet::Create(0);
				packet->SetID(0);
				packet->SetSize(0);
				gamer.SendPacket(packet, false);
				Avalon::Packet::Destroy(packet);

				// 还没有发过，设置上一次收到心跳包的时间就是现在
				if (gamer.GetLastSendHeartbeatTime() == 0)
				{
					gamer.SetLastRecvHeartbeatTime(curTime);
				}
				gamer.SetLastSendHeartbeatTime(curTime);
			}

			// 检测心跳包是否超时
			if (curTime > gamer.GetLastRecvHeartbeatTime() + RLApplication::Instance()->GetHeartbeatTimeoutSec())
			{
				OnGamerLeaveRace(gamer.GetID(), GAMER_LEAVE_HEARTBEAT_TIMEOUT);
				break;
			}

		} while (0);
		

		if ((UInt32)CURRENT_TIME.Sec() > gamer.GetLastRecvChecksumTime() + 30)
		{
			// 长时间没收到随机数，从单局中踢出
			OnGamerLeaveRace(gamer.GetID(), GAMER_LEAVE_NOT_RECV_CHECKSUM);
		}
	}
}

void Race::OnTickFrames(const Avalon::Time& now)
{
    Avalon::Time elapseTime = now - m_startTime;

    UInt64 msecFrameTime = m_nextLogicFrameSequence * TIME_OF_FRAME_MSEC;
    Int64 diff = elapseTime.MSec() - msecFrameTime;

    if (diff >= 0)
    {
        // 超过帧间隔，就意味着跳帧
        if (diff > TIME_OF_FRAME_MSEC) 
        {
            //InfoLogStream << "race(" << GetID() << ") tick frame diff(" << diff << ") ms, meaning miss some frames!" << LogStream::eos;
        }
        _FinishCurrentFrame(now);
    }

	UInt32 realFrameRate = RaceMgr::Instance()->GetPhysicalFPS();  //得到发包间隔
                                                                            //delta = msecFrameTime / realFrameRate;
    msecFrameTime = m_nextRealFrame * realFrameRate;
    diff = elapseTime.MSec() - msecFrameTime;

    if (diff >= 0)
    {
        // 发送帧数据
        _SyncFrames();
    }
}

void Race::OnTickWaitRaceEnd(const Avalon::Time& now)
{
    if (_IsRecvAllRaceEnd())
    {
        OnRaceEnd();
    }
    else
    {
        OnTickFrames(now);
    }
}

void Race::OnTickReconnecting(const Avalon::Time& now)
{
	if (m_status == RACE_STATUS_DESTORY)
	{
		return;
	}

	for (auto& gamer : m_gamers)
	{
		if (!gamer.IsReconnecting())
		{
			if (gamer.IsOffline() && gamer.GetStatus() != GAMER_STATUS_OFFLINE && gamer.GetReconnEndTime() > 0 && now.Sec() >= gamer.GetReconnEndTime())
			{
				InfoLogStream << "[recon_log] race(" << GetID() << ") gamer(" << gamer.GetID() << ") reconnect timeout..." << LogStream::eos;
				gamer.ChangeStatus(GAMER_STATUS_OFFLINE);
				OnGamerReconnectTimeout(gamer.GetID());
			}
			continue;
		}

		if (GetStatus() == RACE_STATUS_WAIT_LOGIN)
		{
			gamer.SetReconnecting(false);
			gamer.ChangeStatus(GAMER_STATUS_LOGIN_SUCCESS);
		}
		else if (GetStatus() == RACE_STATUS_WAIT_LOAD)
		{
			gamer.SetReconnecting(false);
			gamer.ChangeStatus(GAMER_STATUS_LOAD_FINISH);
		}
		else if (GetStatus() == RACE_STATUS_RACING)
		{
			// 重发帧数据
			if (!_SendFrames(&gamer, gamer.GetSyncReconnectFrame()))
			{
				gamer.SetReconnecting(false);
				gamer.ChangeStatus(GAMER_STATUS_RACING);

				// 添加重连帧数据
				CLProtocol::InputData reconnData;
				reconnData.data1 = FRAME_COMMAND_RECONNECT_END;
				_PushInputDataToCurrentFrame(gamer.GetPos(), reconnData);

				gamer.SetLastRecvChecksumTime();

				//InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ", " << gamer.GetPos() << ") reconnect finish..." << LogStream::eos;
			}
		}
		else if (GetStatus() == RACE_STATUS_WAIT_RACE_END)
		{
			// 重发帧数据
			if (!_SendFrames(&gamer, gamer.GetSyncReconnectFrame()))
			{
				gamer.SetReconnecting(false);
				gamer.ChangeStatus(GAMER_STATUS_WAIT_RACE_END);

				// 添加重连帧数据
				CLProtocol::InputData reconnData;
				reconnData.data1 = FRAME_COMMAND_RECONNECT_END;
				_PushInputDataToCurrentFrame(gamer.GetPos(), reconnData);
			}
		}
		else
		{
			ErrorLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") reconnect, error race staus(" << (UInt8)GetStatus() << ")." << LogStream::eos;
			gamer.Offline(GetID(), 0);
			gamer.ChangeStatus(GAMER_STATUS_OFFLINE);
		}
	}
}

void Race::OnTickObserving(const Avalon::Time& now)
{
	if (!CanObserve())
	{
		return;
	}

	if (m_allFrames.empty())
	{
		if (GetStatus() == RACE_STATUS_DESTORY)
		{
			InfoLogStream << "race(" << GetID() << ") not start, remove all observers." << LogStream::eos;

			// 战斗结束时一个帧都没，直接断开所有观战者的连接
			for (auto& observer : m_observers)
			{
				InfoLogStream << "race(" << GetID() << ") observer(" << observer.GetID() << ") finish, disconnect." << LogStream::eos;
				observer.Offline(GetID(), 0);
			}

			m_observers.clear();
		}
		return;
	}

	UInt32 curFrame = m_allFrames[m_allFrames.size() - 1].sequence;
	if (curFrame <= GetObserveDelay() && GetStatus() != RACE_STATUS_DESTORY)
	{
		return;
	}

	UInt32 canSendLastFrame = GetStatus() == RACE_STATUS_DESTORY ? curFrame : curFrame - GetObserveDelay();
	
	for (auto& observer : m_observers)
	{
		if (observer.IsOffline() || observer.GetLoadProgress() != FULL_LOAD_PROGRESS)
		{
			continue;
		}

		UInt32 lastFrame = observer.GetLastObserveFrame();
		// 一次tick最多发送的帧数
		UInt32 maxFrameNumPreTick = 1000;

		UInt32 remainFrameNum = canSendLastFrame - lastFrame;
		UInt32 frameNum = remainFrameNum > maxFrameNumPreTick ? maxFrameNumPreTick : remainFrameNum;
		UInt32 finalFrame = lastFrame + frameNum;
		CLProtocol::RelaySvrFrameDataNotify protocol;
		if (frameNum > 0)
		{
			for (auto& frame : m_allFrames)
			{
				if (frame.sequence > lastFrame && frame.sequence <= finalFrame)
				{
					protocol.frames.push_back(frame);
					observer.SetLastObserveFrame(frame.sequence);
				}
			}

			if (!protocol.frames.empty())
			{
				observer.SendProtocol(protocol);

				UInt32 startFrame = protocol.frames[0].sequence;
				UInt32 endFrame = protocol.frames[protocol.frames.size() - 1].sequence;

				DebugLogStream << "race(" << GetID() << ") send frame (" << startFrame << ") => (" << endFrame << ") to observer(" << observer.GetID() << ")." << LogStream::eos;
			}
		}

		// 战斗已经结束了
		if (GetStatus() == RACE_STATUS_DESTORY && curFrame == observer.GetLastObserveFrame())
		{
			InfoLogStream << "race(" << GetID() << ") observer(" << observer.GetID() << ") finish, disconnect." << LogStream::eos;
			observer.Offline(GetID(), 0);
		}
	}

	// 将已经离线的玩家都删掉
	m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(), [](const RaceGamer& a) {
		return a.IsOffline();
	}), m_observers.end());
}

void Race::OnChecksumUnsync()
{
	if (m_isUnsync)
	{
		return;
	}

	DebugLogStream << "race(" << GetID() << ") checksum unsync, start to dismiss." << LogStream::eos;
	m_isUnsync = true;
	RaceMgr::Instance()->IncUnsyncRaceNum(GetType());

	if(_IsDismissWhenUnsync())
	{
		if (RLApplication::Instance()->IsSendUnsyncNotify())
		{
			BroadcastNotify(8932, false);
		}

		Dismiss(gamesessionDismissType_framechecksum_different);
	}
}

void Race::OnGamerReconnectTimeout(ObjID_t id)
{
	auto gamer = GetGamerByRoleID(id);
	if (!gamer)
	{
		return;
	}

	gamer->SetReconnEndTime(0);

	// 添加重连帧数据
	CLProtocol::InputData quitRace;
	quitRace.data1 = FRAME_COMMAND_QUIT;
	_PushInputDataToCurrentFrame(gamer->GetPos(), quitRace);
}

void Race::OnRaceTimeout()
{
	if (GetStatus() == RACE_STATUS_DESTORY)
	{
		return;
	}

	DebugLogStream << "race(" << GetID() << ") timeout, start to dismiss." << LogStream::eos;
	Dismiss(gamesessionDismissType_timeout);
}

void Race::OnCreateVerifyReturn(UInt32 nodeid)
{
	m_VSId = nodeid;
	_StartVerify();
}

UInt32 Race::OnObserverLogin(RLPlayer* player, UInt32 startFrame)
{
	if (!player)
	{
		return ErrorCode::SYS_ERROR;
	}

	DebugLogStream << "race(" << GetID() << ") observer(" << player->GetID() << ") login, current ability(" << RLPlayerMgr::Instance()->get_ablity() << ")." << LogStream::eos;

	auto gamer = GetObserverByRoleID(player->GetID());
	if (gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") repeat observer(" << player->GetID() << ") wants to login." << LogStream::eos;
		return ErrorCode::RELAY_OBSERVE_REPEAT_PLAYER;
	}

	if (!CanObserve())
	{
		return ErrorCode::RELAY_OBSERVE_RACE_CANNOT_OBSERVE;
	}

	// 开始观战

	RaceGamer observer;
	observer.SetAccountID(player->get_accId());
	observer.SetID(player->GetID());
	observer.SetPlayer(player);
	observer.SetLastObserveFrame(startFrame);
	observer.SetLoadProgress(0);
	m_observers.push_back(observer);

	return ErrorCode::SUCCESS;
}

UInt32 Race::OnObserverReconnect(RLPlayer* player, UInt32 lastFrame)
{
	return true;
}

void Race::OnObserverDisconnect(ObjID_t roleId)
{
	for (UInt32 i = 0; i < m_observers.size(); i++)
	{
		if (m_observers[i].GetID() == roleId)
		{
			m_observers[i].Offline(roleId, 0);
			m_observers.erase(m_observers.begin() + i);
			InfoLogStream << "race(" << GetID() << ") observer(" << roleId << ") disconnect." << LogStream::eos;
		}
	}
}

bool Race::CanObserve() const
{
	return true;
}

UInt32 Race::GetObserveDelay() const
{
	return 0;
}

void Race::OnReceiveObserveFrames(std::vector<CLProtocol::Frame>& frames)
{
}

UInt32 Race::GetObserveLoad()
{
	return 0;
}

void Race::OnLoginFinish()
{
	DebugLogStream << "race(" << GetID() << ") all player login finish." << LogStream::eos;
	//_ChangeAllGamerStatus(GAMER_STATUS_LOAD_FINISH, true);
	_ChangeAllGamerStatus(GAMER_STATUS_WAIT_LOAD, true);

	for (auto& gamer : m_gamers)
	{
		// 登陆完成时可能有些玩家已经加载完成了
		if (gamer.GetLoadProgress() == FULL_LOAD_PROGRESS)
		{
			gamer.ChangeStatus(GAMER_STATUS_LOAD_FINISH);
		}
	}
}

void Race::OnLoadFinish()
{
	Avalon::Time now = Avalon::Time::CurrentTime();

	DebugLogStream << "race(" << GetID() << ") all player load finish, start at time(" << now.MSec() << ")" << LogStream::eos;

	_ChangeAllGamerStatus(GAMER_STATUS_RACING, true);

	SetStartTime(now);

	GameSessionEvent event;
	event.id = gamesessionEventId_start;
	_BroadcastEvent(event);

	CLProtocol::RelaySvrNotifyGameStart notify;
	notify.gamesessionId = GetID();
	notify.startTime = (UInt32)m_startTime.MSec();
	_BroadcastProtocol(notify, false);

	{
		CLProtocol::InputData gameStart;
		gameStart.data1 = FRAME_COMMAND_GAME_START;
		gameStart.data2 = (UInt32)now.MSec();
		_PushInputDataToCurrentFrame((UInt8)-1, gameStart, true);

		for (auto& gamer : m_gamers)
		{
			gamer.SetLastRecvChecksumTime();
			if (gamer.GetStatus() != GAMER_STATUS_RACING)
			{
				// 把离开的玩家push进去
				CLProtocol::InputData leaveRaceData;
				leaveRaceData.data1 = FRAME_COMMAND_LEAVE;
				_PushInputDataToCurrentFrame(gamer.GetPos(), leaveRaceData);
			}
			else
			{
				// 把玩家的网络环境push进去
				CLProtocol::InputData netNotify;
				netNotify.data1 = FRAME_COMMAND_NET_QUALITY;
				netNotify.data2 = gamer.GetNetQuality();
				_PushInputDataToCurrentFrame(gamer.GetPos(), netNotify);
			}
		}
	}

	RaceMgr::Instance()->IncStartRaceNum(GetType());

	// 创建验证
	_CreateVerifyTask();
}

void Race::SendRaceEndToPlayer(EGameSessionDismissType reason)
{
	/*if (m_startTime == 0)
	{
		return;
	}*/

	CLProtocol::InputData raceEnd;
	raceEnd.data1 = FRAME_COMMAND_RACE_END;
	raceEnd.data2 = reason;

	CLProtocol::FighterInput input;
	input.pos = (UInt8)-1;
	input.input = raceEnd;

	if (m_nextLogicFrameSequence < 2)
	{
		m_nextLogicFrameSequence = 2;
	}
	_PushInputDataToCurrentFrame(-1, raceEnd);
	_FinishCurrentFrame(CURRENT_TIME);
	_SyncFrames();

	/*CLProtocol::Frame frame;
	frame.sequence = m_nextLogicFrameSequence;
	frame.data.push_back(input);

	CLProtocol::RelaySvrFrameDataNotify notify;
	notify.frames.push_back(frame);
	_BroadcastProtocol(notify, true);*/
}

void Race::OnGamerLeaveRace(ObjID_t roleId, GamerLeaveReason reason)
{
	InfoLogStream << "race(" << GetID() << ") gamer(" << roleId << ") leave race reason(" << (UInt32)reason << ")" << LogStream::eos;

	auto gamer = GetGamerByRoleID(roleId);
	if (!gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") gamer(" << roleId << ") leave race, can't find gamer." << LogStream::eos;
		return;
	}

	if (reason == GAMER_LEAVE_UNSYNC)
	{
		//gamer->SendNotify(2000, false);
	}

	// 如果是主动离开，就不能重连了
	if (reason == GAMER_LEAVE_INITIATIVE || reason == GAMER_LEAVE_NOT_RECV_CHECKSUM || reason == GAMER_LEAVE_UNSYNC)
	{
		gamer->Offline(GetID(), 0);
	}
	else
	{
		gamer->Offline(GetID(), DUNGEON_RACE_RECONN_WAIT_TIME);
	}

	if (_IsStatusCanReconnect())
	{
		if (GetStartTime())
		{
			CLProtocol::InputData leaveRaceData;
			leaveRaceData.data1 = FRAME_COMMAND_LEAVE;
			_PushInputDataToCurrentFrame(gamer->GetPos(), leaveRaceData);
		}

		// 如果所有玩家都离开了，等待玩家重连
		if (IsAllGamerLeave())
		{
			InfoLogStream << "race(" << GetID() << ") all fighter offline." << LogStream::eos;
			ChangeStatus(RACE_STATUS_WAIT_RECONNECT);
		}

		return;
	}

	Dismiss(gamesessionDismissType_offline);
}

void Race::OnGamerQuiteRace(ObjID_t roleId)
{
	auto gamer = GetGamerByRoleID(roleId);
	if (!gamer)
	{
		return;
	}

	CLProtocol::InputData quitRace;
	quitRace.data1 = FRAME_COMMAND_QUIT;
	_PushInputDataToCurrentFrame(gamer->GetPos(), quitRace);
}

bool Race::OnGamerReportLoadProgress(RLPlayer* player, UInt8 progress)
{
	if (!player)
	{
		return false;
	}

	DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report load progress(" << progress << ")." << LogStream::eos;

	auto gamer = GetGamerByRoleID(player->GetID());
	if (!gamer)
	{
		auto observer = GetObserverByRoleID(player->GetID());
		if (observer)
		{
			observer->SetLoadProgress(progress);
			return true;
		}
		ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") report load progress." << LogStream::eos;
		return false;
	}

	/*if (gamer->GetStatus() != GAMER_STATUS_WAIT_LOAD)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") invalid status(" << gamer->GetStatus() << ")." << LogStream::eos;
		return false;
	}*/

	gamer->SetLoadProgress(progress);

	CLProtocol::RelaySvrNotifyLoadProgress notify;
	notify.pos = gamer->GetPos();
	notify.progress = progress;
	_BroadcastProtocol(notify);

	if (progress == FULL_LOAD_PROGRESS)
	{
		// 加载完成
		InfoLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") load finish." << LogStream::eos;
		if (GetStatus() == RACE_STATUS_WAIT_LOAD)
		{
			gamer->ChangeStatus(GAMER_STATUS_LOAD_FINISH);
		}
	}

	return true;
}

bool Race::OnGamerLogin(RLPlayer* player)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") want to login, current ability(" << RLPlayerMgr::Instance()->get_ablity() << ")." << LogStream::eos;

    if (!IsValidGamer(player))
    {
        ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") wants to login." << LogStream::eos;
        return false;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") wants to login." << LogStream::eos;
        return false;
    }

    // 必须是等待登陆状态，才能加入登陆
    // 之后如果又断线重连，这里的判断可能需要修改，ToDo...
    if (GetStatus() != RACE_STATUS_WAIT_LOGIN)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") wants to login, invalid race status("
                        << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return false;
    }

    if (gamer->GetStatus() != GAMER_STATUS_WAIT_LOGIN)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") wants to login, invalid gamer status(" 
                        << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return false;
    }

    gamer->Online(player);
    gamer->ChangeStatus(GAMER_STATUS_LOGIN_SUCCESS);

    DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") login success." << LogStream::eos;

	// 登录之后检测下加载情况
	OnTickWaitLoginFinish(CURRENT_TIME);

    return true;
}

bool Race::OnGamerReconnect(RLPlayer* player, UInt32 lastFrame)
{
	if (!player)
	{
		return false;
	}

	DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") request to reconnect lastframe(" << lastFrame << ") curframe(" << m_nextLogicFrameSequence << ")." << LogStream::eos;
	auto gamer = GetGamerByRoleID(player->GetID());
	if (!gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") wants to reconnect." << LogStream::eos;
		return false;
	}

	if (gamer->GetStatus() == GAMER_STATUS_OFFLINE)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") reconnect timeout." << LogStream::eos;
		return false;
	}

	if (!gamer->IsOffline() && gamer->GetStatus() != GAMER_STATUS_WAIT_LOGIN)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") is online." << LogStream::eos;
		return false;
	}

	if (GetStatus() == RACE_STATUS_DESTORY)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") want to reconnect to destoryed race." << LogStream::eos;
		return false;
	}

	if (GetStatus() == RACE_STATUS_RACE_END)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") want to reconnect to race ended race." << LogStream::eos;
		return false;
	}

	if (m_nextLogicFrameSequence < lastFrame)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") want to reconnect, invalid frame(" << lastFrame << ")." << LogStream::eos;
		return false;
	}

	gamer->Online(player);
	gamer->SetReconnecting(true);
	gamer->SetSyncReconnectFrame(lastFrame);
	gamer->SetLastRecvHeartbeatTime((UInt32)CURRENT_TIME.Sec());

	if (GetStatus() == RACE_STATUS_WAIT_RECONNECT)
	{
		ChangeStatus(GetLastStatus());
	}

	m_reconnNum++;

	return true;
}

bool Race::OnGamerLoadFinish(RLPlayer* player)
{
    if (!player)
    {
        return false;
    }

    DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report load finish." << LogStream::eos;

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") report load finish." << LogStream::eos;
        return false;
    }

    // 必须是等待加载状态，才能汇报加载情况
    if (GetStatus() != RACE_STATUS_WAIT_LOAD)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report load finish, invalid race status("
            << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return false;
    }

    if (gamer->GetStatus() != GAMER_STATUS_WAIT_LOAD)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report load finish, invalid gamer status("
            << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return false;
    }

	gamer->Online(player);
    gamer->ChangeStatus(GAMER_STATUS_LOAD_FINISH);

    DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") load finish." << LogStream::eos;

    /*if (_IsAllGamerLoadFinish())
    {
        OnLoadFinish();
    }*/

    return true;
}

void Race::OnRecvGamerInputData(RLPlayer* player, CLProtocol::InputData& input)
{
    if (!player)
    {
        return;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") report input data." << LogStream::eos;
        return;
    }

	gamer->IncRecvPacketNum();
	gamer->SetLastRecvHeartbeatTime((UInt32)CURRENT_TIME.Sec());

	if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RACE_END && GetStatus() != RACE_STATUS_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report input data, invalid race status("
			<< _ShowStatus(GetStatus()) << ")." << LogStream::eos;
		
        return;
    }

	if (gamer->GetStatus() != GAMER_STATUS_RACING && gamer->GetStatus() != GAMER_STATUS_WAIT_RACE_END && gamer->GetStatus() != GAMER_STATUS_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report input data, invalid gamer status("
            << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return;
    }
	
	//InfoLogStream << "rev input data1 = " << input.data1 << LogStream::eos;
	if (input.data1 == FRAME_COMMAND_DOUBLE_PRESS_CONFIG)
	{
		DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report double click config(" << input.data2 << ")." << LogStream::eos;
	}

	bool filter = _HandleOneInputData(player, input);
	if (filter)
	{
		return;
	}

    _PushInputDataToCurrentFrame(gamer->GetPos(), input);

    GameSessionEvent event;
    event.id = gamesessionEventId_fighter_input;
    _BroadcastEvent(event);
}

void Race::OnRecvGamerFrameChecksum(RLPlayer* player, CLProtocol::FrameChecksum& data)
{
    if (!player)
    {
        return;
    }

    /*DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum (" 
                    << data.framesequence << ", " << data.checksum << ")." << LogStream::eos;*/

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") report frame checksum." << LogStream::eos;
        return;
    }

    if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum, invalid race status("
            << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return;
    }

    if (gamer->GetStatus() != GAMER_STATUS_RACING && gamer->GetStatus() != GAMER_STATUS_WAIT_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum, invalid gamer status("
            << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return;
    }

	if (data.framesequence >= m_nextLogicFrameSequence || data.framesequence == 0)
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum, invalid frame("
			<< data.framesequence << "), current frame(" << m_nextLogicFrameSequence << ")." << LogStream::eos;
        // 这里已经不同步了，直接断开连接
#ifdef __CHECK_UNSYNC__
        OnGamerLeaveRace(player->GetID(), GAMER_LEAVE_UNSYNC);
#endif
        return;
    }

    // 已经收到过校验数据了
	if (gamer->HaveFrameChecksum(data.framesequence))
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum, repeat report frame(" 
						<< data.framesequence << ")." << LogStream::eos;

        // 这里已经不同步了，直接断开连接
#ifdef __CHECK_UNSYNC__
        OnGamerLeaveRace(player->GetID(), GAMER_LEAVE_UNSYNC);
#endif
        return;
    }

	if (!gamer->PushFrameChecksum(data.framesequence, data.checksum))
    {
        ErrorLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") report frame checksum, push frame("
			<< data.framesequence << ") checksum data failed." << LogStream::eos;

        // 这里已经不同步了，直接断开连接
#ifdef __CHECK_UNSYNC__
        OnGamerLeaveRace(player->GetID(), GAMER_LEAVE_UNSYNC);
#endif
        return;
    }

	// 如果在验证中，同步到验证服务器
	if (_IsVerifying())
	{
		CLProtocol::VerifyReportChecksum report;
		report.raceId = GetID();
		PlayerChecksumInfo info;
		info.roleId = gamer->GetID();
		ChecksumInfo checkInfo;
		checkInfo.frame = data.framesequence;
		checkInfo.checksum = data.checksum;
		info.checksums.push_back(checkInfo);
		report.playerChecksums.push_back(info);
		_SendToVerifyServer(report);

#ifdef _VERIFY_LOG_
		if (!m_unsyncFrames.empty())
		{
			DebugLogStream << "race(" << GetID() << ") player(" << info.roleId << ") report frame(" << data.framesequence << ")'s checksum to verifyserver." << LogStream::eos;
		}
#endif
	}

	if (_IsRecvAllGamerFrameChecksum(data.framesequence))
    {
		_OnFrameChecksum(data.framesequence);
    }
}

void Race::OnRecvGamerPing(RLPlayer* player, UInt32 ping)
{
	if (!player)
	{
		return;
	}

	auto gamer = GetGamerByRoleID(player->GetID());
	if (!gamer)
	{
		//ErrorLogStream << "race(" << GetID() << ") invalid player(" << player->GetID() << ") report ping." << LogStream::eos;
		return;
	}

	UInt8 netQuality = _GetNetworkLevelByPing(ping);
	if (gamer->GetNetQuality() != netQuality)
	{
		DebugLogStream << "race(" << GetID() << ") player(" << player->GetID() << ") new net queality(" << netQuality << ") ping(" << ping << ")." << LogStream::eos;
		gamer->SetNetQuality(netQuality);
	}

	if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RECONNECT)
	{
		return;
	}

	CLProtocol::InputData netNotify;
	netNotify.data1 = FRAME_COMMAND_NET_QUALITY;
	netNotify.data2 = netQuality;
	_PushInputDataToCurrentFrame(gamer->GetPos(), netNotify);
}

void Race::OnGamerRaceEnd(RLPlayer* player)
{
    if (!player)
    {
        return;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        return;
    }

    gamer->ChangeStatus(GAMER_STATUS_RACE_END);
}

void Race::OnGamerLevelUp(ObjID_t roleId, UInt16 level)
{
	DebugLogStream << "race(" << GetID() << ") player(" << roleId << ") level up." << LogStream::eos;

	auto gamer = GetGamerByRoleID(roleId);
	if (!gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") invalid player(" << roleId << ") level up." << LogStream::eos;
		return;
	}

	if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RACE_END)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << roleId << ") level up, invalid race status("
			<< (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
		return;
	}

	// 添加重连帧数据
	CLProtocol::InputData levelUpData;
	levelUpData.data1 = FRAME_COMMAND_LEVEL_UP;
	levelUpData.data2 = level;
	_PushInputDataToCurrentFrame(gamer->GetPos(), levelUpData);

	m_levelUpNum++;
}

void Race::OnGamerReviveOther(ObjID_t roleId, ObjID_t targetId, UInt64 hpMp)
{
	DebugLogStream << "race(" << GetID() << ") player(" << roleId << ") revive target(" << targetId << ")." 
		<< ", hpMp(" << hpMp << ")" << LogStream::eos;

	// 只有地下城可以复活
	if (GetType() != RACE_DUNGEON)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << roleId << ") revive target(" << targetId << "), invalid race type(" << (UInt8)GetType() << ")." << LogStream::eos;
		return;
	}

	auto gamer = GetGamerByRoleID(roleId);
	if (!gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") invalid player(" << roleId << ") reborn." << LogStream::eos;
		return;
	}

	auto target = GetGamerByRoleID(targetId);
	if (!target)
	{
		ErrorLogStream << "race(" << GetID() << ") player(" << roleId << ") reborn invalid player(" << targetId << ")." << LogStream::eos;
		return;
	}

	// 添加重连帧数据
	CLProtocol::InputData rebornData;
	rebornData.data1 = FRAME_COMMAND_REBORN;
	rebornData.data2 = (UInt32)target->GetPos();
	UInt32 hp = hpMp >> 32;
	UInt32 mp = hpMp & 0xFFFFFFFF;
	rebornData.data3 = hp << 16;
	rebornData.data3 |= mp;

	_PushInputDataToCurrentFrame(gamer->GetPos(), rebornData);

	m_reviveNum++;
}

void Race::OnUseItem(ObjID_t roleId, UInt32 itemId, UInt32 num)
{
	DebugLogStream << "race(" << GetID() << ") player(" << roleId << ") use item(" << itemId << "), num("<< num <<")" << LogStream::eos;

	auto gamer = GetGamerByRoleID(roleId);
	if (!gamer)
	{
		ErrorLogStream << "race(" << GetID() << ") invalid player(" << roleId << ") reborn." << LogStream::eos;
		return;
	}

	// 添加使用道具数据
	CLProtocol::InputData useItemData;
	useItemData.data1 = FRAME_COMMAND_USE_ITEM;
	useItemData.data2 = itemId;

	useItemData.data3 = num;
	

	_PushInputDataToCurrentFrame(gamer->GetPos(), useItemData);

	m_reviveNum++;
}

bool Race::Dismiss(EGameSessionDismissType reason)
{
	InfoLogStream << "race(" << GetID() << ") start to dismiss, reason(" << reason << ")." << LogStream::eos;

	if (GetStatus() == RACE_STATUS_DESTORY)
	{
		return false;
	}

	CheckRaceEnd(reason);

	if (reason == gamesessionDismissType_framechecksum_different)
	{
		BroadcastNotify(2000, false);
	}

    SendRaceEndToMatchServer(reason);

    SendRaceEndToPlayer(reason);

    //broadcast event to everyone
    GameSessionEvent event;
    event.id = gamesessionEventId_dismiss;
    _BroadcastEvent(event);

    ChangeStatus(RACE_STATUS_DESTORY);

	return true;
}

void Race::ChangeStatus(RaceStatus status)
{
    if (GetStatus() == status)
    {
        return;
    }

	m_lastStatus = GetStatus();
    DebugLogStream << "race(" << GetID() << ") change status from (" << m_status << ") to (" << status << ")." << LogStream::eos;
    m_status = status;

	_ActiveStatusTimer(m_lastStatus, status);

	if (status == RACE_STATUS_DESTORY)
	{
		m_destoryTime = (UInt32)CURRENT_TIME.Sec() + 60;
	}
}

RaceGamer* Race::GetGamerByPos(UInt8 pos)
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetPos() == pos)
        {
            return &gamer;
        }
    }

    return NULL;
}

RaceGamer* Race::GetGamerByRoleID(ObjID_t roleId)
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetID() == roleId)
        {
            return &gamer;
        }
    }

    return NULL;
}

void Race::SetGamerByPos(UInt8 pos, RaceGamer& gamer)
{
    m_gamers.push_back(gamer);
}

bool Race::AddGamer(UInt32 accId, ObjID_t roleId, UInt8 pos, UInt32 wsId)
{
    if (GetGamerByPos(pos) || GetGamerByRoleID(roleId))
    {
        return false;
    }

    RaceGamer gamer;
    gamer.SetID(roleId);
    gamer.SetAccountID(accId);
    gamer.SetPos(pos);
    gamer.SetWsID(wsId);
    SetGamerByPos(pos, gamer);
    return true;
}

bool Race::IsValidGamer(RLPlayer* player)
{
    if (!player)
    {
        return false;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        return false;
    }
    
    return true;
}

bool Race::IsAllGamerLeave()
{
    for (auto& gamer : m_gamers)
    {
        if (!gamer.IsOffline())
        {
            return false;
        }
    }

    return true;
}

void Race::SendNotify(UInt16 type, UInt8 color, const char* word, bool disconnect)
{
	if (word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;

	_BroadcastProtocol(notify, true, disconnect);
}

RaceGamer* Race::GetObserverByRoleID(ObjID_t id)
{
	for (auto& observer : m_observers)
	{
		if (observer.GetID() == id)
		{
			return &observer;
		}
	}

	return NULL;
}

UInt32 Race::RandGetWSID() const
{
	for (auto& gamer : m_gamers)
	{
		return gamer.GetWsID();
	}

	return 0;
}

void Race::OnReceiveObserveStartSync()
{
}

void Race::OnObserveRealyAddObservePlayer(UInt64 roleID)
{
}

void Race::_OnLoadTimeout()
{
	for (auto& gamer : m_gamers)
	{
		if (gamer.GetStatus() == GAMER_STATUS_LOAD_FINISH)
		{
			continue;
		}

		if (gamer.IsOffline())
		{
			continue;
		}

		// 玩家加载失败，走离开比赛的流程
		OnGamerLeaveRace(gamer.GetID(), GAMER_LEAVE_LOAD_TIMEOUT);
	}

	// 如果所有人都离开了，那么就解散队伍
	if (IsAllGamerLeave())
	{
		InfoLogStream << "dismiss race(" << GetID() << ") load timeout, all player leave race." << LogStream::eos;

		Dismiss(gamesessionDismissType_wait_timeout);
		return;
	}

	// 剩下的人开始比赛
	OnLoadFinish();
	ChangeStatus(RACE_STATUS_RACING);
}

void Race::_CreateVerifyTask()
{
	if (!m_InitPacket)
	{
		ErrorLogStream << "race(" << GetID() << ") no init packet." << LogStream::eos;
		return;
	}

	if (!_IsNeedVerify())
	{
		InfoLogStream << "race(" << GetID() << ") don't need to verify." << LogStream::eos;
		return;
	}

	auto node = VerifyServerMgr::Instance()->SelectNode();
	if (!node)
	{
		ErrorLogStream << "race(" << GetID() << ") no useful verify node." << LogStream::eos;
		return;
	}

	CLProtocol::VerifyCreateTaskReq req(m_InitPacket);
	req.raceId = GetID();
	req.raceType = (UInt8)GetRaceType();
	node->SendProtocol(req);

	InfoLogStream << "race(" << GetID() << ") request create verify to node(" << node->GetID() << ", " << node->GetAddr().ToString() << ")." << LogStream::eos;

	req.Detach();
}

void Race::_StartVerify()
{
	// 开始验证
	DevLogStream << "race(" << GetID() << ") start verify..." << LogStream::eos;

	// 先把之前产生的帧发到验证服务器
	{
		CLProtocol::VerifyReportFrame report;
		report.raceId = GetID();

		for (auto& frame : m_framesData)
		{
			report.frames.push_back(frame);

			// 一次只发部分帧
			if (report.frames.size() == 1000)
			{
				_SendToVerifyServer(report);
				report.frames.clear();
			}
		}

		if (!report.frames.empty())
		{
			_SendToVerifyServer(report);
		}
	}
	
	// 上传校验帧
	{
		CLProtocol::VerifyReportChecksum report;
		report.raceId = GetID();

		PlayerChecksumInfo playerChecksums[MAX_GAMER_COUNT];
		for (auto& checksum : m_checksumInfoes)
		{
			ChecksumInfo info;
			info.frame = checksum.frame;

			for (int i = 0; i < MAX_GAMER_COUNT; i++)
			{
				if (checksum.checksums[i] != 0)
				{
					info.checksum = checksum.checksums[i];
					playerChecksums[i].checksums.push_back(info);
				}
			}
		}

		for (auto& gamer : m_gamers)
		{
			if (gamer.GetPos() >= MAX_GAMER_COUNT)
			{
				continue;
			}

			auto& info = playerChecksums[gamer.GetPos()];
			info.roleId = gamer.GetID();
			report.playerChecksums.push_back(info);
		}

		_SendToVerifyServer(report);
	}
}

void Race::_SendToVerifyServer(Avalon::Protocol& protocol)
{
	if (m_VSId == 0)
	{
		return;
	}

	try
	{
		VerifyServerMgr::Instance()->SendProtocol(m_VSId, protocol);
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "race(" << GetID() << ") send packet to verify failed, " << e.what() << LogStream::eos;
		// 发送失败之后这一局就不要验了
		m_VSId = 0;
	}
}

void Race::_OnTickStatusTimer(const Avalon::Time& now)
{
    auto info = _GetStatusTimer(GetStatus());
    if (!info || info->active == false || info->timeOutFunc == NULL)
    {
        // 该状态没有定时事件
        return;
    }

    // 判断有没有超时
    if (info->timer.IsTimeout(now))
    {
        DebugLogStream << "race(" << GetID() << ") status(" << _ShowStatus(GetStatus()) << ") timeout." << LogStream::eos;

        // 调用相应的超时处理
        (info->timeOutFunc)();
        info->active = false;
    }
}

void Race::_RegisterStatusTimer(RaceStatus status, UInt32 waitSec, RaceStatusTimer::RaceStatusTimeOutFunc func)
{
    auto info = _GetStatusTimer(status);
	
    if (!info || waitSec == 0 || func == NULL)
    {
        return;
    }

    info->waitTime = waitSec * Avalon::Time::MSECS_OF_SEC;
    info->timeOutFunc = func;
}

void Race::_ActiveStatusTimer(RaceStatus oldStatus, RaceStatus newStatus)
{
    auto old = _GetStatusTimer(oldStatus);
    if (old)
    {
        old->active = false;
    }

    auto info = _GetStatusTimer(newStatus);
    if (!info || info->waitTime == 0 || info->timeOutFunc == NULL)
    {
        // 该状态没有定时事件
        return;
    }

    Avalon::Time now = Avalon::Time::CurrentTime();
    info->active = true;
    info->timer.SetLastTime(now.MSec());
    info->timer.SetInterval(info->waitTime);
}

RaceStatusTimer* Race::_GetStatusTimer(RaceStatus status)
{
    if (status >= RACE_STATUS_NUM)
    {
        return NULL;
    }

    return &m_statusTimer[status];
}

bool Race::_IsValidPos(UInt8 pos)
{
    return pos >= 0 && pos < MAX_GAMER_COUNT;
}

bool Race::_IsAllGamerLogin()
{
    // 先不考虑离线的情况
    // ToDo...

    return _IsAllGamerInSameStatus(GAMER_STATUS_LOGIN_SUCCESS);
}

bool Race::_IsAllGamerLoadFinish()
{
    // 先不考虑离线的情况
    // ToDo...

    return _IsAllGamerInSameStatus(GAMER_STATUS_LOAD_FINISH);
}

bool Race::_IsRecvAllRaceEnd()
{
    // 先不考虑离线的情况
    // ToDo...

    return _IsAllGamerInSameStatus(GAMER_STATUS_RACE_END);
}

bool Race::_IsAllGamerInSameStatus(RaceGamerStatus status, bool isGamerMustInRace)
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetStatus() != status)
        {
            if (gamer.IsOffline() && isGamerMustInRace)
            {
                continue;
            }

            return false;
        }
    }

    return true;
}

bool Race::_IsRecvAllGamerFrameChecksum(UInt32 frame)
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetStatus() == GAMER_STATUS_RACING || gamer.GetStatus() == GAMER_STATUS_WAIT_RACE_END)
        {
            if (gamer.IsOffline())
            {
                continue;
            }

            if (!gamer.HaveFrameChecksum(frame))
            {
                return false;
            }
        }
    }

    return true;
}

void Race::_OnFrameChecksum(UInt32 frame)
{
    m_checkedSyncFrame = frame;

    bool unsync = false;
    UInt32 checksum = 0;
    for (auto& gamer : m_gamers)
    {
        if (gamer.IsOffline() || (gamer.GetStatus() != GAMER_STATUS_RACING && gamer.GetStatus() != GAMER_STATUS_WAIT_RACE_END))
        {
            continue;
        }

        if (checksum == 0)
        {
            checksum = gamer.GetFrameChecksum(frame);
        }
        else if (checksum != gamer.GetFrameChecksum(frame))
        {
            unsync = true;
        }
    }

    // 不同步
    if (unsync)
    {
		m_firstUnsyncFrame = frame;

		for (auto& gamer : m_gamers)
		{
			if (gamer.IsOffline() || (gamer.GetStatus() != GAMER_STATUS_RACING && gamer.GetStatus() != GAMER_STATUS_WAIT_RACE_END))
			{
				continue;
			}

			auto gamerChecksum = gamer.GetFrameChecksum(frame);
			ErrorLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") cheat, invalid checksum(" << gamerChecksum << ") frame(" << frame << ")." << LogStream::eos;
		}

        OnChecksumUnsync();
    }
}

void Race::_ChangeAllGamerStatus(RaceGamerStatus status, bool isGamerMustInRace)
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.IsOffline() && isGamerMustInRace)
        {
            continue;
        }

        gamer.ChangeStatus(status);
    }
}

void Race::_BroadcastEvent(const GameSessionEvent& event)
{
    for (auto& gamer : m_gamers)
    {
        gamer.SendEventToPlayer(event);
    }
}

void Race::_PushInputDataToCurrentFrame(UInt8 pos, const CLProtocol::InputData& data, bool push_front)
{
    CLProtocol::FighterInput input;
    input.pos = pos;
    input.input = data;

	//DebugLogStream << "race(" << GetID() << ") pos(" << pos << ") push frame(" << m_nextLogicFrameSequence << ", " << data.data1 << ")." << LogStream::eos;

	if (push_front)
	{
		std::vector<CLProtocol::FighterInput> tmp;
		tmp.push_back(input);
		for (auto& data : m_currentFrame.data)
		{
			tmp.push_back(data);
		}

		m_currentFrame.data = tmp;
	}
	else
	{
		m_currentFrame.data.push_back(input);
	}
}

void Race::_FinishCurrentFrame(const Avalon::Time& now)
{
    m_currentFrame.sequence = m_nextLogicFrameSequence;
    m_unsyncFrames.push_back(m_currentFrame);
    m_currentFrame.Init();

    m_nextLogicFrameSequence++;
}

void Race::_SyncFrames()
{
    CLProtocol::RelaySvrFrameDataNotify notify;
    notify.frames.clear();
    notify.frames = m_unsyncFrames;

    _BroadcastProtocol(notify, true);

	for (auto& frame : m_unsyncFrames)
	{
		if (!frame.data.empty())
		{
			m_framesData.push_back(frame);
		}
		m_allFrames.push_back(frame);
	}

	// 如果有验证服务器，需要同步到验证服务器
	if (_IsVerifying())
	{
		CLProtocol::VerifyReportFrame report;
		report.raceId = GetID();
		report.frames = m_unsyncFrames;
		_SendToVerifyServer(report);

#ifdef _VERIFY_LOG_
		if(!m_unsyncFrames.empty())
		{
			UInt32 startFrame = m_unsyncFrames[0].sequence;
			UInt32 endFrame = m_unsyncFrames[m_unsyncFrames.size() - 1].sequence;
			DebugLogStream << "race(" << GetID() << ") report frame (" << startFrame << ") => (" << endFrame << ") to verifyserver." << LogStream::eos;
		}
#endif
	}

	_OnSyncFrames(m_unsyncFrames);

    //清空
    m_unsyncFrames.clear();
    m_nextRealFrame++;
}

void Race::_OnSyncFrames(std::vector<CLProtocol::Frame>& unSyncFrame)
{
}

void Race::_BroadcastProtocol(Avalon::Protocol& protocol, bool onlyRacingGamer, bool disconnect)
{
    Avalon::Packet* packet = Avalon::Packet::Create();
    if (NULL == packet)
    {
        return;
    }

    if (!protocol.Encode(packet))
    {
        ErrorLogStream << "gamer(" << GetID() << ") protocol(" << protocol.GetProtocolID() << ") encode fail!" << LogStream::eos;
        Avalon::Packet::Destroy(packet);
        return;
    }

	_BroadcastPacket(packet, onlyRacingGamer, disconnect);
    Avalon::Packet::Destroy(packet);
}

void Race::_BroadcastPacket(Avalon::Packet* packet, bool onlyRacingGamer, bool disconnect)
{
    for (auto& gamer : m_gamers)
    {
		if (!onlyRacingGamer || (!gamer.IsOffline() && !gamer.IsReconnecting()))
		{
			gamer.SendPacket(packet, disconnect);
		}
        
    }
}

const char* Race::_ShowStatus(RaceStatus status)
{
    static const char* s_statusStrs[] = 
    {
        "init",
        "wait login",
        "wait load",
        "racing",
        "wait race end",
		"race end",
		"wait reconnect",
        "destory"
    };

    if (status < sizeof(s_statusStrs) / sizeof(s_statusStrs[0]))
    {
        return s_statusStrs[status];
    }

    return "unknown status";
}

_gameSessionDismissType Race::_GetDismissTypeByLeaveReason(GamerLeaveReason reason)
{
	if (reason == GAMER_LEAVE_UNSYNC)
	{
		return gamesessionDismissType_framechecksum_different;
	}
	else if (reason == GAMER_LEAVE_LOGIN_TIMEOUT)
	{
		return gamesessionDismissType_wait_timeout;
	}
	else
	{
		return gamesessionDismissType_normal;
	}
}

bool Race::_SendFrames(RaceGamer* gamer, UInt32 startFrame)
{
	if (!gamer)
	{
		return false;
	}

	CLProtocol::RelaySvrReconnectFrameData data;
	data.finish = 0;

	UInt32 MAX_FRAME_SIZE = 100;
	for (UInt32 i = 0; i < m_framesData.size() && data.frames.size() < MAX_FRAME_SIZE; i++)
	{
		auto& frame = m_framesData[i];
		if (frame.sequence > startFrame)
		{
			data.frames.push_back(frame);
		}

		if (i == m_framesData.size() - 1)
		{
			data.finish = 1;
		}
		gamer->SetSyncReconnectFrame(frame.sequence);
	}

	gamer->SendProtocol(data);
	return data.finish == 0;
}

bool Race::_IsStatusCanReconnect()
{
	if (GetStatus() == RACE_STATUS_RACING || GetStatus() == RACE_STATUS_WAIT_LOGIN || 
		GetStatus() == RACE_STATUS_WAIT_LOAD || GetStatus() == RACE_STATUS_WAIT_RACE_END)
	{
		return true;
	}

	return false;
}

UInt8 Race::_GetNetworkLevelByPing(UInt32 ping)
{
	if (ping < 100) 
	{
		return 3;
	}
	else if (ping < 200) 
	{
		return 2;
	}
	else 
	{
		return 1;
	}
}

void Race::_BroadcastFrame(UInt8 pos, FrameCommandType type, UInt32 param)
{
	CLProtocol::InputData data;
	data.data1 = type;
	data.data2 = param;

	CLProtocol::FighterInput input;
	input.pos = pos;
	input.input = data;

	CLProtocol::Frame frame;
	frame.sequence = m_nextLogicFrameSequence;
	frame.data.push_back(input);

	CLProtocol::RelaySvrFrameDataNotify notify;
	notify.frames.push_back(frame);
	_BroadcastProtocol(notify, true);
}

bool Race::_HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input)
{
	return false;
}

void Race::_OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum)
{
	// 默认就打印一个日志
	InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") frame(" << frame << ") checksum(" << gamer.GetFrameChecksum(frame) << ") unsync final(" << finalChecksum << ").";
}