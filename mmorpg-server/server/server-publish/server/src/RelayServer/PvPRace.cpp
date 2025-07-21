#include "PvPRace.h"
#include <CLGameSessionProtocol.h>
#include <CLReplayProtocol.h>
#include <CLVerifyProtocol.h>

#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"
#include "RaceMgr.h"
#include "RLApplication.h"

PvPRace::PvPRace(RaceType raceType, RaceInstanceType subType)
: Race(raceType, subType)
{
	m_pkType = PK_NORMAL_1V1;
	m_pkDungeonId = 0;
	m_replayScore = 0;
	m_battleFlag = 0;
}

PvPRace::~PvPRace()
{

}

void PvPRace::RegisterStatusTimers()
{
    Race::RegisterStatusTimers();

    REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_LOGIN, 30, &PvPRace::_OnLoginTimeout);
    REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_RACE_END, 30, &PvPRace::_OnRaceEndTimeout);
	REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_RECONNECT, 60, &PvPRace::_OnWaitReconnectTimeout);
}

void PvPRace::OnLoadFinish()
{
	Race::OnLoadFinish();

	for (auto& gamer : m_gamers)
	{
		CLProtocol::RelayServerNotifyRaceStart notify;
		notify.raceId = GetID();
		notify.addr = RLApplication::Instance()->GetAddr();
		notify.roleId = gamer.GetID();
		notify.raceType = GetRaceType();
		Router::SendToWorldSvr(gamer.GetWsID(), notify);
	}
}

void PvPRace::OnRaceEnd()
{
	m_raceEndTime = CURRENT_TIME;
	CheckRaceEnd(gamesessionDismissType_normal);
	Dismiss(gamesessionDismissType_normal);
}

void PvPRace::OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrEndGameReq& req)
{
    if (!player)
    {
        return;
    }

    DebugLogStream << "race(" << GetID() << ") recv player(" << player->GetID() << ")'s pvp race end request." << LogStream::eos;

    if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID()
            << ")'s pvp race end request, invalid race status(" << (UInt8)GetStatus() << ")." << LogStream::eos;
        return;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID() 
                        << ")'s pvp race end request, but can't find gamer." << LogStream::eos;
        return;
    }

    if (gamer->GetStatus() != GAMER_STATUS_RACING)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID()
            << ")'s pvp race end request, gamer invalid status(" << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return;
    }

    /*gamer->SetRaceResult(req.);
	gamer->SetRemainHp(req.remainHp);
	gamer->SetRemainMp(req.remainMp);
	*/
	gamer->SetPkRaceEndInfo(req.info);
    gamer->ChangeStatus(GAMER_STATUS_RACE_END);

    if (GetStatus() == RACE_STATUS_RACING)
    {
        ChangeStatus(RACE_STATUS_WAIT_RACE_END);
    }

	// 如果在验证中，上传结算包
	if (_IsVerifying())
	{
		CLProtocol::VerifyReportRaceEnd report;
		report.raceId = GetID();
		report.roleId = player->GetID();

		// 发到验证服务器的录像得分为0
		req.info.replayScore = 0;

		if (req.Encode(report.packet))
		{
			_SendToVerifyServer(report);
		}
	}
}

void PvPRace::SendRaceEndToMatchServer(EGameSessionDismissType reason)
{
    CLProtocol::MatchSvrEndGameReq end;
    end.endReason = reason;
    end.gamesessionID = GetID();
	end.raceEndInfo = GetPkRaceEndInfo();
	end.raceType = GetRaceType();

    Router::SendToMatchSvr(end);
}

void PvPRace::SendRaceEndToPlayer(EGameSessionDismissType reason)
{
	Race::SendRaceEndToPlayer(reason);

    CLProtocol::RelaySvrGameResultNotify notify;
    notify.endReason = reason;
    notify.gamesessionID = GetID();

    for (auto& gamer : m_gamers)
    {
        FightergResult result;
        result.pos = gamer.GetPos();
        result.resultFlag = gamer.GetRaceResult();
        result.accId = gamer.GetAccountID();
        result.roleId = gamer.GetID();

        notify.results.push_back(result);
    }

    _BroadcastProtocol(notify, false, true);
}

void PvPRace::CheckRaceEnd(EGameSessionDismissType reason)
{
	std::vector<PkRaceEndInfo> akRaceEndInfo;
	for (auto& gamer : m_gamers)
	{
		auto endInfo = gamer.GetPkRaceEndInfo();
		if (!endInfo.empty())
		{
			akRaceEndInfo.push_back(endInfo);
		}
	}

	bool invalidGame = false;
	// 所有的结算都应该一样
	for (UInt32 i = 0; i < akRaceEndInfo.size(); i++)
	{
		for (UInt32 j = i + 1; j < akRaceEndInfo.size(); j++)
		{
			if (akRaceEndInfo[i] != akRaceEndInfo[j])
			{
				invalidGame = true;

				// 如果结算不一致 ，就用验证服务器的结果
				do
				{
					auto packet = GetVerifyRaceEndPacket();
					if (packet == NULL)
					{
						InfoLogStream << "race(" << GetID() << ") race end info not equal, invalid game." << LogStream::eos;
						break;
					}

					InfoLogStream << "race(" << GetID() << ") race end info not equal, use verify server's race end info." << LogStream::eos;

					try
					{
						Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

						CLProtocol::RelaySvrEndGameReq protocol;
						if (!protocol.Decode(packet))
						{
							ErrorLogStream << "decode race(" << GetID() << ") verify server race end failed." << LogStream::eos;
							break;
						}

						// 默认用的是第一个结算信息
						invalidGame = false;
						if (akRaceEndInfo.empty())
						{
							akRaceEndInfo.push_back(protocol.info);
						}
						else
						{
							akRaceEndInfo[0] = protocol.info;
						}
					}
					catch (Avalon::NetSerializeException& e)
					{
						ErrorLogStream << "serialize race(" << GetID() << ") verify server race end failed." << e.what() << LogStream::eos;
						break;
					}
				} while (0);
				break;
			}
		}
	}

	// 处理登录时闪退情况
	if (akRaceEndInfo.empty())
	{
		std::vector<ObjID_t> loginGamers;
		for (auto& gamer : m_gamers)
		{
			if (gamer.GetStatus() != GAMER_STATUS_WAIT_LOAD && gamer.GetStatus() != GAMER_STATUS_WAIT_LOGIN)
			{
				loginGamers.push_back(gamer.GetID());
				DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") have login." << LogStream::eos;
			}
			else
			{
				DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") have not login." << LogStream::eos;
			}
		}

		// 所有号都登上来了，就是无效局
		if (loginGamers.size() == m_gamers.size())
		{
			invalidGame = true;
			InfoLogStream << "race(" << GetID() << ") all player login, invalid game." << LogStream::eos;
		}
		else
		{
			ObjID_t onlineGamerId = loginGamers.empty() ? 0 : loginGamers[0];

			// 闪退用户强制改成失败
			PkRaceEndInfo endInfo;
			endInfo.gamesessionId = GetID();
			for (auto& gamer : m_gamers)
			{
				PkPlayerRaceEndInfo result;
				result.roleId = gamer.GetID();
				result.pos = gamer.GetPos();
				result.remainHp = HP_MP_FULL_VALUE;
				result.remainMp = HP_MP_FULL_VALUE;
				if (gamer.GetID() == onlineGamerId)
				{
					result.resultFlag = resultBitmask_win;
					InfoLogStream << "race(" << GetID() << ") player(" << gamer.GetID() << ") login, win." << LogStream::eos;
				}
				else
				{
					result.resultFlag = resultBitmask_lose;
					InfoLogStream << "race(" << GetID() << ") player(" << gamer.GetID() << ") not login, lose." << LogStream::eos;
				}
				endInfo.results.push_back(result);
			}

			akRaceEndInfo.push_back(endInfo);
		}
	}

	if (reason == gamesessionDismissType_end_timeout)
	{
		// 如果结算超时，就用验证服务器的结果
		do 
		{
			invalidGame = true;
			auto packet = GetVerifyRaceEndPacket();
			if (packet == NULL)
			{
				InfoLogStream << "race(" << GetID() << ") wait end timeout, invalid game." << LogStream::eos;
				break;
			}

			InfoLogStream << "race(" << GetID() << ") wait end timeout, use verify server's race end info." << LogStream::eos;

			try
			{
				Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

				CLProtocol::RelaySvrEndGameReq protocol;
				if (!protocol.Decode(packet))
				{
					ErrorLogStream << "decode race(" << GetID() << ") verify server race end failed." << LogStream::eos;
					break;
				}

				// 默认用的是第一个结算信息
				invalidGame = false;
				if (akRaceEndInfo.empty())
				{
					akRaceEndInfo.push_back(protocol.info);
				}
				else
				{
					akRaceEndInfo[0] = protocol.info;
				}				
			}
			catch (Avalon::NetSerializeException& e)
			{
				ErrorLogStream << "serialize race(" << GetID() << ") verify server race end failed." << e.what() << LogStream::eos;
				break;
			}
		} while (0);
	}

	if (invalidGame || akRaceEndInfo.empty())
	{
		PkRaceEndInfo endInfo;
		endInfo.gamesessionId = GetID();
	
		for (auto& gamer : m_gamers)
		{
			gamer.SetRaceResult(resultBitmask_invalid);

			PkPlayerRaceEndInfo result;
			result.roleId = gamer.GetID();
			result.pos = gamer.GetPos();
			result.remainHp = HP_MP_FULL_VALUE;
			result.remainMp = HP_MP_FULL_VALUE;
			result.resultFlag = resultBitmask_invalid;

			if (invalidGame)
			{
				InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") invalid game." << LogStream::eos;
			}
			else
			{
				InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") race end info empty, invalid game." << LogStream::eos;
			}
			
			endInfo.results.push_back(result);
		}
		SetPkRaceEndInfo(endInfo);
	}
	else
	{
		for (auto& info : akRaceEndInfo[0].results)
		{
			auto gamer = GetGamerByRoleID(info.roleId);
			if (gamer)
			{
				gamer->SetRaceResult(info.resultFlag);

				InfoLogStream << "race(" << GetID() << ") player(" << gamer->GetID() << ") result(" << info.resultFlag << ")." << LogStream::eos;
			}
		}
		SetPkRaceEndInfo(akRaceEndInfo[0]);
	}
}

bool PvPRace::Dismiss(EGameSessionDismissType reason)
{
	if (!Race::Dismiss(reason))
	{
		return false;
	}
	
	// 上报录像简介
	{
		// 3v3乱斗暂时不上报录像
		if (GetPkType() != PK_3V3_TUMBLE)
		{
			CLProtocol::ReplayReportRace report;
			if (_GetReplayInfo(report.info))
			{
				Router::SendToReplaySvr(report);
			}
		}
	}

	return true;
}

bool PvPRace::_IsNeedVerify()
{
	return RLApplication::Instance()->IsVerifyPK();
}

void PvPRace::_OnLoginTimeout()
{
	CLProtocol::RelaySvrNotifyGameStart notify;
	notify.gamesessionId = GetID();
	notify.startTime = CURRENT_TIME.MSec();
	_BroadcastProtocol(notify);

	m_startTime = CURRENT_TIME;
	UInt32 startTime = (UInt32)m_startTime.Sec();
	_BroadcastFrame((UInt8)-1, FRAME_COMMAND_GAME_START, startTime);

	for (auto& gamer : m_gamers)
	{
		if (gamer.GetStatus() == GAMER_STATUS_LOGIN_SUCCESS)
		{
			CLProtocol::RelayServerNotifyRaceStart notify;
			notify.raceId = GetID();
			notify.addr = RLApplication::Instance()->GetAddr();
			notify.roleId = gamer.GetID();
			notify.raceType = GetRaceType();
			Router::SendToWorldSvr(gamer.GetWsID(), notify);
		}
		else
		{
			gamer.Offline(GetID(), 0);
		}
	}

	if (_IsNeedDismiss())
	{
		Dismiss(gamesessionDismissType_wait_timeout);
	}
}

void PvPRace::_OnLoadTimeout()
{
	CLProtocol::RelaySvrNotifyGameStart notify;
	notify.gamesessionId = GetID();
	notify.startTime = CURRENT_TIME.MSec();
	_BroadcastProtocol(notify);

	m_startTime = CURRENT_TIME;
	UInt32 startTime = (UInt32)m_startTime.Sec();
	_BroadcastFrame((UInt8)-1, FRAME_COMMAND_GAME_START, startTime);

	for (auto& gamer : m_gamers)
	{
		if (gamer.GetStatus() == GAMER_STATUS_LOAD_FINISH)
		{
			CLProtocol::RelayServerNotifyRaceStart notify;
			notify.raceId = GetID();
			notify.addr = RLApplication::Instance()->GetAddr();
			notify.roleId = gamer.GetID();
			notify.raceType = GetRaceType();
			Router::SendToWorldSvr(gamer.GetWsID(), notify);
		}
		else
		{
			gamer.Offline(GetID(), 0);
		}
	}

	if (_IsNeedDismiss())
	{
		Dismiss(gamesessionDismissType_wait_timeout);
	}
}

void PvPRace::_OnRaceEndTimeout()
{
    Dismiss(gamesessionDismissType_end_timeout);
}

void PvPRace::_OnWaitReconnectTimeout()
{
	DebugLogStream << "race(" << GetID() << ") wait gamer reconnect timeout." << LogStream::eos;

	if (_IsNeedDismiss())
	{
		Dismiss(gamesessionDismissType_offline);
	}
}

void PvPRace::OnGamerReconnectTimeout(ObjID_t id)
{
	auto gamer = GetGamerByRoleID(id);
	if (!gamer)
	{
		return;
	}

	gamer->SetReconnEndTime(0);

	PkRaceEndInfo endInfo;
	endInfo.gamesessionId = GetID();
	for (auto& gamer : m_gamers)
	{
		gamer.ChangeStatus(GAMER_STATUS_RACE_END);

		PkPlayerRaceEndInfo result;
		result.roleId = gamer.GetID();
		result.pos = gamer.GetPos();
		result.remainHp = HP_MP_FULL_VALUE;
		result.remainMp = HP_MP_FULL_VALUE;
		if (gamer.GetID() == id)
		{
			result.resultFlag = resultBitmask_lose;
		}
		else
		{
			result.resultFlag = resultBitmask_win;
		}
		endInfo.results.push_back(result);
	}

	for (auto& gamer : m_gamers)
	{
		gamer.ChangeStatus(GAMER_STATUS_RACE_END);
		gamer.SetPkRaceEndInfo(endInfo);
	}

	Dismiss(gamesessionDismissType_offline);
}

RaceGamer* PvPRace::_GetOpponent(ObjID_t gamerId)
{
    // PVP肯定是1V1的，所以只要不是指定的玩家，那么就一定是对手
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetID() != gamerId)
        {
            return &gamer;
        }
    }

    return NULL;
}

bool PvPRace::_GetReplayInfo(ReplayInfo& info)
{
	if (!RLApplication::Instance()->IsOpenReplay())
	{
		return false;
	}

	info.version = RLApplication::Instance()->GetVersion();
	info.raceId = GetID();
	info.type = GetPkType();
	info.recordTime = (UInt32)CURRENT_TIME.Sec();

	if (m_racePlayerInfoes.empty())
	{
		ErrorLogStream << "race(" << GetID() << ") player info is empty." << LogStream::eos;
		return false;
	}

	ObjID_t player1 = m_racePlayerInfoes[0].roleId;
	UInt8 resultFlag = 0;

	const auto& endInfo = GetPkRaceEndInfo();
	for (auto& result : endInfo.results)
	{
		if (result.roleId == player1)
		{
			resultFlag = result.resultFlag;
			break;
		}
	}

	if (resultFlag == 0)
	{
		InfoLogStream << "race(" << GetID() << ") invalid game." << LogStream::eos;
		return false;
	}

	info.result = _GetPkRaceResultByFlag(resultFlag);
	for (auto& player : m_racePlayerInfoes)
	{
		ReplayFighterInfo fighter;
		fighter.roleId = player.roleId;
		fighter.name = player.name;
		fighter.occu = player.occu;
		fighter.level = player.level;
		fighter.pos = player.pos;
		fighter.seasonLevel = player.seasonLevel;
		fighter.seasonStars = player.seasonStar;

		info.fighters.push_back(fighter);
	}

	// 录像评分应该是从客户端上报
	m_replayScore = endInfo.replayScore;
	info.score = m_replayScore;

	return true;
}

ReplayFile PvPRace::GenReplayFile()
{
	ReplayFile replay;
	// 头
	{
		auto& header = replay.header;
		header.magicCode = 0;
		header.version = RLApplication::Instance()->GetVersion();
		header.startTime = (UInt32)m_startTime.Sec();
		header.sessionId = GetID();
		header.raceType = (UInt8)GetRaceType();
		header.pkDungeonId = GetPkDungeonID();
		header.duration = (UInt32)m_raceEndTime.Sec() - (UInt32)m_startTime.Sec();
		header.players = m_racePlayerInfoes;
		header.battleFlag = GetBattleFlag();
	}

	// 帧数据
	{
		for (auto& frame : m_framesData)
		{
			if (frame.data.empty())
			{
				continue;
			}

			CLProtocol::Frame tmpFrame;
			tmpFrame.sequence = frame.sequence;
			for (auto& data : frame.data)
			{
				if (data.input.data1 == FRAME_COMMAND_NET_QUALITY)
				{
					continue;
				}

				tmpFrame.data.push_back(data);
			}

			if (tmpFrame.data.empty())
			{
				continue;
			}

			replay.frames.push_back(tmpFrame);
		}
	}

	// 结果
	{
		for (auto& result : m_pkRaceEndInfo.results)
		{
			ReplayRaceResult replayResult;
			replayResult.pos = result.pos;
			replayResult.result = result.resultFlag;
			replay.results.push_back(replayResult);
		}
	}

	return replay;
}

PkRaceResult PvPRace::_GetPkRaceResultByFlag(UInt8 flag)
{
	if (flag & resultBitmask_win)
	{
		return PKRR_WIN;
	}
	else if (flag & resultBitmask_lose)
	{
		return PKRR_LOSE;
	}
	else if (flag & resultBitmask_draw)
	{
		return PKRR_DOGFALL;
	}

	return PKRR_INVALID;
}

bool PvPRace::_IsNeedDismiss()
{
	return true;
}