#include "RaceMgr.h"
#include "PvPRace.h"
#include "DungeonRace.h"
#include "PkRoomRace.h"
#include "RLPlayerMgr.h"
#include "ChampionPvPRace.h"
#include "ObservedRace.h"

RaceMgr::RaceMgr()
{

}

RaceMgr::~RaceMgr()
{

}

bool RaceMgr::OnInit(Avalon::PropertySet* prop)
{
	m_physicalFPS = prop->GetProperty("FrameRate", "physical_fps");
	m_logicalFPS = prop->GetProperty("FrameRate", "logical_fps");

	m_reportTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);

	return true;
}

void RaceMgr::OnTick(const Avalon::Time& now)
{
    std::vector<Race*> needDelRaces;
    for (auto itr : m_races)
    {
        auto race = itr.second;
        if (!race)
        {
            continue;            
        }

        race->OnTick(now);

		if (race->GetStatus() == RACE_STATUS_DESTORY && race->GetDestoryTime() < (UInt32)now.Sec() && race->GetObserverNum() == 0)
        {
            needDelRaces.push_back(race);
        }
    }

    for (auto race : needDelRaces)
    {
        if (race)
        {
            DelRace(race->GetID());
            delete race;
        }
    }

	if (m_reportTimer.IsTimeout(now))
	{
		InfoLogStream << "current race(" << (UInt32)m_races.size() << ") player(" << RLPlayerMgr::Instance()->get_player_count() << ")." << LogStream::eos;

		// 比赛统计
		for (UInt32 i = 0; i < RACE_NUM; i++)
		{
			auto& sta = m_raceStatistic[i];
			InfoLogStream << "race(" << i << ") create(" << sta.createNum << ") start(" << sta.startNum << ") unsync(" << sta.unsyncNum << ")." << LogStream::eos;
		}
	}
}

Race* RaceMgr::CreateRace(RaceType type)
{
    Race* race = NULL;
	if (type == RT_PK || type == RT_GUILD_BATTLE || type == RT_PREMIUM_LEAGUE_PRELIMINAY 
		|| type == RT_PREMIUM_LEAGUE_BATTLE || type == RT_CHIJI || type == RT_EQULA_PK
		|| type == RT_LOST_DUNGEON_PK)
    {
        race = new PvPRace(type);
    }
	else if (type == RT_DUNGEON || type == RT_DUNGEON_CROSS)
    {
        race = new DungeonRace(type);
    }
	else if (type == RT_PK_3V3 || type == RT_SCORE_WAR || type == RT_3V3_TUMBLE || type == RT_2V2_TUMNLE || type == RT_PK_5V5)
	{
		race = new PkRoomRace(type);
	}
	else if (type == RT_CHAMPION)
	{
		race = new ChampionPvPRace(type);
	}
	else if (type == RT_OBSERVE)
	{
		race = new ObservedRace(type);
	}

    if (race)
    {
        race->Init();
		RaceMgr::Instance()->IncCreateRaceNum(race->GetType());
    }

    return race;
}

void RaceMgr::AddRace(Race* race)
{
    if (race)
    {
        m_races[race->GetID()] = race;
    }
}

Race* RaceMgr::FindRace(ObjID_t id)
{
    auto itr = m_races.find(id);
    return itr == m_races.end() ? NULL : itr->second;
}

void RaceMgr::DelRace(ObjID_t id)
{
    m_races.erase(id);
}

const ReconnectInfo* RaceMgr::GetReconnectInfo(ObjID_t roleId)
{
	auto itr = m_reconnInfo.find(roleId);
	if (itr == m_reconnInfo.end())
	{
		return NULL;
	}

	UInt32 currentSec = (UInt32)CURRENT_TIME.Sec();
	if (itr->second.timeoutTime < currentSec)
	{
		m_reconnInfo.erase(itr);
		return NULL;
	}

	return &(itr->second);
}

void RaceMgr::DelReconnectInfo(ObjID_t roleId)
{
	m_reconnInfo.erase(roleId);
}

void RaceMgr::AddReconnectInfo(ObjID_t roleId, ObjID_t raceId, UInt32 timeoutTime)
{
	ReconnectInfo info;
	info.roldId = roleId;
	info.raceId = raceId;
	info.timeoutTime = timeoutTime;

	m_reconnInfo[roleId] = info;
}

UInt32 RaceMgr::GetObserveLoad()
{
	UInt32 totalLoad = 0;
	for (auto itr : m_races)
	{
		auto race = itr.second;
		if (!race)
		{
			continue;
		}
		totalLoad += race->GetObserveLoad();
	}
	return totalLoad;
}

void RaceMgr::IncCreateRaceNum(RaceInstanceType type)
{
	if (type >= RACE_NUM)
	{
		return;
	}

	m_raceStatistic[type].createNum++;
}

void RaceMgr::IncStartRaceNum(RaceInstanceType type)
{
	if (type >= RACE_NUM)
	{
		return;
	}

	m_raceStatistic[type].startNum++;
}

void RaceMgr::IncUnsyncRaceNum(RaceInstanceType type)
{
	if (type >= RACE_NUM)
	{
		return;
	}

	m_raceStatistic[type].unsyncNum++;
}