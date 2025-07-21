#include "SSDungeonConfigMgr.h"
#include "CounterMgr.h"
#include "ActivityMgr.h"
#include <CLDungeonDataEntry.h>
#include <CLDungeonTimesDataEntry.h>
#include "Player.h"

SSActivityDungeonMgr::SSActivityDungeonMgr()
{

}
SSActivityDungeonMgr::~SSActivityDungeonMgr()
{

}

bool SSActivityDungeonMgr::Init()
{
	class DungeonTimesVisitor : public Avalon::DataEntryVisitor<DungeonTimesDataEntry>
	{
	public:
		bool operator()(DungeonTimesDataEntry* dataEntry)
		{
			CounterCfg::Instance()->RegCounter(dataEntry->usedTimesCounter.c_str(), COUNTER_CYCLE_DAY, 6);
			CounterCfg::Instance()->RegCounter(dataEntry->buyTimesCounter.c_str(), COUNTER_CYCLE_DAY, 6);
			if (dataEntry->weekUsedTimesCounter.size() > 0)
			{
				CounterCfg::Instance()->RegCounter(dataEntry->weekUsedTimesCounter.c_str(), COUNTER_CYCLE_WEEK_SPECIFY, 1);
			}
			return true;
		}
	};

	DungeonTimesVisitor vistor;
	DungeonTimesDataEntryMgr::Instance()->Visit(vistor);

	return true;
}

void SSActivityDungeonMgr::RegActivityDungeon(DungeonSubType type, UInt32 maxTimes, UInt32 activityId, VipPrivilegeType vipPrivilege)
{
	ActivityDungeonConfig config;
	config.type = type;
	config.activityId = activityId;
	config.maxTimes = maxTimes;
	config.vipPrivilege = vipPrivilege;

	m_dungeonConfigs[type] = config;

	// ×¢²ácounter
	if (maxTimes > 0)
	{
		std::string counterName = DUNGEON_DAILY_COUNT_PREFIX + Avalon::StringUtil::ConvertToString((UInt32)type);
		CounterCfg::Instance()->RegCounter(counterName.c_str(), COUNTER_CYCLE_DAY, 6);
	}
}

void SSActivityDungeonMgr::DelActivityDungeon(DungeonSubType type)
{
	m_dungeonConfigs.erase(type);
}

bool SSActivityDungeonMgr::IsDungeonOpened(UInt32 dungeonId)
{
	auto data = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!data)
	{
		return false;
	}

	auto activityId = data->activityId;
	return activityId == 0 ? true : ActivityMgr::Instance()->IsInActivity(activityId);
}

bool SSActivityDungeonMgr::IsDungeonTimesLimit(UInt32 dungeonId)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return false;
	}

	auto timesData = DungeonTimesDataEntryMgr::Instance()->FindEntry(dungeonData->subType);
	if (!timesData)
	{
		return false;
	}

	return timesData->baseTimes > 0;
}

UInt32 SSActivityDungeonMgr::GetDungeonMaxTimes(Player* player, DungeonSubType type)
{
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(type);
	if (data == NULL)
	{
		return 0;
	}

	UInt32 buyedTimes = player->GetCount(data->buyTimesCounter.c_str());
	return data->baseTimes + buyedTimes;
}

UInt32 SSActivityDungeonMgr::GetDungeonBuyTimes(Player* player, DungeonSubType type)
{
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(type);
	if (data == NULL)
	{
		return 0;
	}

	UInt32 buyedTimes = player->GetCount(data->buyTimesCounter.c_str());
	return buyedTimes;
}

UInt32 SSActivityDungeonMgr::GetDungeonWeekMaxTimes(Player* player, DungeonSubType type)
{
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(type);
	if (data == NULL)
	{
		return 0;
	}

	return data->weekTimesLimit;
}

bool SSActivityDungeonMgr::CheckWeekTimes(Player* player, DungeonSubType type)
{
	auto data = DungeonTimesDataEntryMgr::Instance()->FindEntry(type);
	if (data == NULL)
	{
		return true;
	}
	UInt32 maxtimes = GetDungeonWeekMaxTimes(player, type);
	if (maxtimes == 0)
	{
		return true;
	}
	auto weekCount = player->GetCounter(data->weekUsedTimesCounter.c_str());
	if (weekCount < maxtimes)
	{
		return true;
	}
	return false;
}