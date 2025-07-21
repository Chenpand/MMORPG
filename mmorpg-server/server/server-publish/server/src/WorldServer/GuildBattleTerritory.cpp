#include "GuildBattleTerritory.h"
#include "GuildMgr.h"
#include "GuildBattleMgr.h"
#include "WSServiceSwitchMgr.h"
#include "WSPlayer.h"
#include <CLGuildProtocol.h>

void TerritorySortListMgr::UpdateSortList(TerritorySortListEntry& entry)
{
	if (!IsGuildBattleSortList(entry.sortType)) return;
	if (!GuildBattleMgr::Instance()->IsGuildBattle()) return;

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

void TerritorySortListMgr::ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count)
{
	if (player == NULL || !IsGuildBattleSortList(sortType)) return;
	TerritorySortEntryList& list = GetSortEntryList(sortType, player->GetGuildID());
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

			if ((IsGuildSortList(iter->sortType) && iter->guildId == player->GetGuildID()) ||
				(!IsGuildSortList(iter->sortType) && iter->playerId == player->GetID()))
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

	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}

UInt32 TerritorySortListMgr::GetRanking(SortListType sortType, WSPlayer* player)
{
	if (player == NULL || !IsGuildBattleSortList(sortType)) return 0;
	UInt32 ranking = 0;
	TerritorySortEntryList& list = GetSortEntryList(sortType, player->GetGuildID());
	for (TerritorySortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		ranking++;
		if (IsGuildSortList(sortType))
		{
			if (iter->guildId == player->GetGuildID())
				return ranking;
		}
		else
		{
			if (iter->playerId == player->GetID())
				return ranking;
		}
	}
	return 0;
}

UInt32 TerritorySortListMgr::GetNumRankScoreSum(SortListType sortType, UInt64 guildId, UInt32 num)
{
	if (!IsGuildBattleSortList(sortType))
		return 0;

	UInt32 scoreSum = 0;
	TerritorySortEntryList& list = GetSortEntryList(sortType, guildId);
	for (TerritorySortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		if (num == 0)
			break;
		
		--num;
		scoreSum += iter->score;
	}

	return scoreSum;
}

void TerritorySortListMgr::AddToList(TerritorySortListEntry& entry)
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


static TerritorySortListMgr::TerritorySortEntryList NullSortList;
TerritorySortListMgr::TerritorySortEntryList& TerritorySortListMgr::GetSortEntryList(SortListType sortType, UInt64 guildId)
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
	return NullSortList;
}

void TerritorySortListMgr::ClearSortList()
{
	m_SortGuildList.clear();
	m_SortGuildMemberList.clear();
	m_SortGuildMemberMap.clear();
}

bool GuildBattleTerritory::Init(GuildTerritoryDataEntry* dataEntry)
{
	if (dataEntry == NULL) return false;
	m_DataEntry = dataEntry;
	return true;
}

void GuildBattleTerritory::OnBattleRaceEnd(GuildBattleMember& winnerMember, GuildBattleMember& loserMember)
{
	if (m_DataEntry->guildBattleScore.empty()) return;
	UInt32 maxWinStreak = m_DataEntry->guildBattleScore.size();
	UInt32 endScore = 0;
	//终结积分
	if (loserMember.winStreak >= 1)
	{
		UInt32 loserWinStreak = loserMember.winStreak <= maxWinStreak ? loserMember.winStreak : maxWinStreak;
		endScore = m_DataEntry->guildBattleScore[loserWinStreak - 1].endScore;
		loserMember.gotScore = m_DataEntry->guildBattleScore[loserWinStreak - 1].loserScore;
	}
	else
	{
		loserMember.gotScore = m_DataEntry->guildBattleScore[0].loserScore;
	}

	winnerMember.winStreak++;
	UInt32 winnerWinStreak = winnerMember.winStreak <= maxWinStreak ? winnerMember.winStreak : maxWinStreak;
	winnerMember.gotScore = endScore + m_DataEntry->guildBattleScore[winnerWinStreak - 1].winScore;

}

void GuildBattleTerritory::OnEnrollBegin()
{
	//公会自动报名上一级领地争夺
	if (m_Guild == NULL) return;
	UInt32 terrId = GetPreTerritoryId();
	if (terrId == 0)
		terrId = GetTerritoryId();
	UInt32 errorcode = GuildBattleMgr::Instance()->AddEnroll(m_Guild, terrId, 0);
	InfoLogStream << "on enroll terr(" << GetTerritoryId() << " ->" << terrId << ") begin guild(" << m_Guild->GetID() << ") battle error:" << errorcode << LogStream::eos;
}

void GuildBattleTerritory::AutoEnroll()
{
	//公会自动报名领地争夺
	if (m_Guild == NULL) return;
	UInt8 terrId = GetTerritoryId();
	UInt32 errorcode = GuildBattleMgr::Instance()->AddEnroll(m_Guild, terrId, 0);
	InfoLogStream << "on AutoEnroll terr(" << GetTerritoryId() << " ->" << terrId << ") begin guild(" << m_Guild->GetID() << ") battle error:" << errorcode << LogStream::eos;
}

void GuildBattleTerritory::AddGuildBattleRecord(GuildBattleRecord& record)
{
	record.index = m_Records.size();
	m_Records.push_back(record);

	CLProtocol::WorldGuildBattleRecordSync sync;
	sync.record = record;
	GuildBattleMgr::Instance()->SyncBattleRecord(GetTerritoryId(), sync);
}

void GuildBattleTerritory::AddGuildBattleRecord(GuildBattleMember& winnerMember, Guild* winGuild, GuildBattleMember& loserMember, Guild* loseGuild)
{
	_UpdateSortList(SORTLIST_GUILD_BATTLE_MEMBER, winnerMember, winGuild);
	_UpdateSortList(SORTLIST_GUILD_MEMBER_SCORE, winnerMember, winGuild);
	_UpdateSortList(SORTLIST_GUILD_BATTLE_SCORE, winnerMember, winGuild);

	_UpdateSortList(SORTLIST_GUILD_BATTLE_MEMBER, loserMember, loseGuild);
	_UpdateSortList(SORTLIST_GUILD_MEMBER_SCORE, loserMember, loseGuild);
	_UpdateSortList(SORTLIST_GUILD_BATTLE_SCORE, loserMember, loseGuild);

	if (!IsCrossTerritory())
	{
		GuildBattleRecord record;
		record.loser = loserMember;
		record.winner = winnerMember;
		record.time = (UInt32)CURRENT_TIME.Sec();

		AddGuildBattleRecord(record);
	}
}

void GuildBattleTerritory::ClearGuildBattleRecord()
{
	m_Records.clear();
	m_SortListMgr.ClearSortList();
}

void GuildBattleTerritory::SetGuild(Guild* guild)
{
	if (guild == NULL) m_Guild = NULL;
	if ((m_Guild == NULL) || 
		(m_Guild->GetGuildBattleScore() < guild->GetGuildBattleScore()))
	{
		m_Guild = guild;
	}
}

void GuildBattleTerritory::GetRecords(std::vector<GuildBattleRecord>& record, Int32 startIndex, UInt32 count)
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

void GuildBattleTerritory::GetSelfRecords(std::vector<GuildBattleRecord>& record, UInt64 playerId)
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

void GuildBattleTerritory::ListSortEntries(WSPlayer* player, SortListType sortType, UInt16 start, UInt16 count)
{
	if (player == NULL) return;

	if (GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		CLProtocol::CrossGuildBattleSortListReq req;
		req.guildId = player->GetGuildID();
		req.num = count;
		req.playerId = player->GetID();
		req.start = start;
		req.terrId = GetTerritoryId();
		req.type = (UInt8)sortType;
		Router::SendToCross(req);
	}
	else
	{
		m_SortListMgr.ListSortEntries(player, sortType, start, count);
	}
}

UInt32 GuildBattleTerritory::GetRanking(SortListType sortType, WSPlayer* player)
{
	return m_SortListMgr.GetRanking(sortType, player);
}

std::string GuildBattleTerritory::GetGuildName()
{
	if (GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
	{
		return m_CrossGuildName;
	}
	else
	{
		if (m_Guild != NULL)
		{
			return m_Guild->GetName();
		}
	}
	return std::string();
}

void GuildBattleTerritory::InitCrossSortList(Guild* guild)
{
	if (guild == NULL) return;
	std::vector<GuildMember*> members;
	guild->GetGuildMembers(members);

	std::vector<GuildMember*>::iterator iter = members.begin();
	while (iter != members.end())
	{
		GuildMember* member = *iter;

		GuildBattleMember battleMember;
		if (member != NULL)
		{
			battleMember.id = member->GetID();
			battleMember.gotScore = 0;
			battleMember.name = member->GetName();
			battleMember.totalScore = member->GetGuildBattleScore();
			battleMember.winStreak = member->GetGuildBattleWinStreak();

			_UpdateSortList(SORTLIST_GUILD_BATTLE_MEMBER, battleMember, guild);
			_UpdateSortList(SORTLIST_GUILD_MEMBER_SCORE, battleMember, guild);

		}

		_UpdateSortList(SORTLIST_GUILD_BATTLE_SCORE, battleMember, guild);

		++iter;
	}
}

void GuildBattleTerritory::_UpdateSortList(SortListType sortType, GuildBattleMember& winnerMember, Guild* winGuild)
{
	if (winGuild != NULL && winnerMember.id != 0)
	{
		TerritorySortListEntry winEntry;
		winEntry.guildId = winGuild->GetID();
		winEntry.guildName = winGuild->GetName();
		winEntry.serverName = SERVER_NAME;

		if (sortType == SortListType::SORTLIST_GUILD_BATTLE_SCORE)
		{
			if (winGuild->GetLeader() != NULL)
			{
				winEntry.playerId = winGuild->GetLeader()->GetID();
				winEntry.playerName = winGuild->GetLeader()->GetName();
			}
		}
		else
		{
			winEntry.playerId = winnerMember.id;
			winEntry.playerName = winnerMember.name;
		}

		if (IsGuildSortList(sortType))
		{
			// 新的公会战积分排行方式，取前n名的玩家积分之和
			UInt32 numRank = WSApplication::Instance()->GetGuildBattleScoreRank();
			if (GetTerritoryType() != GuildTerritoryType::GTT_CROSS && numRank != 0)
			{
				winEntry.score = m_SortListMgr.GetNumRankScoreSum(SORTLIST_GUILD_MEMBER_SCORE, winGuild->GetID(), numRank);
				UInt32 oldScore = winGuild->GetGuildBattleScore();
				winGuild->SetGuildBattleScore(winEntry.score);
				if(winEntry.score > oldScore)
					winGuild->UpdateGuildBattleWeekScoreRank(winEntry.score - oldScore);
			}
			else 
			{
				winEntry.score = winGuild->GetGuildBattleScore();
			}
		}
		else
		{
			winEntry.score = winnerMember.totalScore;
		}
		winEntry.sortType = sortType;

		if (GetTerritoryType() == GuildTerritoryType::GTT_CROSS)
		{
			CLProtocol::CrossGuildBattleSortListSync sync;
			sync.terrId = GetTerritoryId();
			sync.entry = winEntry;
			Router::SendToCross(sync);
		}
		else
		{
			m_SortListMgr.UpdateSortList(winEntry);
		}
	}
}

