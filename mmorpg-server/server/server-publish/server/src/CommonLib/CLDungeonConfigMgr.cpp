#include "CLDungeonConfigMgr.h"

CLDungeonConfigMgr::CLDungeonConfigMgr()
{

}

CLDungeonConfigMgr::~CLDungeonConfigMgr()
{

}

void CLDungeonConfigMgr::SetDungeonOpen(UInt32 dungeonId, UInt8 maxTimes)
{
	if (IsDungeonOpened(dungeonId))
	{
		return;
	}

	m_openedDungeon[dungeonId] = maxTimes;
	OnDungeonOpened(dungeonId, maxTimes);
}

void CLDungeonConfigMgr::SetDungeonClose(UInt32 dungeonId)
{
	if (!IsDungeonOpened(dungeonId))
	{
		return;
	}

	m_openedDungeon.erase(dungeonId);
	OnDungeonClosed(dungeonId);
}

bool CLDungeonConfigMgr::IsDungeonOpened(UInt32 dungeonId)
{
	if (m_dungeonConfigs.find(dungeonId) == m_dungeonConfigs.end())
	{
		return true;
	}

	return m_openedDungeon.find(dungeonId) != m_openedDungeon.end();
}

UInt8 CLDungeonConfigMgr::GetDungeonMaxTimes(UInt32 dungeonId)
{
	auto itr = m_openedDungeon.find(dungeonId);
	return itr == m_openedDungeon.end() ? 0 : itr->second;
}

void CLDungeonConfigMgr::AddWeeklyConfig(UInt32 dungeonId, const DungeonWeeklyConfig& config)
{
	ClearConfig(dungeonId);
	m_dungeonConfigs[dungeonId] = config;
}

void CLDungeonConfigMgr::AddConfig(UInt32 dungeonId, UInt8 dayOfWeek, const VDungeonOpenTime& openTimes, UInt8 maxTimes)
{
	auto& config = m_dungeonConfigs[dungeonId].dailyConfigs[dayOfWeek - 1];
	config.dayOfWeek = dayOfWeek;
	config.maxTimes = maxTimes;
	config.openTimes = openTimes;
}

void CLDungeonConfigMgr::ClearConfig(UInt32 dungeonId)
{
	m_dungeonConfigs.erase(dungeonId);
	m_openedDungeon.erase(dungeonId);
}