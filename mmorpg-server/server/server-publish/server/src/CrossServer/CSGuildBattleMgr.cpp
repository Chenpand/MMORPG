#include "CSGuildBattleMgr.h"
#include <CLGuildBattleDataEntry.h>
#include <CLGuildProtocol.h>
#include <CLErrorCode.h>


bool CSGuildBattleMgr::Init()
{
	class GuildTerritoryDataEntryVisitor : public Avalon::DataEntryVisitor<GuildTerritoryDataEntry>
	{
	public:
		GuildTerritoryDataEntryVisitor(HashMap<UInt8, CSGuildBattleTerritory>& territory) :m_Territory(territory){}
		~GuildTerritoryDataEntryVisitor(){}

		bool operator()(GuildTerritoryDataEntry* dataEntry)
		{
			if (dataEntry->type == GuildTerritoryType::GTT_CROSS)
			{
				CSGuildBattleTerritory territory;
				if (territory.Init(dataEntry))
				{
					m_Territory[dataEntry->id] = territory;
				}
			}

			return true;
		}
	private:
		HashMap<UInt8, CSGuildBattleTerritory>& m_Territory;
	};

	GuildTerritoryDataEntryVisitor visitor(m_Territory);
	GuildTerritoryDataEntryMgr::Instance()->Visit(visitor);

	SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(CURRENT_TIME));

	_InitGuildBattleStatus();

	return true;
}

void CSGuildBattleMgr::LoadEnroll(std::map<UInt8, std::vector<CSEnrollInfo>> enrollInfo)
{
	std::map<UInt8, std::vector<CSEnrollInfo>>::iterator iter = enrollInfo.begin();
	while (iter != enrollInfo.end())
	{
		HashMap<UInt8, std::vector<CSEnrollInfo>>::iterator iterEnroll = m_Enroll.find(iter->first);
		if (iterEnroll != m_Enroll.end())
		{
			for (UInt32 i = 0; i < iter->second.size(); ++i)
			{
				std::vector<CSEnrollInfo>::iterator guildIter = std::find(iterEnroll->second.begin(), iterEnroll->second.end(), iter->second[i]);
				if (guildIter == iterEnroll->second.end())
				{
					iterEnroll->second.push_back(iter->second[i]);
				}
			}
		}
		else
		{
			for (UInt32 i = 0; i < iter->second.size(); ++i)
			{
				m_Enroll[iter->first].push_back(iter->second[i]);
			}
		}
		++iter;
	}
}

UInt32 CSGuildBattleMgr::AddEnroll(CSEnrollInfo info)
{
	//重复报名
	if (_IsEnroll(info.guildId)) return ErrorCode::GUILD_BATTLE_ENROLL_EXIST;

	m_Enroll[info.terrId].push_back(info);
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::SetTerritoryBaseInfo(std::vector<GuildTerritoryBaseInfo> info)
{
	std::vector<GuildTerritoryBaseInfo>::iterator iter = info.begin();
	while (iter != info.end())
	{
		CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(iter->terrId);
		if (pTerr != NULL)
		{
			pTerr->SetCrossGuildName(iter->guildName);
			pTerr->SetCrossOccupyServerName(iter->serverName);
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::GetTerritoryBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info)
{
	info.terrId = terrId;
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	info.guildName = pTerr->GetCrossGuildName();
	info.serverName = pTerr->GetCrossOccupyServerName();

	if (m_Status != GuildBattleStatus::GBS_INVALID)
	{
		info.enrollSize = GetEnrollSize(terrId);
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::GetTerritoryBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos)
{
	HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		GuildTerritoryBaseInfo info;
		info.terrId = iter->first;
		info.guildName = iter->second.GetCrossGuildName();
		info.serverName = iter->second.GetCrossOccupyServerName();
		if (IsGuildBattleEnroll() || IsGuildBattle())
		{
			info.enrollSize = GetEnrollSize(iter->first);
		}
		infos.push_back(info);
		++iter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::OnCrossBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd)
{
	HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.find(raceEnd.terrId);
	if (iter == m_Territory.end())
	{
		return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;
	}

	iter->second.AddGuildCrossBattleRecord(raceEnd);
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::GetBattleRecord(UInt8 terrId, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count)
{
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;
	pTerr->GetRecords(records, startIndex, count);
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::GetBattleSelfRecrod(UInt8 terrId, ObjID_t playerId, std::vector<GuildBattleRecord>& records)
{
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;
	pTerr->GetSelfRecords(records, playerId);
	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::UpdateSortList(UInt8 terrId, TerritorySortListEntry& entry)
{
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	pTerr->UpdateSortList(entry);

	return ErrorCode::SUCCESS;
}

UInt32 CSGuildBattleMgr::GetSortList(GameZone* zone, UInt8 terrId, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count)
{
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (pTerr == NULL) return ErrorCode::GUILD_BATTLE_TERRITORY_NOT_EXIST;

	pTerr->ListSortEntries(zone, playerId, guildId, sortType, start, count);

	return ErrorCode::SUCCESS;
}

void CSGuildBattleMgr::OnTick(const Avalon::Time& now)
{
	UInt32 nowSec = (UInt32)now.Sec();
	if (nowSec - m_CheckWaitTime >= 5)
	{
		for (UInt32 i = 0; i < m_WaitLoadEnrollConnIds.size(); ++i)
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(m_WaitLoadEnrollConnIds[i]);
			if (zone != NULL)
			{
				CLProtocol::WorldCrossGuildBattleEnrollInfoReq req;
				zone->TransmitToWorld(req);
			}
		}
		m_CheckWaitTime = nowSec;
	}

	if (m_IsByTime)
	{
		
		if (nowSec > m_GuildBattleTimeInfo.endTime || nowSec < m_GuildBattleTimeInfo.enrollTime)
		{
			if (m_Status != GuildBattleStatus::GBS_INVALID)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_INVALID);
				SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(now));
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.rewardTime)
		{
			if (m_Status != GuildBattleStatus::GBS_REWARD)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_REWARD);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.battleTime)
		{
			if (m_Status != GuildBattleStatus::GBS_BATTLE)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_BATTLE);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.preTime)
		{
			if (m_Status != GuildBattleStatus::GBS_PREPARE)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_PREPARE);
			}
		}
		else if (nowSec >= m_GuildBattleTimeInfo.enrollTime)
		{
			if (m_Status != GuildBattleStatus::GBS_ENROLL)
			{
				ChangeTargetStatus(GuildBattleStatus::GBS_ENROLL);
			}
		}
	}

	_ChangeToTargetStatus();

}

void CSGuildBattleMgr::OnConnected(UInt32 id)
{
	//if (IsCrossBattle())
	{
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(id);
		if (zone != NULL)
		{
			CLProtocol::WorldCrossGuildBattleEnrollInfoReq req;
			zone->TransmitToWorld(req);
			m_WaitLoadEnrollConnIds.push_back(id);
			m_CheckWaitTime = (UInt32)CURRENT_TIME.Sec();
		}
	}
}

void CSGuildBattleMgr::OnDisconnected(UInt32 id)
{
	RemoveWaitLoadEnrollConnId(id);
}

void CSGuildBattleMgr::OnEnrollBegin()
{
	InfoLogStream << " GuildBattleMgr::OnEnrollBegin " << LogStream::eos;

	m_WaitLoadEnrollConnIds.clear();
	HashMap<UInt8, std::vector<CSEnrollInfo> >::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		iter->second.clear();
		++iter;
	}

	HashMap<UInt8, CSGuildBattleTerritory>::iterator terrIter = m_Territory.begin();
	while (terrIter != m_Territory.end())
	{
		//清空战斗记录
		terrIter->second.ClearGuildBattleRecord();
		++terrIter;
	}

	_SetGuildBattleStatus(GuildBattleStatus::GBS_ENROLL);
}

void CSGuildBattleMgr::OnEnrollEnd()
{
	InfoLogStream << " GuildBattleMgr::OnEnrollEnd " << LogStream::eos;
	_SetGuildBattleStatus(GuildBattleStatus::GBS_PREPARE);
}

void CSGuildBattleMgr::OnBattlePrepare()
{
	_SetGuildBattleStatus(GuildBattleStatus::GBS_PREPARE);
}

void CSGuildBattleMgr::OnBattleBegin()
{
	InfoLogStream << " GuildBattleMgr::OnBattleBegin " << LogStream::eos;
	if (IsCrossBattle())
	{
		_ClearTerritory();
	}
	_SetGuildBattleStatus(GuildBattleStatus::GBS_BATTLE);
}

void CSGuildBattleMgr::OnBattleEnd()
{
	InfoLogStream << " GuildBattleMgr::OnBattleEnd " << LogStream::eos;
	m_GuildBattleEndInfo.clear();

	if (IsCrossBattle())
	{
		HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.begin();
		while (iter != m_Territory.end())
		{
			GuildBattleEndInfo info;
			info.terrId = iter->first;
			info.terrName = iter->second.GetTerritoryName();
			if (iter->second.GetSortFirstGuild(info) == ErrorCode::CROSS_GUILD_BATTLE_SORT_NOT_GUILD)
			{
				//没有工会
				ErrorLogStream << " GuildBattleMgr::OnBattleEnd : Terr(" << iter->first << ") GetSortFirstGuild is not Guild " << LogStream::eos;
				HashMap<UInt8, std::vector<CSEnrollInfo>>::iterator enrollIter = m_Enroll.find(iter->first);
				if (enrollIter != m_Enroll.end())
				{
					if (enrollIter->second.size() > 0)
					{
						info.guildId = enrollIter->second[0].guildId;
						info.guildLeaderName = enrollIter->second[0].guildLeaderName;
						info.guildName = enrollIter->second[0].guildName;
						info.serverName = enrollIter->second[0].serverName;
					
					}
				}
			}

			iter->second.SetCrossGuildId(info.guildId);
			iter->second.SetCrossGuildName(info.guildName);
			iter->second.SetCrossOccupyServerName(info.serverName);
			InfoLogStream << " GuildBattleMgr::OnBattleEnd : Terr(" << iter->first << ") Guild(" << info.guildId << ") " << LogStream::eos;
			m_GuildBattleEndInfo.push_back(info);
			++iter;
		}

	
		CLProtocol::WorldCrossGuildBattleEndInfoSync sync;
		sync.info = m_GuildBattleEndInfo;
		GameZoneMgr::Instance()->TransmitToWorld(sync);
	}

	_SetGuildBattleStatus(GuildBattleStatus::GBS_REWARD);
}

void CSGuildBattleMgr::OnBattleReward()
{
	_SetGuildBattleStatus(GuildBattleStatus::GBS_INVALID);
}

void CSGuildBattleMgr::ChangeTargetStatus(GuildBattleStatus targetStatus)
{
	m_TargetStatus = targetStatus;
}

bool CSGuildBattleMgr::IsCrossBattle()
{
	return m_Type == GuildBattleType::GBT_CROSS;
}

void CSGuildBattleMgr::RemoveWaitLoadEnrollConnId(UInt32 id)
{
	std::vector<UInt32>::iterator iter = std::find(m_WaitLoadEnrollConnIds.begin(), m_WaitLoadEnrollConnIds.end(), id);
	if (iter != m_WaitLoadEnrollConnIds.end())
	{
		m_WaitLoadEnrollConnIds.erase(iter);
	}
}

UInt32 CSGuildBattleMgr::GetEnrollSize(UInt32 terrId)
{
	HashMap<UInt8, std::vector<CSEnrollInfo> >::iterator iter = m_Enroll.find(terrId);
	if (iter != m_Enroll.end())
		return iter->second.size();
	return 0;
}

void CSGuildBattleMgr::SetGuildBattleTimeInfo(GuildBattleTimeInfo info)
{
	m_GuildBattleTimeInfo = info;
	SetGuildBattleType(info.type);
}

UInt32 CSGuildBattleMgr::GetRanking(UInt8 terrId, SortListType sortType, ObjID_t playerId, ObjID_t guildId)
{
	CSGuildBattleTerritory* pTerr = _GetGuildBattleTerritory(terrId);
	if (NULL == pTerr)
	{
		return 0;
	}

	return pTerr->GetRanking(sortType, playerId, guildId);
}

bool CSGuildBattleMgr::_IsEnroll(UInt64 guildId)
{
	HashMap<UInt8, std::vector<CSEnrollInfo> >::iterator iter = m_Enroll.begin();
	while (iter != m_Enroll.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (guildId == iter->second[i].guildId) return true;
		}
		++iter;
	}
	return false;
}

void CSGuildBattleMgr::_ClearTerritory()
{
	HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.begin();
	while (iter != m_Territory.end())
	{
		iter->second.SetCrossGuildId(0);
		iter->second.SetCrossGuildName("");
		iter->second.SetCrossOccupyServerName("");
		++iter;
	}
}

void CSGuildBattleMgr::_ClearTerritory(UInt8 terrId)
{
	HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		iter->second.SetCrossGuildId(0);
		iter->second.SetCrossGuildName("");
		iter->second.SetCrossOccupyServerName("");
	}
}

CSGuildBattleTerritory* CSGuildBattleMgr::_GetGuildBattleTerritory(UInt8 terrId)
{
	HashMap<UInt8, CSGuildBattleTerritory>::iterator iter = m_Territory.find(terrId);
	if (iter != m_Territory.end())
	{
		return &iter->second;
	}
	return NULL;
}

void CSGuildBattleMgr::_InitGuildBattleStatus()
{
	UInt32 nowSec = (UInt32)CURRENT_TIME.Sec();
	if (nowSec > m_GuildBattleTimeInfo.endTime || nowSec < m_GuildBattleTimeInfo.enrollTime)
	{
		m_Status = GuildBattleStatus::GBS_INVALID;
		m_TargetStatus = GuildBattleStatus::GBS_INVALID;
		SetGuildBattleTimeInfo(GuildBattleTimeDataEntryMgr::Instance()->GetGuildBattleTimeInfo(CURRENT_TIME));
	}
	else if (nowSec >= m_GuildBattleTimeInfo.rewardTime)
	{
		m_Status = GuildBattleStatus::GBS_REWARD;
		m_TargetStatus = GuildBattleStatus::GBS_REWARD;
	}
	else if (nowSec >= m_GuildBattleTimeInfo.battleTime)
	{
		m_Status = GuildBattleStatus::GBS_BATTLE;
		m_TargetStatus = GuildBattleStatus::GBS_BATTLE;
	}
	else if (nowSec >= m_GuildBattleTimeInfo.preTime)
	{
		m_Status = GuildBattleStatus::GBS_PREPARE;
		m_TargetStatus = GuildBattleStatus::GBS_PREPARE;
	}
	else if (nowSec >= m_GuildBattleTimeInfo.enrollTime)
	{
		m_Status = GuildBattleStatus::GBS_ENROLL;
		m_TargetStatus = GuildBattleStatus::GBS_ENROLL;
	}
}

void CSGuildBattleMgr::_SetGuildBattleStatus(GuildBattleStatus status)
{
	if (m_Status != status)
	{
		m_Status = status;
	}
}

void CSGuildBattleMgr::_ChangeToTargetStatus()
{
	if (m_Status == m_TargetStatus || m_TargetStatus >= GuildBattleStatus::GBS_MAX) return;
	GuildBattleStatus tmpStatus = (GuildBattleStatus)(m_Status + 1);
	if (tmpStatus >= GuildBattleStatus::GBS_MAX)
	{
		tmpStatus = GuildBattleStatus::GBS_INVALID;
	}
	switch (tmpStatus)
	{
	case GuildBattleStatus::GBS_ENROLL:
		OnEnrollBegin();
		break;
	case GuildBattleStatus::GBS_PREPARE:
		OnEnrollEnd();
		break;
	case GuildBattleStatus::GBS_BATTLE:
		OnBattleBegin();
		break;
	case GuildBattleStatus::GBS_REWARD:
		OnBattleEnd();
		break;
	case GuildBattleStatus::GBS_INVALID:
		OnBattleReward();
		break;
	default:
		m_Status = tmpStatus;
		break;
	}
	_ChangeToTargetStatus();
}
