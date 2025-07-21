#include "TCSDungeonMgr.h"

#include <CLDungeonProtocol.h>
#include <CLTeamCopyBossDataEntry.h>
#include "TCSNetwork.h"
#include "TCSDungeonRace.h"
#include "TCSTeamMgr.h"

TCSDungeonMgr::TCSDungeonMgr()
{
	m_DungeonSvrPtr = NULL;
}

TCSDungeonMgr::~TCSDungeonMgr()
{
	for (auto& iter : m_DungeonRaceMap)
	{
		if (iter.second)
		{
			CL_SAFE_DELETE(iter.second);
		}
	}

	m_DungeonRaceMap.clear();
	m_TeamSquadRaceMap.clear();
}

void TCSDungeonMgr::OnConnected(UInt32 id)
{
	m_DungeonSvrPtr = TCSNetwork::Instance()->FindConnection(id);
}

void TCSDungeonMgr::OnDisconnected(UInt32 id)
{
	m_DungeonSvrPtr = NULL;
}

void TCSDungeonMgr::SendToDungeonSvr(Avalon::Packet* packet)
{
	if (m_DungeonSvrPtr != NULL)
	{
		m_DungeonSvrPtr->SendPacket(packet);
	}
}

void TCSDungeonMgr::SendToDungeonSvr(Avalon::Protocol& protocol)
{
	if (m_DungeonSvrPtr != NULL)
	{
		CL_SEND_PROTOCOL(m_DungeonSvrPtr, protocol);
	}
}

void TCSDungeonMgr::AddRace(UInt64 raceId, UInt32 teamSquadId, UInt32 fieldId)
{
	TCSDungeonRace* racePtr = FindRaceByRaceId(raceId);
	if (racePtr != NULL)
	{
		ErrorLogStream << "add repeated race id:" << raceId << " teamSquadId:" << teamSquadId << " fieldId:" << fieldId << LogStream::eos;
		return;
	}

	TCSDungeonRace* race = new TCSDungeonRace();
	race->SetRaceId(raceId);
	race->SetTeamSquadId(teamSquadId);
	race->SetFieldId(fieldId);
	race->SetCreateTime(CURRENT_TIME.Sec());

	m_DungeonRaceMap[raceId] = race;
	m_TeamSquadRaceMap[teamSquadId] = race;
}

void TCSDungeonMgr::RemoveRace(UInt64 raceId)
{
	UInt32 teamSquadId = 0;
	auto iter = m_DungeonRaceMap.find(raceId);
	if (iter != m_DungeonRaceMap.end())
	{
		teamSquadId = iter->second->GetTeamSquadId();
		CL_SAFE_DELETE(iter->second);
		m_DungeonRaceMap.erase(raceId);
	}

	if (teamSquadId != 0)
	{
		m_TeamSquadRaceMap.erase(teamSquadId);
	}

	m_RacePhaseMap.erase(raceId);
}

void TCSDungeonMgr::HandlerBossPhase(UInt32 bossPhase, UInt64 raceSessionId)
{
	//DebugLogStream << "raceId:" << raceSessionId << " bossPhase:" << bossPhase << LogStream::eos;

	//TCSDungeonRace* race = FindRaceByRaceId(raceSessionId);
	//if (NULL == race)
	//{
	//	ErrorLogStream << "not find race id:" << raceSessionId << LogStream::eos;
	//	return;
	//}

	//TeamCopyBossDataEntry* bossPhaseEntry = TeamCopyBossDataEntryMgr::Instance()->FindEntry(bossPhase);
	//if (NULL == bossPhaseEntry)
	//{
	//	DebugLogStream << "bossPhase:" << bossPhase << "not find config!" << LogStream::eos;
	//	return;
	//}

	//auto iter = m_RacePhaseMap.find(raceSessionId);
	//if (iter != m_RacePhaseMap.end())
	//{
	//	auto it = iter->second.find(bossPhase);
	//	if (it != iter->second.end())
	//	{
	//		// 该比赛的阶段已经有其他客户端上报了
	//		DebugLogStream << "boss phase exist raceId:" << raceSessionId << " bossPhase:" << bossPhase << LogStream::eos;
	//		return;
	//	}

	//	iter->second.insert(bossPhase);
	//}
	//else
	//{
	//	std::set<UInt32> tempSet;
	//	tempSet.insert(bossPhase);
	//	m_RacePhaseMap[raceSessionId] = tempSet;
	//}

	//// 阶段出现据点
	//if (bossPhaseEntry->appearField != 0)
	//{
	//	TCSTeamMgr::Instance()->BossPhaseAddField(race->GetTeamSquadId(), bossPhaseEntry->appearField);
	//	InfoLogStream << "raceId:" << raceSessionId << " phase appear field:" << bossPhaseEntry->appearField << LogStream::eos;
	//}

	//// 上一个阶段的据点
	//TeamCopyBossDataEntry* lastPhaseEntry = TeamCopyBossDataEntryMgr::Instance()->FindEntry(bossPhase - 1);
	//if (NULL == lastPhaseEntry)
	//{
	//	return;
	//}

	//// 删掉上一个阶段据点
	//if (lastPhaseEntry->appearField != 0)
	//{
	//	TCSTeamMgr::Instance()->BossPhaseDelField(race->GetTeamSquadId(), lastPhaseEntry->appearField);
	//	InfoLogStream << "raceId:" << raceSessionId << " phase disappear field:" << lastPhaseEntry->appearField << LogStream::eos;
	//}
}

void TCSDungeonMgr::DungeonRaceEnd(UInt64 raceSessionId, UInt32 reason)
{
	CLProtocol::DungeonSvrEndRace end;
	end.raceSessionId = raceSessionId;
	end.resaon = reason;
	SendToDungeonSvr(end);
}

void TCSDungeonMgr::DungeonRaceDestroy(UInt64 raceSessionId, UInt32 dungeonId)
{
	CLProtocol::TeamCopyNotifyRaceDestroyDungeon end;
	end.destroyDungeonId = dungeonId;
	end.notifySessionId = raceSessionId;
	SendToDungeonSvr(end);
}

void TCSDungeonMgr::AddRaceReviveCnt(UInt64 raceSessionId, UInt32 reviveCnt)
{
	CLProtocol::TeamCopyAddRaceReviveCnt notify;
	notify.raceSessionId = raceSessionId;
	notify.reviveCnt = reviveCnt;
	SendToDungeonSvr(notify);
}

void TCSDungeonMgr::ReconnectEndRace(UInt64 roldeId)
{
	CLProtocol::WorldDungeonLeaveRace protocol;
	protocol.roleId = roldeId;
	protocol.isDisconnect = false;
	SendToDungeonSvr(protocol);
}

TCSDungeonRace* TCSDungeonMgr::FindRaceByTeamSquadId(UInt32 teamSquadId)
{
	auto iter = m_TeamSquadRaceMap.find(teamSquadId);
	return iter != m_TeamSquadRaceMap.end() ? iter->second : NULL;
}

TCSDungeonRace* TCSDungeonMgr::FindRaceByRaceId(UInt64 raceId)
{
	auto iter = m_DungeonRaceMap.find(raceId);
	return iter != m_DungeonRaceMap.end() ? iter->second : NULL;
}

