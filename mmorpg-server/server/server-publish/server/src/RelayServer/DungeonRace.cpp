#include "DungeonRace.h"
#include <CLGameSessionProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLDungeonVerifyProbDataEntry.h>
#include <CLDungeonProtocol.h>

#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"
#include "RaceMgr.h"
#include "RLApplication.h"

DungeonRace::DungeonRace(RaceType type)
    : Race(type, RACE_DUNGEON)
{
	m_DungeonId = 0;
	m_BossPhase = 0;
	m_UseVerifyRaceEnd = false;
	m_StartWaitVerifyRaceEndTime = 0;
}

DungeonRace::~DungeonRace()
{

}

void DungeonRace::RegisterStatusTimers()
{
    Race::RegisterStatusTimers();

    REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_LOGIN, 30, &DungeonRace::_OnLoginTimeout);
	REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_RECONNECT, 60, &DungeonRace::_OnWaitReconnectTimeout);
	REGISTER_STATUS_TIMER(RACE_STATUS_RACE_END, 15, &DungeonRace::_OnWiatGamerLeaveRaceTimeout);
	REGISTER_STATUS_TIMER(RACE_STATUS_WAIT_RACE_END, 20, &DungeonRace::_OnWiatRaceEnd);
	
}

void DungeonRace::OnRaceEnd()
{
	m_raceEndTime = CURRENT_TIME;
	CheckRaceEnd(gamesessionDismissType_normal);
    Dismiss(gamesessionDismissType_normal);
}

void DungeonRace::OnRecvRaceEndMsg(RLPlayer* player, CLProtocol::RelaySvrDungeonRaceEndReq& req)
{
    if (!player)
    {
        return;
    }

    DebugLogStream << "race(" << GetID() << ") recv player(" << player->GetID() << ")'s dungeon race end request." << LogStream::eos;

    if (GetStatus() != RACE_STATUS_RACING && GetStatus() != RACE_STATUS_WAIT_RACE_END)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID()
            << ")'s dungeon race end request, invalid race status(" << _ShowStatus(GetStatus()) << ")." << LogStream::eos;
        return;
    }

    auto gamer = GetGamerByRoleID(player->GetID());
    if (!gamer)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID()
            << ")'s dungeon race end request, but can't find gamer." << LogStream::eos;
        return;
    }

    if (gamer->GetStatus() != GAMER_STATUS_RACING)
    {
        ErrorLogStream << "race(" << GetID() << ") recv player(" << player->GetID()
            << ")'s dungeon race end request, gamer invalid status(" << (UInt8)gamer->GetStatus() << ")." << LogStream::eos;
        return;
    }

	if (m_isUnsync)
	{
		ErrorLogStream << "[unsync_data] race(" << GetID() << ") dungeon(" << req.raceEndInfo.dungeonId << ") reconn(" << m_reconnNum << ") revive(" 
						<< m_reviveNum << ") levelup(" << m_levelUpNum << ") unsync frame(" << m_firstUnsyncFrame << ") current frame("
						<< m_nextLogicFrameSequence << ")." << LogStream::eos;
	}

    gamer->SetDungeonRaceEndInfo(req.raceEndInfo);
    gamer->ChangeStatus(GAMER_STATUS_RACE_END);

    if (GetStatus() == RACE_STATUS_RACING)
    {
		m_StartWaitVerifyRaceEndTime = (UInt32)CURRENT_TIME.Sec();
        ChangeStatus(RACE_STATUS_WAIT_RACE_END);
    }

	// �������֤�У��ϴ������
	if (_IsVerifying())
	{
		CLProtocol::VerifyReportRaceEnd report;
		report.raceId = GetID();
		report.roleId = player->GetID();

		// ���md5
		for (auto& info : req.raceEndInfo.infoes)
		{
			memset(info.md5, 0, sizeof(info.md5));
		}

		if (req.Encode(report.packet))
		{
			_SendToVerifyServer(report);
		}
	}
}

void DungeonRace::SendRaceEndToMatchServer(EGameSessionDismissType reason)
{
    CLProtocol::MatchSvrDungeonRaceEnd  end;
    end.reason = reason;
    end.sessionID = GetID();

    for (auto& gamer : m_gamers)
    {
        FightergResult result;
        if (gamer.GetStatus() == GAMER_STATUS_RACE_END)
        {
            end.raceEndInfo = gamer.GetDungeonRaceEndInfo();
            break;
        }
    }

	// ����Ǳ���Ҫ�ȴ���֤����Ĺؿ��������ܵ���֤���������㣬������֤�������Ľ���
	if (RLApplication::Instance()->IsDungeonNeedWaitVerifyRaceEnd(m_DungeonId) && this->IsUseVerifyRaceEnd())
	{
		InfoLogStream << "race(" << GetID() << ") use verify race end." << LogStream::eos;
		end.raceEndInfo = GetDungeonRaceEndInfo();
	}

	end.raceEndInfo.gamesessionId = GetID();

	std::vector<DungeonPlayerRaceEndInfo> playerEndInfoes;
	for (auto info : end.raceEndInfo.infoes)
	{
		auto gamer = this->GetGamerByRoleID(info.roleId);
		if (gamer && gamer->GetStatus() == GAMER_STATUS_RACE_END)
		{
			playerEndInfoes.push_back(info);
		}
		else
		{
			DebugLogStream << "race(" << GetID() << ") gamer(" << info.roleId << ") have not receive race end." << LogStream::eos;
		}
	}
	end.raceEndInfo.infoes = playerEndInfoes;

	if (_IsCrossDungeon())
	{
		Router::SendToDungeonSvr(end);
	}
	else
	{
		Router::SendToMatchSvr(end);
	}
}

void DungeonRace::SendRaceEndToPlayer(EGameSessionDismissType reason)
{
	Race::SendRaceEndToPlayer(reason);

    CLProtocol::RelaySvrRaceEndNotify notify;
    notify.endReason = reason;
    _BroadcastProtocol(notify, false, true);
}

bool DungeonRace::Dismiss(EGameSessionDismissType reason)
{
	InfoLogStream << "race(" << GetID() << ") start to dismiss, reason(" << reason << ")." << LogStream::eos;

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

	if (reason == gamesessionDismissType_normal)
	{
		ChangeStatus(RACE_STATUS_RACE_END);
	}
	else
	{
		ChangeStatus(RACE_STATUS_DESTORY);
	}
	
	return true;
}

void DungeonRace::CheckRaceEnd(EGameSessionDismissType reason)
{
	if (!RLApplication::Instance()->IsDungeonNeedWaitVerifyRaceEnd(m_DungeonId))
	{
		return;
	}

	if (GetVerifyRaceEndPacket() == NULL)
	{
		InfoLogStream << "race(" << GetID() << ") dont recv verify race end." << LogStream::eos;
		return;
	}

	DungeonRaceEndInfo vsRaceEnd;
	try
	{
		auto packet = GetVerifyRaceEndPacket();
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		CLProtocol::RelaySvrDungeonRaceEndReq protocol;
		if (!protocol.Decode(packet))
		{
			ErrorLogStream << "decode race(" << GetID() << ") verify server race end failed." << LogStream::eos;
			return;
		}

		vsRaceEnd = protocol.raceEndInfo;
		InfoLogStream << "race(" << GetID() << ") verify race end:" << vsRaceEnd.ToString() << LogStream::eos;
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "serialize race(" << GetID() << ") verify server race end failed." << e.what() << LogStream::eos;
		return;
	}

	UseVerifyRaceEnd();
	SetDungeonRaceEndInfo(vsRaceEnd);

	// �ȽϽ��㣬����ֻ�Ǵ�ӡ����־
	for (auto& gamer : m_gamers)
	{
		if (gamer.GetStatus() != GAMER_STATUS_RACE_END)
		{
			continue;
		}

		auto& endInfo = gamer.GetDungeonRaceEndInfo();
		InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") race end:" << endInfo.ToString() << LogStream::eos;

		if (endInfo != vsRaceEnd)
		{
			InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") race end check failed." << LogStream::eos;
		}
	}
}

void DungeonRace::OnRecvInitPacket()
{
	if (!m_InitPacket)
	{
		return;
	}

	CLProtocol::SceneDungeonStartRes init;
	try
	{
		if (!init.Decode(m_InitPacket))
		{
			return;
		}

		m_DungeonId = init.dungeonId;
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << e.what();
	}
}

void DungeonRace::_OnFrameChecksum(UInt32 frame)
{
	if (!RLApplication::Instance()->IsOpenNewChecksumCheck())
	{
		Race::_OnFrameChecksum(frame);
		return;
	}


	m_checkedSyncFrame = frame;

	// ͳ�����е������
	struct ChecksumSta
	{
		ChecksumSta() { memset(this, 0, sizeof(*this)); }

		// ��Ҫ�Ӵ�С����
		bool operator<(const ChecksumSta& other) { return num > other.num; }

		UInt32 checksum;
		UInt32 num;
	};
	std::vector<ChecksumSta> checksums;
	UInt32 checksumNum = 0;

	for (auto& gamer : m_gamers)
	{
		if (gamer.IsOffline() || (gamer.GetStatus() != GAMER_STATUS_RACING && gamer.GetStatus() != GAMER_STATUS_WAIT_RACE_END))
		{
			continue;
		}

		checksumNum++;
		UInt32 gamerChecksum = gamer.GetFrameChecksum(frame);
		bool isInsert = true;
		for (UInt32 i = 0; i < checksums.size(); i++)
		{
			if (checksums[i].checksum == gamerChecksum)
			{
				checksums[i].num++;
				isInsert = false;
				break;
			}
		}

		if (isInsert)
		{
			ChecksumSta info;
			info.checksum = gamerChecksum;
			info.num = 1;
			checksums.push_back(info);
		}
	}

	// �µ���������
	// ��������������Ӵ�С����(��ǰ��ĸ�������)
	std::sort(checksums.begin(), checksums.end());

	// ������������<=1��˵��û�в�ͬ��
	if (checksums.size() <= 1)
	{
		return;
	}

	// ��ֹһ���������������������ǰ������ϵľ�����������Ϊ׼����Ϊ������������Ǵ����
	auto finalChecksum = checksums[0];
	if (finalChecksum.num * 2 > checksumNum)
	{
		// �ҵ������������һ�µ����
		for (auto& gamer : m_gamers)
		{
			if (gamer.IsOffline() || (gamer.GetStatus() != GAMER_STATUS_RACING && gamer.GetStatus() != GAMER_STATUS_WAIT_RACE_END))
			{
				continue;
			}

			UInt32 gamerChecksum = gamer.GetFrameChecksum(frame);
			if (gamerChecksum != finalChecksum.checksum)
			{
				_OnPlayerChecksumUnsync(gamer, frame, finalChecksum.checksum);
			}
		}
	}
	else
	{
		// ���ֲ�ͬ��
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
	return;
}

void DungeonRace::_OnLoginTimeout()
{
    for (auto& gamer : m_gamers)
    {
        if (gamer.GetStatus() == GAMER_STATUS_LOGIN_SUCCESS)
        {
            continue;
        }

        if (gamer.IsOffline())
        {
            continue;
        }

        // ��Ҽ���ʧ�ܣ����뿪����������
        OnGamerLeaveRace(gamer.GetID(), GAMER_LEAVE_LOGIN_TIMEOUT);
    }

    // ��������˶��뿪�ˣ���ô�ͽ�ɢ����
    if (IsAllGamerLeave())
    {
        Dismiss(gamesessionDismissType_wait_timeout);
        return;
    }

    // ʣ�µ��˿�ʼ����
    OnLoginFinish();
	ChangeStatus(RACE_STATUS_WAIT_LOAD);
}

void DungeonRace::_OnWaitReconnectTimeout()
{
	DebugLogStream << "race(" << GetID() << ") wait gamer reconnect timeout." << LogStream::eos;
	Dismiss(gamesessionDismissType_offline);
}

void DungeonRace::_OnWiatGamerLeaveRaceTimeout()
{
	ChangeStatus(RACE_STATUS_DESTORY);
}

void DungeonRace::_OnWiatRaceEnd()
{
	DebugLogStream << "race(" << GetID() << ") wait race end timeout." << LogStream::eos;
	Dismiss(gamesessionDismissType_end_timeout);
}

bool DungeonRace::_IsNeedVerify()
{
	if (!RLApplication::Instance()->IsOpenVerifyDungeon())
	{
		return false;
	}

	auto data = DungeonVerifyProbDataEntryMgr::Instance()->FindEntry(m_DungeonId);
	if (!data)
	{
		// ����Ҳ�����һ��Ҫ��֤
		return true;
	}

	if (Avalon::Random::SelectInHundred(data->prob))
	{
		return true;
	}

	return false;
}

bool DungeonRace::_IsCrossDungeon()
{
	return RT_DUNGEON_CROSS == GetRaceType();
}

bool DungeonRace::_IsRecvAllRaceEnd()
{
	if (!Race::_IsRecvAllRaceEnd())
	{
		return false;
	}

	// �������Ҫ�ȴ�����Ĺؿ�����Ҫ�ȴ���֤�������Ľ���

	// ����Ҫ�ȴ�
	if (!RLApplication::Instance()->IsDungeonNeedWaitVerifyRaceEnd(m_DungeonId))
	{
		return true;
	}

	// ������֤
	if (!_IsVerifying())
	{
		return true;
	}

	// �Ѿ��յ���֤������
	if (GetVerifyRaceEndPacket() != NULL)
	{
		return true;
	}

	// �ж��Ƿ�ʱ
	if (m_StartWaitVerifyRaceEndTime + RLApplication::Instance()->GetDungeonWaitVerifyRaceEndSec() < (UInt32)CURRENT_TIME.Sec())
	{
		return true;
	}
	
	return false;
}

bool DungeonRace::_HandleOneInputData(RLPlayer* player, CLProtocol::InputData& input)
{
	if (!player)
	{
		return false;
	}
	
	bool filter = false;

	filter = Race::_HandleOneInputData(player, input);

	if (filter)
	{
		return true;
	}

	switch (input.data1)
	{
	case FRAME_COMMAND_BOSS_PHASE_CHANGE:
	{
		_OnBossPhaseChange(input.data2);
		filter = true;
		break;
	}	
	default:
		break;
	}

	return filter;
}

void DungeonRace::_OnBossPhaseChange(UInt32 bossPhase)
{
	InfoLogStream << "race id : " << GetID() << ", bossPhase : " << bossPhase << LogStream::eos;
	if (bossPhase == m_BossPhase)
	{
		return;
	}
	m_BossPhase = bossPhase;

	CLProtocol::RelayDungeonBossPhaseChange protocol;
	protocol.raceSessionId = GetID();
	protocol.bossphase = m_BossPhase;

	if (_IsCrossDungeon())
	{
		Router::SendToDungeonSvr(protocol);
	}
	else
	{
	}
}

void DungeonRace::OnNotifyDestoryDungeon(ObjID_t destoryDungeonId)
{
	InfoLogStream << "session : " << GetID() << ", destoryDungeonId : " << destoryDungeonId << LogStream::eos;
	if (GetStatus() != RACE_STATUS_RACING)
	{
		//return;
	}
	CLProtocol::InputData destoryDungeon;
	destoryDungeon.data1 = FRAME_COMMAND_DESTORY_DUNGEON;
	destoryDungeon.data2 = destoryDungeonId;
	_PushInputDataToCurrentFrame((UInt8)-1, destoryDungeon);
}

void DungeonRace::OnLogicEndDungeon(UInt32 reason)
{
	InfoLogStream << "OnLogicEndDungeon session : " << GetID() << ", reason : " << reason << LogStream::eos;
	if (GetStatus() != RACE_STATUS_RACING)
	{
		//return;
	}
	CLProtocol::InputData endDungeon;
	endDungeon.data1 = FRAME_COMMAND_TEAMCOPY_RACE_END;
	endDungeon.data2 = reason;
	_PushInputDataToCurrentFrame((UInt8)-1, endDungeon);
}

void DungeonRace::_OnPlayerChecksumUnsync(RaceGamer& gamer, UInt32 frame, UInt32 finalChecksum)
{
	InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") frame(" << frame << ") checksum(" << gamer.GetFrameChecksum(frame) << ") unsync final(" << finalChecksum << ").";

	if (RLApplication::Instance()->IsKickPlayerWhenDungeonUnsync())
	{
		InfoLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") kick player." << LogStream::eos;;
		gamer.SendNotify(2000, true);
	}
}

void DungeonRace::OnTeamCopyNotifyBimsEnergyProgress(UInt32 progress, UInt32 dungeonId)
{
	InfoLogStream << "teamcopy notify bims energy progress session : " << GetID() << ", progress : " << progress << LogStream::eos;
	if (GetStatus() != RACE_STATUS_RACING)
	{
		//return;
	}
	CLProtocol::InputData endDungeon;
	endDungeon.data1 = FRAME_COMMAND_TEAMCOPY_BIMS_PROGRESS;
	endDungeon.data2 = progress;
	endDungeon.data3 = dungeonId;
	_PushInputDataToCurrentFrame((UInt8)-1, endDungeon);
}

void DungeonRace::OnAddReviveCnt(UInt32 reviveCnt)
{
	InfoLogStream << "teamcopy notify add revive cnt session : " << GetID() << ", reviveCnt : " << reviveCnt << LogStream::eos;
	if (GetStatus() != RACE_STATUS_RACING)
	{
		//return;
	}

	CLProtocol::InputData data;
	data.data1 = FRAME_COMMAND_TEAMCOPY_REVIVE_CNT;
	data.data2 = reviveCnt;
	_PushInputDataToCurrentFrame((UInt8)-1, data);
}