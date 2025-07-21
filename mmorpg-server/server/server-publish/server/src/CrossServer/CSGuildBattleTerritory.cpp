#include "CSGuildBattleTerritory.h"
#include "CLErrorCode.h"
#include "GameZoneMgr.h"
#include "CSGuildBattleMgr.h"

#include <CLGuildProtocol.h>
#include <CLGuildBattleDataEntry.h>


void CSTerritorySortListMgr::UpdateSortList(TerritorySortListEntry& entry)
{
	if (!IsGuildBattleSortList(entry.sortType)) return;
	//if (!CSGuildBattleMgr::Instance()->IsGuildBattle()) return;

	TerritorySortEntryList& list = GetSortEntryList((SortListType)entry.sortType, entry.guildId);
	TerritorySortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		if (IsGuildSortList(entry.sortType))
		{
			if (iter->guildId == entry.guildId) break;
		}
		else
		{
			if (iter->playerId == entry.playerId) break;
		}
		++iter;
	}

	if (iter != list.end())
	{
		if (entry.GetSortFactor() == iter->GetSortFactor()) return;
		iter->score = entry.score;

		bool bUpdate = false;
		if (iter != list.begin())
		{
			TerritorySortEntryList::iterator preIter = iter;
			--preIter;
			if (iter->GetSortFactor() >= preIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if (!bUpdate)
		{
			TerritorySortEntryList::iterator nextIter = iter;
			++nextIter;
			if (nextIter != list.end() && iter->GetSortFactor() < nextIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if (bUpdate)
		{
			TerritorySortListEntry sortEntry = *iter;
			list.erase(iter);
			AddToList(sortEntry);
		}
	}
	else
	{
		UInt32 listSize = GetSortListSize(entry.sortType);
		if (listSize > 0 && list.size() >= listSize)
		{
			TerritorySortEntryList::iterator lastIter = --list.end();
			if (entry.GetSortFactor() <= lastIter->GetSortFactor()) return;
			list.erase(lastIter);
		}
		AddToList(entry);
	}
}

void CSTerritorySortListMgr::ListSortEntries(GameZone* zone, ObjID_t playerId , ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count)
{
	if ( zone == NULL || !IsGuildBattleSortList(sortType)) return;
	TerritorySortEntryList& list = GetSortEntryList(sortType, guildId);
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

		TerritorySortEntryList::iterator playerIter = list.end();
		UInt16 playerRanking = 0;

		for (TerritorySortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			ranking++;

			if ((IsGuildSortList(iter->sortType) && iter->guildId == guildId) ||
				(!IsGuildSortList(iter->sortType) && iter->playerId == playerId))
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

	CLProtocol::CrossGuildBattleSortListRes res(packet);
	res.playerId = playerId;
	zone->TransmitToWorld(res);
	res.Detach();

	Avalon::Packet::Destroy(packet);
}

UInt32 CSTerritorySortListMgr::GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId)
{
	if (playerId == 0 || !IsGuildBattleSortList(sortType)) return 0;
	UInt32 ranking = 0;
	TerritorySortEntryList& list = GetSortEntryList(sortType, guildId);
	for (TerritorySortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		ranking++;
		if (IsGuildSortList(sortType))
		{
			if (iter->guildId == guildId)
				return ranking;
		}
		else
		{
			if (iter->playerId == playerId)
				return ranking;
		}
	}
	return 0;
}

UInt32 CSTerritorySortListMgr::GetSortFirstGuild(GuildBattleEndInfo& info)
{
	TerritorySortEntryList& list = GetSortEntryList(SORTLIST_GUILD_BATTLE_SCORE, 0);
	if (list.size() <= 0) return ErrorCode::CROSS_GUILD_BATTLE_SORT_NOT_GUILD;

	TerritorySortEntryList::iterator iter = list.begin();
	info.guildName = iter->guildName;
	info.guildLeaderName = iter->playerName;
	info.guildId = iter->guildId;
	info.serverName = iter->serverName;
	return ErrorCode::SUCCESS;
}

void CSTerritorySortListMgr::AddToList(TerritorySortListEntry& entry)
{
	if (!IsGuildBattleSortList(entry.sortType)) return;

	TerritorySortEntryList& list = GetSortEntryList((SortListType)entry.sortType, entry.guildId);
	UInt64 sortFactor = entry.GetSortFactor();

	TerritorySortEntryList::iterator iter = list.begin();
	while (iter != list.end())
	{
		if (sortFactor > iter->GetSortFactor()) break;
		++iter;
	}

	list.insert(iter, entry);
}


static CSTerritorySortListMgr::TerritorySortEntryList NullTerritorySortList;
CSTerritorySortListMgr::TerritorySortEntryList& CSTerritorySortListMgr::GetSortEntryList(SortListType sortType, UInt64 guildId)
{
	if (IsGuildBattleSortList(sortType))
	{
		switch (sortType)
		{
		case SORTLIST_GUILD_BATTLE_SCORE:
			return m_SortGuildList;
		case SORTLIST_GUILD_BATTLE_MEMBER:
			return m_SortGuildMemberList;
		case SORTLIST_GUILD_MEMBER_SCORE:
		{
			std::map<UInt64, TerritorySortEntryList>::iterator iter = m_SortGuildMemberMap.find(guildId);
			if (iter == m_SortGuildMemberMap.end())
			{
				m_SortGuildMemberMap.insert(std::make_pair(guildId, TerritorySortEntryList()));
			}
			return m_SortGuildMemberMap[guildId];
		}
		default:
			break;
		}
	}
	return NullTerritorySortList;
}

void CSTerritorySortListMgr::ClearSortList()
{
	m_SortGuildList.clear();
	m_SortGuildMemberList.clear();
	m_SortGuildMemberMap.clear();
}

bool CSGuildBattleTerritory::Init(GuildTerritoryDataEntry* dataEntry)
{
	if (dataEntry == NULL) return false;
	m_DataEntry = dataEntry;
	return true;
}

UInt32 CSGuildBattleTerritory::AddGuildCrossBattleRecord(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd)
{
	std::vector<ObjID_t>::iterator iter = std::find(m_AlreadyRecordRaceIds.begin(), m_AlreadyRecordRaceIds.end(), raceEnd.sessionId);
	if (iter != m_AlreadyRecordRaceIds.end())
	{
		m_AlreadyRecordRaceIds.erase(iter);
		return ErrorCode::SUCCESS;
	}

	GuildBattleRecord record;
	record.loser = raceEnd.loser;
	record.winner = raceEnd.winner;
	record.time = (UInt32)CURRENT_TIME.Sec();

	AddGuildBattleRecord(record);

	m_AlreadyRecordRaceIds.push_back(raceEnd.sessionId);
	return ErrorCode::SUCCESS;
}

void CSGuildBattleTerritory::AddGuildBattleRecord(GuildBattleRecord& record)
{
	record.index = m_Records.size();
	m_Records.push_back(record);

	CLProtocol::WorldCrossGuildBattleRecordInfoSync sync;
	sync.terrId = GetTerritoryId();
	sync.info = record;
	GameZoneMgr::Instance()->TransmitToWorld(sync);
}

void CSGuildBattleTerritory::ClearGuildBattleRecord()
{
	m_Records.clear();
	m_SortListMgr.ClearSortList();
}

void CSGuildBattleTerritory::GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count)
{
	if (m_Records.empty()) return;
	UInt32 tmpIndex = (UInt32)startIndex;
	if ((startIndex < 0) ||
		(tmpIndex >(m_Records.size() - 1)))
	{
		tmpIndex = m_Records.size() - 1;
	}

	UInt32 tmpCount = count >(tmpIndex + 1) ? tmpIndex + 1 : count;
	tmpIndex = tmpIndex + 1 - tmpCount;

	record.assign(m_Records.begin() + tmpIndex, m_Records.begin() + tmpIndex + tmpCount);
}

void CSGuildBattleTerritory::GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId)
{
	std::vector<GuildBattleRecord>::iterator iter = m_Records.begin();
	while (iter != m_Records.end())
	{
		if (iter->winner.id == playerId || iter->loser.id == playerId)
		{
			record.push_back(*iter);
		}
		++iter;
	}
}

void CSGuildBattleTerritory::UpdateSortList(TerritorySortListEntry& entry)
{
	m_SortListMgr.UpdateSortList(entry);
}

void CSGuildBattleTerritory::ListSortEntries(GameZone* zone, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count)
{
	m_SortListMgr.ListSortEntries(zone, playerId, guildId, sortType, start, count);
}

UInt32 CSGuildBattleTerritory::GetSortFirstGuild(GuildBattleEndInfo& info)
{
	return m_SortListMgr.GetSortFirstGuild(info);
}

UInt32 CSGuildBattleTerritory::GetRanking(SortListType sortType, ObjID_t playerId, ObjID_t guildId)
{
	return m_SortListMgr.GetRanking(sortType, playerId, guildId);
}