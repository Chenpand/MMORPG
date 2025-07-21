#include "CS2V2ScoreWarMgr.h"
#include "CL2V2ScoreWarProtocol.h"
#include "CL2V2ScoreWarDataEntry.h"
#include "CSRoomMgr.h"
#include <AvalonRetryDBConnection.h>
#include "CSApplication.h"

const UInt32 SCORE_WAR_2V2_RANKING_REWARD_LIMIT = 10;

void CS2V2ScoreWarSortListMgr::Init()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	Avalon::ResultSet* resultSet = dbConn->Query("SELECT * FROM `t_2v2sortlist` WHERE sorttype = " + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_2V2_SCORE_WAR)) + " ORDER BY `update_time`");

	InfoLogStream << " CS2V2ScoreWarSortListMgr::Init()." << LogStream::eos;

	if (resultSet == NULL) return;

	while (resultSet->NextRow())
	{
		ScoreWar2V2SortListEntry entry;
		
		entry.connId = resultSet->GetField("conn_id").GetUInt32();
		entry.playerId = resultSet->GetField("id").GetUInt64();
		entry.playerName = resultSet->GetField("name").GetString();
		entry.score = resultSet->GetField("value1").GetUInt32();
		entry.serverName = resultSet->GetField("server_name").GetString();
		entry.sortType = resultSet->GetField("sorttype").GetUInt8();

		AddToList(entry);
	}

	dbConn->FreeResultSet(resultSet);
}

void CS2V2ScoreWarSortListMgr::UpdateSortList(ScoreWar2V2SortListEntry& entry)
{
	if (!Is2V2ScoreWarSortList(entry.sortType)) return;

	ScoreWarSortEntryList& list = GetSortEntryList((SortListType)entry.sortType);
	ScoreWarSortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		if (iter->playerId == entry.playerId) break;
		++iter;
	}

	if (iter != list.end())
	{
		if (entry.GetSortFactor() == iter->GetSortFactor()) return;
		iter->score = entry.score;

		bool bUpdate = false;
		if (iter != list.begin())
		{
			ScoreWarSortEntryList::iterator preIter = iter;
			--preIter;
			if (iter->GetSortFactor() >= preIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if (!bUpdate)
		{
			ScoreWarSortEntryList::iterator nextIter = iter;
			++nextIter;
			if (nextIter != list.end() && iter->GetSortFactor() < nextIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if (bUpdate)
		{
			ScoreWar2V2SortListEntry sortEntry = *iter;
			//list.erase(iter);
			DeleteEntry(iter);
			InsertEntry(sortEntry);
			AddToList(sortEntry);
		}
	}
	else
	{
		UInt32 listSize = GetSortListSize(entry.sortType);
		if (listSize > 0 && list.size() >= listSize)
		{
			ScoreWarSortEntryList::iterator lastIter = --list.end();
			if (entry.GetSortFactor() <= lastIter->GetSortFactor()) return;
			//list.erase(lastIter);
			DeleteEntry(lastIter);
		}
		InsertEntry(entry);
		AddToList(entry);
	}
}

void CS2V2ScoreWarSortListMgr::ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count)
{
	if (zone == NULL || !Is2V2ScoreWarSortList(sortType)) return;
	ScoreWarSortEntryList& list = GetSortEntryList(sortType);
	if (count > 100) count = 100;
	if (start > list.size()) start = list.size();

	if (size_t(start + count) > list.size())
	{
		count = UInt16(list.size()) - start;
	}

	UInt16 totalNum = 0;
	UInt16 num = 0;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		UInt16 ranking = 0;
		UInt8 tmpType = (UInt8)sortType;
		stream & tmpType & start;
		UInt8* pos1 = packet->GetBuffer() + stream.GetPos();
		stream & totalNum;
		UInt8* pos2 = packet->GetBuffer() + stream.GetPos();
		stream & num;

		ScoreWarSortEntryList::iterator playerIter = list.end();
		UInt16 playerRanking = 0;
		for (ScoreWarSortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			ranking++;
			if (iter->playerId == playerId)
			{
				playerIter = iter;
				playerRanking = ranking;
			}
			++totalNum;
			if (totalNum > start && totalNum <= start + count)
			{
				stream & ranking;
				iter->SerializeForClient(stream);
				++num;
			}
		}

		*((UInt16*)pos1) = totalNum;
		*((UInt16*)pos2) = num;

		//加上自己的排名
		if (playerIter != list.end())
		{
			stream & playerRanking;
			playerIter->SerializeForClient(stream);
		}
		stream & UInt16(0);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	CLProtocol::Cross2V2ScoreWarSortListRes res(packet);
	res.playerId = playerId;
	zone->TransmitToWorld(res);
	res.Detach();

	Avalon::Packet::Destroy(packet);
}

UInt32 CS2V2ScoreWarSortListMgr::GetRanking(SortListType sortType, ObjID_t playerId)
{
	if (playerId == 0 || !Is2V2ScoreWarSortList(sortType)) return 0;
	UInt32 ranking = 0;
	ScoreWarSortEntryList& list = GetSortEntryList(sortType);
	for (ScoreWarSortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		ranking++;
		if (playerId == iter->playerId)
			return ranking;
	}
	return 0;
}

void CS2V2ScoreWarSortListMgr::AddToList(ScoreWar2V2SortListEntry& entry)
{
	if (!Is2V2ScoreWarSortList(entry.sortType)) return;
	ScoreWarSortEntryList& list = GetSortEntryList((SortListType)entry.sortType);
	UInt64 sortFactor = entry.GetSortFactor();

	ScoreWarSortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		if (sortFactor > iter->GetSortFactor()) break;
		++iter;
	}
	list.insert(iter, entry);
}

static CS2V2ScoreWarSortListMgr::ScoreWarSortEntryList Null2V2ScoreWarSortList;
CS2V2ScoreWarSortListMgr::ScoreWarSortEntryList& CS2V2ScoreWarSortListMgr::GetSortEntryList(SortListType sortType)
{
	if (Is2V2ScoreWarSortList(sortType))
	{
		return m_ScoreWarList;
	}
	return Null2V2ScoreWarSortList;
}

void CS2V2ScoreWarSortListMgr::ClearSortList()
{
	m_ScoreWarList.clear();
}

void CS2V2ScoreWarSortListMgr::SendRankingReward()
{
	ScoreWarSortEntryList::iterator iter = m_ScoreWarList.begin();
	UInt32 ranking = 0;
	while (iter != m_ScoreWarList.end())
	{
		if (iter->sortType != SORTLIST_2V2_SCORE_WAR)
		{
			continue;
		}
		ranking++;
		if (ranking > SCORE_WAR_2V2_RANKING_REWARD_LIMIT)
		{
			return;
		}
		CLProtocol::World2V2ScoreWarRankingReward protocol;
		protocol.playerId = (*iter).playerId;
		protocol.ranking = ranking;
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone((*iter).connId);
		if (zone != NULL)
		{
			zone->TransmitToWorld(protocol);
		}
		++iter;
	}
}

void CS2V2ScoreWarSortListMgr::InsertEntry(ScoreWar2V2SortListEntry entry)
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();
	std::string insertSql = "INSERT INTO `t_2v2sortlist` VALUES(" + Avalon::StringUtil::ConvertToString(entry.playerId) + ","
		+ Avalon::StringUtil::ConvertToString(entry.playerId) + ", "
		+ Avalon::StringUtil::ConvertToString(entry.sortType) + ", '"
		+ entry.playerName + "',"
		+ Avalon::StringUtil::ConvertToString(entry.connId) + ", '"
		+ entry.serverName + "',0,0," + Avalon::StringUtil::ConvertToString(entry.score) + ",0,0,"
		+ Avalon::StringUtil::ConvertToString((UInt32)CURRENT_TIME.Sec()) + ")";

	if (dbConn->Update(insertSql) <= 0)
	{
		ErrorLogStream << "CS2V2ScoreWarSortListMgr::InsertEntry(" << insertSql << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
	}
	else
	{
		InfoLogStream << "CS2V2ScoreWarSortListMgr::InsertEntry(" << insertSql << ")" << LogStream::eos;
	}

}

void CS2V2ScoreWarSortListMgr::DeleteEntry(ScoreWarSortEntryList::iterator iter)
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();
	std::string deleteStr = "DELETE FROM t_2v2sortlist WHERE guid=" + Avalon::StringUtil::ConvertToString(iter->playerId) + " AND sorttype =" + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_2V2_SCORE_WAR));

	if (dbConn->Update(deleteStr) < 0)
	{
		ErrorLogStream << "CS2V2ScoreWarSortListMgr::DeleteEntry(" << deleteStr << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
	m_ScoreWarList.erase(iter);
}

bool CS2V2ScoreWarMgr::Init()
{
	m_SortListMgr.Init();

	SetScoreWarTimeInfo(ScoreWar2V2TimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(CURRENT_TIME));

	_InitScoreWarStatus();

	return true;
}

void CS2V2ScoreWarMgr::SetScoreWarTimeInfo(ScoreWar2V2TimeInfo info)
{
	m_ScoreWarTimeInfo = info;
}

void CS2V2ScoreWarMgr::ChangeTargetStatus(ScoreWar2V2Status targetStatus)
{
	InfoLogStream << "current Status = " << static_cast<UInt32>(m_TargetStatus) << "target status = " << static_cast<UInt32>(targetStatus) << LogStream::eos;
	m_TargetStatus = targetStatus;
}

void CS2V2ScoreWarMgr::RemoveWaitResultConnId(UInt32 id)
{
	std::vector<UInt32>::iterator iter = std::find(m_WaitResultConnIds.begin(), m_WaitResultConnIds.end(), id);
	if (iter != m_WaitResultConnIds.end())
	{
		m_WaitResultConnIds.erase(iter);
	}
}

void CS2V2ScoreWarMgr::ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count)
{
	m_SortListMgr.ListSortEntries(zone, playerId, sortType, start, count);
}

void CS2V2ScoreWarMgr::UpdateSortList(ScoreWar2V2SortListEntry& entry)
{
	m_SortListMgr.UpdateSortList(entry);
}

void CS2V2ScoreWarMgr::OnTick(Avalon::Time now)
{
	UInt32 nowSec = (UInt32)now.Sec();

	if (nowSec - m_CheckWaitTime >= 5)
	{
		for (UInt32 i = 0; i < m_WaitResultConnIds.size(); ++i)
		{
			SyncScoreWarStatus(m_WaitResultConnIds[i]);
		}
	}

	if (m_IsByTime)
	{
		if (nowSec > m_ScoreWarTimeInfo.endTime || nowSec < m_ScoreWarTimeInfo.preTime)
		{
			if (m_Status != ScoreWar2V2Status::SWS_2V2_INVALID)
			{
				ChangeTargetStatus(ScoreWar2V2Status::SWS_2V2_INVALID);
				SetScoreWarTimeInfo(ScoreWar2V2TimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(now));
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.waitEndTime)
		{
			if (m_Status != ScoreWar2V2Status::SWS_2V2_WAIT_END)
			{
				ChangeTargetStatus(ScoreWar2V2Status::SWS_2V2_WAIT_END);
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.battleTime)
		{
			if (m_Status != ScoreWar2V2Status::SWS_2V2_BATTLE)
			{
				ChangeTargetStatus(ScoreWar2V2Status::SWS_2V2_BATTLE);
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.preTime)
		{
			if (m_Status != ScoreWar2V2Status::SWS_2V2_PREPARE)
			{
				ChangeTargetStatus(ScoreWar2V2Status::SWS_2V2_PREPARE);
			}
		}
	}

	_ChangeToTargetStatus();
}

void CS2V2ScoreWarMgr::OnConnected(UInt32 id)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(id);
	if (zone != NULL)
	{
		SyncScoreWarStatus(id);
		m_WaitResultConnIds.push_back(id);
		m_CheckWaitTime = (UInt32)CURRENT_TIME.Sec();
	}
}

void CS2V2ScoreWarMgr::OnDisconnected(UInt32 id)
{
	RemoveWaitResultConnId(id);
}

void CS2V2ScoreWarMgr::OnScoreWarPrepare()
{
	InfoLogStream << " CS2V2ScoreWarMgr::OnScoreWarPrepare()." << LogStream::eos;

	_ClearScoreWarSortList();
	_SetScoreWarStatus(ScoreWar2V2Status::SWS_2V2_PREPARE);
}

void CS2V2ScoreWarMgr::OnScoreWarBattle()
{
	InfoLogStream << " CS2V2ScoreWarMgr::OnScoreWarBattle()." << LogStream::eos;
	_SetScoreWarStatus(ScoreWar2V2Status::SWS_2V2_BATTLE);
}

void CS2V2ScoreWarMgr::OnScoreWarWaitEnd()
{
	InfoLogStream << " CS2V2ScoreWarMgr::OnScoreWarWaitEnd()." << LogStream::eos;
	//CSRoomMgr::Instance()->StopMatch(RoomType::ROOM_TYPE_THREE_SCORE_WAR);
	_SetScoreWarStatus(ScoreWar2V2Status::SWS_2V2_WAIT_END);
}

void CS2V2ScoreWarMgr::OnScoreWarEnd()
{
	InfoLogStream << " CS2V2ScoreWarMgr::OnScoreWarEnd()." << LogStream::eos;
	m_SortListMgr.SendRankingReward();
	_SetScoreWarStatus(ScoreWar2V2Status::SWS_2V2_INVALID);
}


void CS2V2ScoreWarMgr::SyncScoreWarStatus(UInt32 zoneId)
{
	UInt32 statusEndTime = 0;
	switch (m_Status)
	{
	case ScoreWar2V2Status::SWS_2V2_INVALID:
		statusEndTime = m_ScoreWarTimeInfo.preTime;
		break;
	case ScoreWar2V2Status::SWS_2V2_PREPARE:
		statusEndTime = m_ScoreWarTimeInfo.battleTime;
		break;
	case ScoreWar2V2Status::SWS_2V2_BATTLE:
		statusEndTime = m_ScoreWarTimeInfo.waitEndTime;
		break;
	case ScoreWar2V2Status::SWS_2V2_WAIT_END:
		statusEndTime = m_ScoreWarTimeInfo.endTime;
		break;
	default:
		break;
	}

	CLProtocol::Scene2V2CrossSyncScoreWarInfo sync;
	sync.status = m_Status;
	sync.statusEndTime = statusEndTime;
	sync.startTime = m_ScoreWarTimeInfo.preTime;

	if (zoneId == 0)
	{
		GameZoneMgr::Instance()->TransmitToScene(sync);
	}
	else
	{
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(zoneId);
		if (zone != NULL)
		{
			zone->TransmitToScene(sync);
		}
	}
}

void CS2V2ScoreWarMgr::_SetScoreWarStatus(ScoreWar2V2Status status)
{
	if (m_Status != status)
	{
		m_Status = status;
		_SaveScoreWarStatus();
		SyncScoreWarStatus();
	}
}

void CS2V2ScoreWarMgr::_InitScoreWarStatus()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	Avalon::ResultSet* resultSet = dbConn->Query("SELECT * FROM `t_gameparam` WHERE name='score_war_2v2_status'");

	if (resultSet == NULL) return;

	while (resultSet->NextRow())
	{
		m_Status = (ScoreWar2V2Status)resultSet->GetField("value").GetUInt32();
	}

	dbConn->FreeResultSet(resultSet);

	UInt32 nowSec = (UInt32)CURRENT_TIME.Sec();
	if (nowSec > m_ScoreWarTimeInfo.endTime || nowSec < m_ScoreWarTimeInfo.preTime)
	{
		m_TargetStatus = ScoreWar2V2Status::SWS_2V2_INVALID;
		SetScoreWarTimeInfo(ScoreWar2V2TimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(CURRENT_TIME));
	}
	else if (nowSec >= m_ScoreWarTimeInfo.waitEndTime)
	{
		//m_Status = ScoreWar2V2Status::SWS_2V2_WAIT_END;
		m_TargetStatus = ScoreWar2V2Status::SWS_2V2_WAIT_END;
	}
	else if (nowSec >= m_ScoreWarTimeInfo.battleTime)
	{
		//m_Status = ScoreWar2V2Status::SWS_2V2_BATTLE;
		m_TargetStatus = ScoreWar2V2Status::SWS_2V2_BATTLE;
	}
	else if (nowSec >= m_ScoreWarTimeInfo.preTime)
	{
		//m_Status = ScoreWar2V2Status::SWS_2V2_PREPARE;
		m_TargetStatus = ScoreWar2V2Status::SWS_2V2_PREPARE;
	}

	InfoLogStream << "init status = " << static_cast<UInt32>(m_TargetStatus) << LogStream::eos;
}

void CS2V2ScoreWarMgr::_ChangeToTargetStatus()
{
	if (m_Status == m_TargetStatus || m_TargetStatus >= ScoreWar2V2Status::SWS_2V2_MAX) return;
	ScoreWar2V2Status tmpStatus = (ScoreWar2V2Status)(m_Status + 1);
	if (tmpStatus >= ScoreWar2V2Status::SWS_2V2_MAX)
	{
		tmpStatus = ScoreWar2V2Status::SWS_2V2_INVALID;
	}

	switch (tmpStatus)
	{
	case ScoreWar2V2Status::SWS_2V2_PREPARE:
		OnScoreWarPrepare();
		break;
	case ScoreWar2V2Status::SWS_2V2_BATTLE:
		OnScoreWarBattle();
		break;
	case ScoreWar2V2Status::SWS_2V2_WAIT_END:
		OnScoreWarWaitEnd();
		break;
	case ScoreWar2V2Status::SWS_2V2_INVALID:
		OnScoreWarEnd();
		break;
	default:
		m_Status = tmpStatus;
		_SaveScoreWarStatus();
		break;
	}
	_ChangeToTargetStatus();
}

void CS2V2ScoreWarMgr::_SaveScoreWarStatus()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	std::string updateStr = "REPLACE INTO `t_gameparam` VALUES('score_war_2v2_status'," + Avalon::StringUtil::ConvertToString((UInt32)m_Status) + ")";

	InfoLogStream << updateStr << LogStream::eos;

	if (dbConn->Update(updateStr) <= 0)
	{
		ErrorLogStream << "CS2V2ScoreWarMgr::_SaveScoreWarStatus(" << m_Status << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void CS2V2ScoreWarMgr::_ClearScoreWarSortList()
{
	m_SortListMgr.ClearSortList();

	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	std::string deleteStr = "DELETE FROM t_2v2sortlist WHERE sorttype = " + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_2V2_SCORE_WAR));

	InfoLogStream << deleteStr << LogStream::eos;

	if (dbConn->Update(deleteStr) < 0)
	{
		ErrorLogStream << "CS2V2ScoreWarMgr::_ClearScoreWarSortList(" << m_Status << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

