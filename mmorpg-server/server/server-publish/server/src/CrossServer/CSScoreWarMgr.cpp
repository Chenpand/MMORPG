#include "CSScoreWarMgr.h"
#include "CLScoreWarProtocol.h"
#include "CLScoreWarDataEntry.h"
#include "CSRoomMgr.h"
#include <AvalonRetryDBConnection.h>
#include "CSApplication.h"

void CSScoreWarSortListMgr::Init()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	Avalon::ResultSet* resultSet = dbConn->Query("SELECT * FROM `t_sortlist` WHERE sorttype = " + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_SCORE_WAR)) + " ORDER BY `update_time`");
	InfoLogStream << " CSScoreWarSortListMgr::Init()." << LogStream::eos;

	if (resultSet == NULL) return;

	while (resultSet->NextRow())
	{
		ScoreWarSortListEntry entry;
		
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

void CSScoreWarSortListMgr::UpdateSortList(ScoreWarSortListEntry& entry)
{
	if (!IsScoreWarSortList(entry.sortType)) return;

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
			ScoreWarSortListEntry sortEntry = *iter;
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

void CSScoreWarSortListMgr::ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count)
{
	if (zone == NULL || !IsScoreWarSortList(sortType)) return;
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

	CLProtocol::CrossScoreWarSortListRes res(packet);
	res.playerId = playerId;
	zone->TransmitToWorld(res);
	res.Detach();

	Avalon::Packet::Destroy(packet);
}

UInt32 CSScoreWarSortListMgr::GetRanking(SortListType sortType, ObjID_t playerId)
{
	if (playerId == 0 || !IsScoreWarSortList(sortType)) return 0;
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

void CSScoreWarSortListMgr::AddToList(ScoreWarSortListEntry& entry)
{
	if (!IsScoreWarSortList(entry.sortType)) return;
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

static CSScoreWarSortListMgr::ScoreWarSortEntryList NullScoreWarSortList;
CSScoreWarSortListMgr::ScoreWarSortEntryList& CSScoreWarSortListMgr::GetSortEntryList(SortListType sortType)
{
	if (IsScoreWarSortList(sortType))
	{
		return m_ScoreWarList;
	}
	return NullScoreWarSortList;
}

void CSScoreWarSortListMgr::ClearSortList()
{
	m_ScoreWarList.clear();
}

void CSScoreWarSortListMgr::SendRankingReward()
{
	ScoreWarSortEntryList::iterator iter = m_ScoreWarList.begin();
	UInt32 ranking = 0;
	while (iter != m_ScoreWarList.end())
	{
		if (iter->sortType != SORTLIST_SCORE_WAR)
		{
			continue;
		}
		ranking++;
		CLProtocol::WorldScoreWarRankingReward protocol;
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

void CSScoreWarSortListMgr::InsertEntry(ScoreWarSortListEntry entry)
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();
	std::string insertSql = "INSERT INTO `t_sortlist` VALUES(" + Avalon::StringUtil::ConvertToString(entry.playerId) + ","
		+ Avalon::StringUtil::ConvertToString(entry.playerId) + ", "
		+ Avalon::StringUtil::ConvertToString(entry.sortType) + ", '"
		+ entry.playerName + "',"
		+ Avalon::StringUtil::ConvertToString(entry.connId) + ", '"
		+ entry.serverName + "',0,0," + Avalon::StringUtil::ConvertToString(entry.score) + ",0,0,"
		+ Avalon::StringUtil::ConvertToString((UInt32)CURRENT_TIME.Sec()) + ")";

	if (dbConn->Update(insertSql) <= 0)
	{
		ErrorLogStream << "CSScoreWarSortListMgr::InsertEntry(" << insertSql << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
	}

}

void CSScoreWarSortListMgr::DeleteEntry(ScoreWarSortEntryList::iterator iter)
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();
	std::string deleteStr = "DELETE FROM t_sortlist WHERE guid=" + Avalon::StringUtil::ConvertToString(iter->playerId) + " AND sorttype ="  + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_SCORE_WAR));

	if (dbConn->Update(deleteStr) < 0)
	{
		ErrorLogStream << "CSScoreWarSortListMgr::DeleteEntry(" << deleteStr << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
	m_ScoreWarList.erase(iter);
}

bool CSScoreWarMgr::Init()
{
	m_SortListMgr.Init();

	SetScoreWarTimeInfo(ScoreWarTimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(CURRENT_TIME));

	_InitScoreWarStatus();

	return true;
}

void CSScoreWarMgr::SetScoreWarTimeInfo(ScoreWarTimeInfo info)
{
	m_ScoreWarTimeInfo = info;
}

void CSScoreWarMgr::ChangeTargetStatus(ScoreWarStatus targetStatus)
{
	m_TargetStatus = targetStatus;
}

void CSScoreWarMgr::RemoveWaitResultConnId(UInt32 id)
{
	std::vector<UInt32>::iterator iter = std::find(m_WaitResultConnIds.begin(), m_WaitResultConnIds.end(), id);
	if (iter != m_WaitResultConnIds.end())
	{
		m_WaitResultConnIds.erase(iter);
	}
}

void CSScoreWarMgr::ListSortEntries(GameZone* zone, ObjID_t playerId, SortListType sortType, UInt16 start, UInt16 count)
{
	m_SortListMgr.ListSortEntries(zone, playerId, sortType, start, count);
}

void CSScoreWarMgr::UpdateSortList(ScoreWarSortListEntry& entry)
{
	m_SortListMgr.UpdateSortList(entry);
}

void CSScoreWarMgr::OnTick(Avalon::Time now)
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
			if (m_Status != ScoreWarStatus::SWS_INVALID)
			{
				ChangeTargetStatus(ScoreWarStatus::SWS_INVALID);
				SetScoreWarTimeInfo(ScoreWarTimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(now));
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.waitEndTime)
		{
			if (m_Status != ScoreWarStatus::SWS_WAIT_END)
			{
				ChangeTargetStatus(ScoreWarStatus::SWS_WAIT_END);
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.battleTime)
		{
			if (m_Status != ScoreWarStatus::SWS_BATTLE)
			{
				ChangeTargetStatus(ScoreWarStatus::SWS_BATTLE);
			}
		}
		else if (nowSec >= m_ScoreWarTimeInfo.preTime)
		{
			if (m_Status != ScoreWarStatus::SWS_PREPARE)
			{
				ChangeTargetStatus(ScoreWarStatus::SWS_PREPARE);
			}
		}
	}

	_ChangeToTargetStatus();
}

void CSScoreWarMgr::OnConnected(UInt32 id)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(id);
	if (zone != NULL)
	{
		SyncScoreWarStatus(id);
		m_WaitResultConnIds.push_back(id);
		m_CheckWaitTime = (UInt32)CURRENT_TIME.Sec();
	}
}

void CSScoreWarMgr::OnDisconnected(UInt32 id)
{
	RemoveWaitResultConnId(id);
}

void CSScoreWarMgr::OnScoreWarPrepare()
{
	InfoLogStream << " CSScoreWarMgr::OnScoreWarPrepare()." << LogStream::eos;

	_ClearScoreWarSortList();
	_SetScoreWarStatus(ScoreWarStatus::SWS_PREPARE);
}

void CSScoreWarMgr::OnScoreWarBattle()
{
	InfoLogStream << " CSScoreWarMgr::OnScoreWarBattle()." << LogStream::eos;
	_SetScoreWarStatus(ScoreWarStatus::SWS_BATTLE);
}

void CSScoreWarMgr::OnScoreWarWaitEnd()
{
	InfoLogStream << " CSScoreWarMgr::OnScoreWarWaitEnd()." << LogStream::eos;
	//CSRoomMgr::Instance()->StopMatch(RoomType::ROOM_TYPE_THREE_SCORE_WAR);
	_SetScoreWarStatus(ScoreWarStatus::SWS_WAIT_END);
}

void CSScoreWarMgr::OnScoreWarEnd()
{
	InfoLogStream << " CSScoreWarMgr::OnScoreWarEnd()." << LogStream::eos;
	m_SortListMgr.SendRankingReward();
	_SetScoreWarStatus(ScoreWarStatus::SWS_INVALID);
}


void CSScoreWarMgr::SyncScoreWarStatus(UInt32 zoneId)
{
	UInt32 statusEndTime = 0;
	switch (m_Status)
	{
	case ScoreWarStatus::SWS_INVALID:
		statusEndTime = m_ScoreWarTimeInfo.preTime;
		break;
	case ScoreWarStatus::SWS_PREPARE:
		statusEndTime = m_ScoreWarTimeInfo.battleTime;
		break;
	case ScoreWarStatus::SWS_BATTLE:
		statusEndTime = m_ScoreWarTimeInfo.waitEndTime;
		break;
	case ScoreWarStatus::SWS_WAIT_END:
		statusEndTime = m_ScoreWarTimeInfo.endTime;
		break;
	default:
		break;
	}

	CLProtocol::SceneCrossSyncScoreWarInfo sync;
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

void CSScoreWarMgr::_SetScoreWarStatus(ScoreWarStatus status)
{
	if (m_Status != status)
	{
		m_Status = status;
		_SaveScoreWarStatus();
		SyncScoreWarStatus();
	}
}

void CSScoreWarMgr::_InitScoreWarStatus()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	Avalon::ResultSet* resultSet = dbConn->Query("SELECT * FROM `t_gameparam` WHERE name='score_war_status'");

	if (resultSet == NULL) return;

	while (resultSet->NextRow())
	{
		m_Status = (ScoreWarStatus)resultSet->GetField("value").GetUInt32();
	}

	dbConn->FreeResultSet(resultSet);

	UInt32 nowSec = (UInt32)CURRENT_TIME.Sec();
	if (nowSec > m_ScoreWarTimeInfo.endTime || nowSec < m_ScoreWarTimeInfo.preTime)
	{
		m_TargetStatus = ScoreWarStatus::SWS_INVALID;
		SetScoreWarTimeInfo(ScoreWarTimeDataEntryMgr::Instance()->GetScoreWarTimeInfo(CURRENT_TIME));
	}
	else if (nowSec >= m_ScoreWarTimeInfo.waitEndTime)
	{
		//m_Status = ScoreWarStatus::SWS_WAIT_END;
		m_TargetStatus = ScoreWarStatus::SWS_WAIT_END;
	}
	else if (nowSec >= m_ScoreWarTimeInfo.battleTime)
	{
		//m_Status = ScoreWarStatus::SWS_BATTLE;
		m_TargetStatus = ScoreWarStatus::SWS_BATTLE;
	}
	else if (nowSec >= m_ScoreWarTimeInfo.preTime)
	{
		//m_Status = ScoreWarStatus::SWS_PREPARE;
		m_TargetStatus = ScoreWarStatus::SWS_PREPARE;
	}
}

void CSScoreWarMgr::_ChangeToTargetStatus()
{
	if (m_Status == m_TargetStatus || m_TargetStatus >= ScoreWarStatus::SWS_MAX) return;
	ScoreWarStatus tmpStatus = (ScoreWarStatus)(m_Status + 1);
	if (tmpStatus >= ScoreWarStatus::SWS_MAX)
	{
		tmpStatus = ScoreWarStatus::SWS_INVALID;
	}

	switch (tmpStatus)
	{
	case ScoreWarStatus::SWS_PREPARE:
		OnScoreWarPrepare();
		break;
	case ScoreWarStatus::SWS_BATTLE:
		OnScoreWarBattle();
		break;
	case ScoreWarStatus::SWS_WAIT_END:
		OnScoreWarWaitEnd();
		break;
	case ScoreWarStatus::SWS_INVALID:
		OnScoreWarEnd();
		break;
	default:
		m_Status = tmpStatus;
		_SaveScoreWarStatus();
		break;
	}
	_ChangeToTargetStatus();
}

void CSScoreWarMgr::_SaveScoreWarStatus()
{
	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	std::string updateStr = "REPLACE INTO `t_gameparam` VALUES('score_war_status'," + Avalon::StringUtil::ConvertToString((UInt32)m_Status) + ")";

	if (dbConn->Update(updateStr) <= 0)
	{
		ErrorLogStream << "CSScoreWarMgr::_SaveScoreWarStatus(" << m_Status << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void CSScoreWarMgr::_ClearScoreWarSortList()
{
	m_SortListMgr.ClearSortList();

	Avalon::RetryDBConnection* dbConn = CSApplication::Instance()->GetDBConnection();

	std::string deleteStr = "DELETE FROM t_sortlist WHERE sorttype = " + Avalon::StringUtil::ConvertToString(static_cast<UInt32>(SORTLIST_SCORE_WAR));

	if (dbConn->Update(deleteStr) < 0)
	{
		ErrorLogStream << "CSScoreWarMgr::_ClearScoreWarSortList(" << m_Status << ")  ERROR:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

