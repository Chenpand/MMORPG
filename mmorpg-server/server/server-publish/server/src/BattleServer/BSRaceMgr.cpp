#include "BSRaceMgr.h"
#include <CLMatchDefine.h>
#include <CLLostDungeonDefine.h>
#include "BSLostPKRace.h"
#include "BSLostMonsterRace.h"
#include "BSLostTeamDungeonRace.h"
#include "BSPlayer.h"

BSRaceMgr::BSRaceMgr()
{

}

BSRaceMgr::~BSRaceMgr()
{
}

ObjID_t	BSRaceMgr::GenRaceId()
{
	// 生成唯一的比赛ID,括号内为相应字段的位数
	// NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return raceID;
}

BSBaseRace* BSRaceMgr::CreateRace(UInt8 raceType)
{
	if (LDRT_PVP == raceType)
	{
		return new BSLostPKRace(raceType);
	}
	else if(LDRT_MONSTER == raceType)
	{
		return new BSLostMonsterRace(raceType);
	}
	else if (LDRT_DUNGEON == raceType)
	{
		return new BSLostTeamDungeonRace(raceType);
	}
	return NULL;
}

void BSRaceMgr::DestoryRace(BSBaseRace* race)
{
	if (!race)	return;
	m_races.erase(race->GetRaceID());
	CL_SAFE_DELETE(race);
}

void BSRaceMgr::AddRace(BSBaseRace* race)
{
	auto iter = m_races.find(race->GetRaceID());
	if (iter != m_races.end())
	{
		CL_SAFE_DELETE(iter->second);
		iter->second = race;
	}
	else
	{
		m_races[race->GetRaceID()] = race;
	}
}

BSBaseRace* BSRaceMgr::FindRace(ObjID_t raceId)
{
	auto it = m_races.find(raceId);
	if (it == m_races.end())
	{
		return NULL;
	}
	return it->second;
}

void BSRaceMgr::RemoveRace(UInt64 raceId)
{
	m_races.erase(raceId);
}

void BSRaceMgr::OnTick(const Avalon::Time& now)
{
	std::vector<BSBaseRace*> allRaceNeedToDestory;
	for (auto itr : m_races)
	{
		auto race = itr.second;
		if (!race)
		{
			continue;
		}

		race->OnTick(now);

		if (race->IsTimeToDestory(now))
		{
			allRaceNeedToDestory.push_back(race);
		}
	}

	for (auto race : allRaceNeedToDestory)
	{
		DestoryRace(race);
	}
}

BSBaseRace* BSRaceMgr::FindRaceByRoleId(UInt64 roleId)
{
	auto itr = m_roleId2Race.find(roleId);
	return itr == m_roleId2Race.end() ? NULL : itr->second;
}

void  BSRaceMgr::OnPlayerJoinRace(BSBaseRace* race, BSPlayer* player)
{
	if (!race || !player)
	{
		return;
	}
	m_roleId2Race[player->GetID()] = race;
	race->OnPlayerJoin(player);
}

void BSRaceMgr::OnPlayerLeaveRace(UInt64 roleId, bool isDisconnect)
{
	auto itr = m_roleId2Race.find(roleId);
	if (itr == m_roleId2Race.end())
	{
		return;
	}

	auto race = itr->second;
	if (race)
	{
		race->OnPlayerLeave(roleId, isDisconnect);
	}
	m_roleId2Race.erase(itr);
}