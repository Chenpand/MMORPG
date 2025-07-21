#include "BSSortListMgr.h"
#include "BSApplication.h"
#include "GameZone.h"
#include "BSBattleMgr.h"

#include <CLSortListDefine.h>
#include <CLSortListProtocol.h>
#include <AvalonPacket.h>
#include <CLBattleProtocol.h>

BSSortListMgr::BSSortListMgr()
{

}

BSSortListMgr::~BSSortListMgr()
{

}

bool BSSortListMgr::Init()
{
	{
		Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
		Avalon::ResultSet* result = dbConn->QueryFmt("select * from t_sortlist where sorttype=%d", SORTLIST_CHIJI_SCORE);
		if (NULL == result) return false;

		InfoLogStream << " BSSortListMgr Init Success" << LogStream::eos;

		while (result->NextRow())
		{
			ChijiSortListEntry entry;
			entry.connId = result->GetField("conn_id").GetUInt32();
			entry.playerId = result->GetField("id").GetUInt64();
			entry.playerName = result->GetField("name").GetString();
			entry.score = result->GetField("value1").GetUInt32();
			entry.serverName = result->GetField("server_name").GetString();
			entry.sortType = result->GetField("sorttype").GetUInt8();
			entry.playerOccu = result->GetField("occu").GetUInt32();

			_AddToList(entry);
		}

		dbConn->FreeResultSet(result);
	}

	{
		Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
		Avalon::ResultSet* result = dbConn->QueryFmt("select * from t_player_best_rank");
		if (NULL == result) return true;

		InfoLogStream << " BSSortListMgr Get Player Best Rank Success" << LogStream::eos;

		while (result->NextRow())
		{
			ChiJiPlayerBestRank entry;
			entry.playerId = result->GetField("guid").GetUInt64();
			entry.connId = result->GetField("conn_id").GetUInt32();
			entry.rank = result->GetField("rank").GetUInt32();

			m_PlayerBestRankMap[entry.playerId] = entry;
		}

		dbConn->FreeResultSet(result);
	}

	return true;
}

void BSSortListMgr::HandleSortListReq(GameZone* zone, ObjID_t playerId, UInt32 sortType, UInt16 start, UInt16 count)
{
	if (NULL == zone)
	{
		ErrorLogStream << "GameZone Null! playerId:"<< playerId << LogStream::eos;
		return;
	}
	
	if (count > 100) count = 100;
	if (start > m_SortEntryList.size()) start = m_SortEntryList.size();

	if (size_t(start + count) > m_SortEntryList.size())
	{
		count = UInt16(m_SortEntryList.size()) - start;
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

		for (SortEntryList::iterator iter = m_SortEntryList.begin(); iter != m_SortEntryList.end(); ++iter)
		{
			ranking++;
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
		stream & UInt16(0);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	packet->SetSize(stream.GetPos());
	zone->SendToPlayer(playerId, packet);

	Avalon::Packet::Destroy(packet);
}

void BSSortListMgr::UpdateSortList(ChijiSortListEntry& entry)
{
	if (!IsChijiSortList(entry.sortType)) return;

	SortEntryList::iterator iter = m_SortEntryList.begin();
	while (iter != m_SortEntryList.end())
	{
		if (iter->playerId == entry.playerId) break;
		++iter;
	}

	if (iter != m_SortEntryList.end())
	{
		iter->score += entry.score;

		bool bUpdate = false;
		if (iter != m_SortEntryList.begin())
		{
			SortEntryList::iterator preIter = iter;
			--preIter;
			if (iter->GetSortFactor() > preIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if (!bUpdate)
		{
			SortEntryList::iterator nextIter = iter;
			++nextIter;
			if (nextIter != m_SortEntryList.end() && iter->GetSortFactor() < nextIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}

		_UpdateData(*iter);

		if (bUpdate)
		{
			entry.score = iter->score;
			m_SortEntryList.erase(iter);
			_AddToList(entry);
		}
	}
	else
	{
		entry.score += GetPlayerScore(entry.playerId);
		if (entry.score == 0)
		{
			return;
		}

		if (m_SortEntryList.size() >= GetSortListSize(entry.sortType))
		{
			SortEntryList::iterator lastIter = --m_SortEntryList.end();
			if (entry.GetSortFactor() <= lastIter->GetSortFactor()) 
				return;

			_deleteData(lastIter->playerId);
			m_SortEntryList.erase(lastIter);
		}

		_InsertData(entry);
		_AddToList(entry);
	}
}

void BSSortListMgr::ClearSortList()
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn Error!" << LogStream::eos;
		return;
	}

	std::string delSql;
	delSql.reserve(128);
	delSql.append("delete from t_sortlist where sorttype=").append(std::to_string(SORTLIST_CHIJI_SCORE));

	if (dbConn->Update(delSql) < 0)
	{
		ErrorLogStream << "clearSortList " << delSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}

	// 记录下吃鸡排行奖励
	UInt32 rank = 0;
	for (auto& iter : m_SortEntryList)
	{
		++rank;
		InfoLogStream << "chi ji score rank:"<< rank << " playerId:"<< iter.playerId << " playerName:" << iter.playerName 
			<< " connId:" << iter.connId << " score:" << iter.score << LogStream::eos;
	}

	m_SortEntryList.clear();
	m_PlayerScoreMap.clear();
}

ChijiSortListEntry* BSSortListMgr::FindEntry(ObjID_t id)
{
	for (auto& tt : m_SortEntryList)
	{
		if (tt.playerId == id)
			return &tt;
	}

	return NULL;
}

void BSSortListMgr::_AddToList(ChijiSortListEntry& entry)
{
	UInt64 sortFactor = entry.GetSortFactor();

	SortEntryList::iterator iter = m_SortEntryList.begin();
	while (iter != m_SortEntryList.end())
	{
		if (sortFactor > iter->GetSortFactor()) break;
		++iter;
	}

	m_SortEntryList.insert(iter, entry);
}

void BSSortListMgr::_InsertData(ChijiSortListEntry& entry)
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn null!" << LogStream::eos;
		return;
	}

	std::string insertSql;
	insertSql.reserve(255);
	insertSql.append("insert into t_sortlist values(")
		.append(std::to_string(entry.playerId)).append(",")
		.append(std::to_string(entry.playerId)).append(",")
		.append(std::to_string(entry.sortType)).append(",'")
		.append(entry.playerName).append("',")
		.append(std::to_string(entry.connId)).append(",'")
		.append(entry.serverName).append("',0,")
		.append(std::to_string(entry.playerOccu)).append(",")
		.append(std::to_string(entry.score)).append(",0,0,")
		.append(std::to_string((UInt32)CURRENT_TIME.Sec())).append(")");

	if (dbConn->Update(insertSql) <= 0)
	{
		ErrorLogStream << "_InsertData " << insertSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void BSSortListMgr::_UpdateData(ChijiSortListEntry& entry)
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn null!" << LogStream::eos;
		return;
	}

	std::string updateSql;
	updateSql.reserve(255);
	updateSql.append("update t_sortlist set")
		.append(" name='").append(entry.playerName)
		.append("',conn_id=").append(std::to_string(entry.connId))
		.append(",server_name='").append(entry.serverName)
		.append("',value1=").append(std::to_string(entry.score))
		.append(",update_time=").append(std::to_string((UInt32)CURRENT_TIME.Sec()))
		.append(" where guid=").append(std::to_string(entry.playerId))
		.append(";");

	if (dbConn->Update(updateSql) <= 0)
	{
		ErrorLogStream << "_UpdateData " << updateSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void BSSortListMgr::_deleteData(UInt64 playerId)
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn null!" << LogStream::eos;
		return;
	}

	std::string delSql;
	delSql.reserve(128);
	delSql.append("delete from t_sortlist where guid=")
		.append(std::to_string(playerId));

	if (dbConn->Update(delSql) < 0)
	{
		ErrorLogStream << "_deleteData " << delSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void BSSortListMgr::SetPlayerScore(UInt64 playerID, UInt32 score)
{
	if (score == 0)
		return;

	m_PlayerScoreMap[playerID] = score;
}

UInt32 BSSortListMgr::GetPlayerScore(UInt64 playerID)
{
	auto iter = m_PlayerScoreMap.find(playerID);
	if (iter == m_PlayerScoreMap.end())
	{
		return 0;
	}

	return iter->second;
}

void BSSortListMgr::SetPlayerBestRank(UInt64 playerID, UInt32 connId, UInt32 rank)
{
	bool needNotify = false;

	do 
	{
		auto iter = m_PlayerBestRankMap.find(playerID);
		if (iter == m_PlayerBestRankMap.end())
		{
			ChiJiPlayerBestRank tmpInfo;
			tmpInfo.playerId = playerID;
			tmpInfo.connId = connId;
			tmpInfo.rank = rank;

			m_PlayerBestRankMap[playerID] = tmpInfo;

			char insertSql[128] = {0};
			avalon_snprintf(insertSql, (sizeof(insertSql) - 1), "insert into t_player_best_rank (guid,conn_id,rank) values (%llu,%u,%u)", playerID, connId, rank);
			Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
			if (dbConn)
			{
				DebugLogStream << "SQL:" << insertSql << LogStream::eos;
				dbConn->Update(insertSql, strlen(insertSql));
			}

			needNotify = true;
			break;
		}

		if (rank < iter->second.rank)
		{
			iter->second.rank = rank;
			iter->second.connId = connId;

			char updateSql[128] = { 0 };
			avalon_snprintf(updateSql, (sizeof(updateSql) - 1), "update t_player_best_rank set conn_id=%u,rank=%u where guid=%llu", connId, rank, playerID);
			Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
			if (dbConn)
			{
				DebugLogStream << "SQL:" << updateSql << LogStream::eos;
				dbConn->Update(updateSql, strlen(updateSql));
			}

			needNotify = true;
			break;
		}

	} while (0);

	if (needNotify)
	{
		NotifyPlayerBestRank(playerID);
	}
}

void BSSortListMgr::ClearPlayerBestRank()
{
	// 发奖
	for (auto& data : m_PlayerBestRankMap)
	{
		CLProtocol::BattleNotifyRankAward nofityMsg;
		nofityMsg.playerId = data.second.playerId;
		nofityMsg.rank = data.second.rank;

		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(data.second.connId);
		if (zone != NULL)
		{
			zone->TransmitToScene(nofityMsg);
			DebugLogStream << "send msg to self scene:" << nofityMsg.playerId << ", msg id:" << nofityMsg.GetProtocolID() << ", rank:" << nofityMsg.rank << LogStream::eos;
		}
		else
		{
			DebugLogStream << "Not Find Zone Player:" << nofityMsg.playerId << ", msg id:" << nofityMsg.GetProtocolID() << ", rank:" << nofityMsg.rank << ", ConnId:" << data.second.connId << LogStream::eos;
		}
	}

	// 清理内存数据
	m_PlayerBestRankMap.clear();

	// 删除数据库中的数据
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn Error!" << LogStream::eos;
		return;
	}

	std::string delSql;
	delSql.reserve(128);
	delSql.append("delete from t_player_best_rank");

	if (dbConn->Update(delSql) < 0)
	{
		ErrorLogStream << "clearSortList " << delSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void BSSortListMgr::NotifyPlayerBestRank(UInt64 playerID)
{
	auto iter = m_PlayerBestRankMap.find(playerID);
	if (iter != m_PlayerBestRankMap.end())
	{
		CLProtocol::BattleNotifyBestRank nofityMsg;
		nofityMsg.rank = iter->second.rank;

		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iter->second.connId);
		if (zone != NULL)
		{
			zone->SendToPlayer(iter->second.playerId, nofityMsg);
		}
	}
}
