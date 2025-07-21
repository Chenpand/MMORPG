#include "WSActivityMonsterMgr.h"
#include <CLActivityMonsterDataEntry.h>
#include <CLDungeonDataEntry.h>
#include <CLActivitySuitAdditionDataEntry.h>
#include <CLActivityMonsterProbDataEntry.h>
#include <CLMsgParser.h>
#include <CLEqualItemDataEntry.h>
#include "SysRecordMgr.h"
#include "AnnouncementMgr.h"
#include "DungeonPlayer.h"
#include "WSActivityMgr.h"
#include "WSApplication.h"
#include "WSPlayerMgr.h"

WSActivityMonsterMgr::WSActivityMonsterMgr()
{
	m_Open = false;
}

WSActivityMonsterMgr::~WSActivityMonsterMgr()
{

}

bool WSActivityMonsterMgr::Init()
{
	m_Monsters = _GenMonsters();
	return true;
}

void WSActivityMonsterMgr::OnTick(const Avalon::Time& now)
{
	if (!m_Open)
	{
		return;
	}

	for (auto& monster : m_Monsters)
	{
		for (auto& roll : monster.rolls)
		{
			bool activity = false;

			if (roll.startTime <= (UInt32)now.Sec() && roll.endTime > (UInt32)now.Sec())
			{
				activity = true;
			}

			if (roll.activity != activity)
			{
				roll.activity = activity;
				if (activity)
				{
					OnMonsterActive(monster);
				}
				else
				{
					OnMonsterDeActive(monster);
				}
			}

			if (!roll.activity)
			{
				continue;
			}

			auto record = SysRecordMgr::Instance()->GetRecord(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, monster.groupId);
			auto curNum = record != NULL ? record->num : 0;
			if (monster.perRollMonsterNum > 0 && record && record->num > 0 && record->recordTime < roll.startTime)
			{
				InfoLogStream << "activity monster(" << monster.id << ") reset monster num." << LogStream::eos;
				SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, monster.groupId, 0);
				curNum = 0;
			}

			if (monster.perRollMonsterNum > 0 && monster.perRollMonsterNum <= curNum && !roll.isOver)
			{
				roll.isOver = true;
				if (monster.clearNotice > 0)
				{
					UInt32 nextRollStartTime = 0;
					UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
					// 查找下一轮开始时间
					for (auto& roll : monster.rolls)
					{
						if (roll.startTime > curSec)
						{
							nextRollStartTime = roll.startTime;
							break;
						}
					}

					if (nextRollStartTime > 0)
					{
						Avalon::Date nextRollDate(Avalon::Time((UInt64)nextRollStartTime * Avalon::Time::MSECS_OF_SEC));
						char nextRollStartStr[256];
						avalon_snprintf(nextRollStartStr, sizeof(nextRollStartStr), "%02d:%02d:%02d", (int)nextRollDate.Hour(), (int)nextRollDate.Min(), (int)nextRollDate.Sec());
						AnnouncementMgr::Instance()->PublicAnnouncement(0, monster.clearNotice, (UInt32)CURRENT_TIME.Sec(), nextRollStartStr);
					}
					else
					{
						AnnouncementMgr::Instance()->PublicAnnouncement(0, monster.overNotice, (UInt32)CURRENT_TIME.Sec());
					}
				}
			}

			if (monster.startNotice > 0 && roll.noticeTimer.GetInterval() > 0 && roll.noticeTimer.IsTimeout(now) && (monster.perRollMonsterNum == 0 || monster.perRollMonsterNum > curNum))
			{
				std::string systemLinkStr;
				MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_ACTIVITY, monster.id);
				AnnouncementMgr::Instance()->PublicAnnouncement(0, monster.startNotice, (UInt32)CURRENT_TIME.Sec(), systemLinkStr);
			}
		}
		
	}
}

void WSActivityMonsterMgr::SetOpen(bool open)
{
	if (m_Open == open)
	{
		return;
	}

	m_Open = open;
	InfoLogStream << "activity monster open(" << (open ? 1 : 0) << ")." << LogStream::eos;
}

void WSActivityMonsterMgr::Reload()
{
	InfoLogStream << "reload activity monster." << LogStream::eos;

	std::string configpath = CONFIG_PATH;
	configpath = configpath + "DataTables/";
	try
	{
		ActivityMonsterDataEntryMgr::Instance()->ReloadData(configpath + "ActivityMonsterTable.txt");

		class ActivityMonsterReloadVisitor : public Avalon::DataEntryVisitor<ActivityMonsterDataEntry>
		{
		public:
			bool operator()(ActivityMonsterDataEntry* entry)
			{
				if (entry)
				{
					datas.push_back(entry);
				}
				return true;
			}

			std::vector<ActivityMonsterDataEntry*> datas;
		};
		ActivityMonsterReloadVisitor vistor;
		ActivityMonsterDataEntryMgr::Instance()->Visit(vistor);

		for (auto data : vistor.datas)
		{
			auto monster = FindMonster(data->id);
			if (!monster)
			{
				ErrorLogStream << "can't find monster(" << data->id << ")." << LogStream::eos;
				continue;
			}

			if (monster->perRollMonsterNum == data->perRollMonsterNum)
			{
				continue;
			}

			monster->perRollMonsterNum = data->perRollMonsterNum;
			InfoLogStream << "activity monster(" << data->id << ") new num(" << monster->perRollMonsterNum << ")." << LogStream::eos;
		}
	}
	catch (std::string str)
	{
		ErrorLogStream << str << LogStream::eos;
	}
}

ActivityMonsterGroup WSActivityMonsterMgr::GetMonsterGroupID(UInt32 dungeonId, DungeonAreaPointType type)
{
	ActivityMonsterGroup group;
	if (!m_Open)
	{
		return group;
	}

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		return group;
	}

	for (auto& monster : m_Monsters)
	{
		if (!monster.activity)
		{
			continue;
		}

		if (monster.pointType != type)
		{
			continue;
		}

		if (monster.needDungeonLevel > dungeonData->level)
		{
			continue;
		}

		UInt32 curNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, monster.groupId);
		if (monster.perRollMonsterNum > 0 && curNum >= monster.perRollMonsterNum)
		{
			// 说明已经没了
			continue;
		}

		auto prob = ActivityMonsterProbDataEntryMgr::Instance()->GetProbByActivity(monster.id, dungeonData->tag);

		group.activityId = monster.id;
		group.groupId = monster.groupId;
		group.prob += prob;

		if (group.remainNum != UINT32_MAX)
		{
			if (monster.perRollMonsterNum == 0)
			{
				group.remainNum = UINT32_MAX;
			}
			else
			{
				group.remainNum += monster.perRollMonsterNum;
			}
		}
	}

	return group;
}

ActivityMonsterInfo WSActivityMonsterMgr::GetActivityMonsterInfo(UInt32 id)
{
	ActivityMonsterInfo info;
	info.activity = false;
	info.nextRollStartTime = 0;
	info.id = id;

	if (!m_Open)
	{
		return info;
	}

	for (UInt32 i = 0; i < m_Monsters.size(); i++)
	{
		auto& monster = m_Monsters[i];

		if (monster.id != id)
		{
			continue;
		}

		if (monster.perRollMonsterNum > 0)
		{
			UInt32 num = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, monster.groupId);
			if (num > monster.perRollMonsterNum)
			{
				info.remainNum = 0;
			}
			else
			{
				info.remainNum = monster.perRollMonsterNum - num;
			}
		}
		else
		{
			info.remainNum = UINT32_MAX;
		}

		info.activity = monster.activity;
		info.name = monster.name;
		info.drops = monster.mayDropItems;
		info.pointType = monster.pointType;

		UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
		// 查找下一轮开始时间
		for (auto& roll : monster.rolls)
		{
			if (roll.activity)
			{
				info.startTime = roll.startTime;
				info.endTime = roll.endTime;
			}

			if (roll.startTime > curSec)
			{
				info.nextRollStartTime = roll.startTime;
				break;
			}
		}

		break;
	}

	return info;
}

WSActivityMonster* WSActivityMonsterMgr::FindMonster(UInt32 id)
{
	for (auto& monster : m_Monsters)
	{
		if (monster.id == id)
		{
			return &monster;
		}
	}

	return NULL;
}

ActivitySuitAdditionDataEntry* WSActivityMonsterMgr::GetActivitySuitAddition(UInt32 activityMonsterId, DungeonPlayer* player)
{
	class ActivitySuitAdditionVistor : public Avalon::DataEntryVisitor<ActivitySuitAdditionDataEntry>
	{
	public:
		ActivitySuitAdditionVistor(UInt32 activityMonsterId_, DungeonPlayer* player_) : activityMonsterId(activityMonsterId_), player(player_), addition(NULL) {}

		bool operator()(ActivitySuitAdditionDataEntry* data)
		{
			if (!data || data->type != AMAT_FASHION)
			{
				return true;
			}

			if (std::find(data->activityMonsterList.begin(), data->activityMonsterList.end(), activityMonsterId) == data->activityMonsterList.end())
			{
				return true;
			}

			// 查看装备是不是都有
			bool hasSuit = false;
			for (auto suit : data->equipList)
			{
				bool hasAll = true;
				for (auto id : suit)
				{
					bool find = false;
					for (const auto& equip : player->GetEquips())
					{
						if (equip.isTimeLimit != 0)
						{
							continue;
						}

						if (equip.id == id)
						{
							find = true;
							continue;
						}

						auto equipItemData = EqualItemDataEntryMgr::Instance()->FindEntry(id);
						if (!equipItemData)
						{
							continue;
						}

						if (std::find(equipItemData->equalItemIds.begin(), equipItemData->equalItemIds.end(), equip.id) != equipItemData->equalItemIds.end())
						{
							find = true;
							continue;
						}
					}

					if (!find)
					{
						hasAll = false;
						break;
					}
				}
				
				if (hasAll)
				{
					hasSuit = true;
					break;
				}
			}

			if (!hasSuit)
			{
				return false;
			}
			
			addition = data;
			return false;
		}

		UInt32 activityMonsterId;
		DungeonPlayer* player;
		ActivitySuitAdditionDataEntry* addition;
	};

	ActivitySuitAdditionVistor vistor(activityMonsterId, player);
	ActivitySuitAdditionDataEntryMgr::Instance()->Visit(vistor);

	return vistor.addition;
}

ActivitySuitAdditionDataEntry* WSActivityMonsterMgr::GetActivityMonthCardAddition(UInt32 activityMonsterId, DungeonPlayer* player)
{
	class ActivitySuitAdditionVistor : public Avalon::DataEntryVisitor<ActivitySuitAdditionDataEntry>
	{
	public:
		ActivitySuitAdditionVistor(UInt32 activityMonsterId_, DungeonPlayer* player_) : activityMonsterId(activityMonsterId_), player(player_), addition(NULL) {}

		bool operator()(ActivitySuitAdditionDataEntry* data)
		{
			if (!data || data->type != AMAT_MONTH_CARD)
			{
				return true;
			}

			if (std::find(data->activityMonsterList.begin(), data->activityMonsterList.end(), activityMonsterId) == data->activityMonsterList.end())
			{
				return true;
			}

			auto wsPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(player->GetID());
			if (wsPlayer && wsPlayer->IsInMonthCard())
			{
				addition = data;
				return false;
			}

			return true;
		}

		UInt32 activityMonsterId;
		DungeonPlayer* player;
		ActivitySuitAdditionDataEntry* addition;
	};

	ActivitySuitAdditionVistor vistor(activityMonsterId, player);
	ActivitySuitAdditionDataEntryMgr::Instance()->Visit(vistor);

	return vistor.addition;
}

void WSActivityMonsterMgr::OnMonsterActive(WSActivityMonster& monster)
{
	monster.activity = true;
	InfoLogStream << "dungeon(" << monster.dungeonId << ") activity monster group(" << monster.groupId << ")." << LogStream::eos;
}

void WSActivityMonsterMgr::OnMonsterDeActive(WSActivityMonster& monster)
{
	monster.activity = false;
	SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, monster.groupId, 0);
}

std::vector<WSActivityMonster> WSActivityMonsterMgr::_GenMonsters()
{
	class ActivityMonsterDataEntryVisitor : public Avalon::DataEntryVisitor<ActivityMonsterDataEntry>
	{
	public:
		bool operator()(ActivityMonsterDataEntry* entry)
		{
			if (entry)
			{
				datas.push_back(entry);
			}
			return true;
		}

		std::vector<ActivityMonsterDataEntry*> datas;
	};

	std::vector<WSActivityMonster> monsters;

	ActivityMonsterDataEntryVisitor vistor;
	ActivityMonsterDataEntryMgr::Instance()->Visit(vistor);

	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	for (auto data : vistor.datas)
	{
		if (!data || data->perRollDurningSec == 0)
		{
			continue;
		}

		// 活动已经结束了
		if (data->endDate < curTime)
		{
			continue;
		}

		WSActivityMonster monster;
		monster.id = data->id;
		monster.name = data->name;
		monster.perRollMonsterNum = data->perRollMonsterNum;
		monster.perRollDurningSec = data->perRollDurningSec;
		monster.pointType = data->pointType;
		monster.groupId = data->groupId;
		monster.prob = data->prob;
		monster.drops = data->drops;
		monster.mayDropItems = data->mayDropItems;
		monster.startNotice = data->startNotice;
		monster.killNotice = data->killNotice;
		monster.clearNotice = data->clearNotice;
		monster.overNotice = data->overNotice;
		monster.needDungeonLevel = data->needDungeonLevel;

		Avalon::Date date = CURRENT_DATE;
		date.Hour(0);
		date.Min(0);
		date.Sec(0);

		UInt32 startTime = data->startDate + data->openTime.startHour * Avalon::Time::SECS_OF_HOUR + data->openTime.startMin * Avalon::Time::SECS_OF_MIN;
		UInt32 totalEndTime = data->startDate + data->openTime.endHour * Avalon::Time::SECS_OF_HOUR + data->openTime.endMin * Avalon::Time::SECS_OF_MIN;
		UInt32 rollNum = (totalEndTime - startTime) / data->perRollDurningSec;
		for (; startTime <= data->endDate; startTime += Avalon::Time::SECS_OF_DAY)
		{
			for (UInt32 i = 0; i < rollNum; i++)
			{
				WSActivityMonsterRoll roll;
				roll.startTime = startTime + i * data->perRollDurningSec;
				roll.endTime = roll.startTime + data->perRollDurningSec;
				roll.noticeTimer.SetInterval(5 * Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);

				if (curTime >= roll.endTime)
				{
					continue;
				}

				monster.rolls.push_back(roll);
			}
		}
		
		// 检查概率表
		auto probDatas = ActivityMonsterProbDataEntryMgr::Instance()->GetDatasByActivity(monster.id);
		if (probDatas.size() == 0)
		{
			ErrorLogStream << "activity monster(" << monster.id << ") no prob data." << LogStream::eos;
			WSApplication::Instance()->Exit();
		}

		monsters.push_back(monster);
	}

	return monsters;
}

UInt32 WSActivityMonsterMgr::_GenMonsterDungeonID(UInt32 minLevel, UInt32 maxLevel)
{
	class DungeonDataEntryVisitor : public Avalon::DataEntryVisitor<DungeonDataEntry>
	{
	public:
		DungeonDataEntryVisitor(UInt32 minLevel, UInt32 maxLevel) : minLevel_(minLevel), maxLevel_(maxLevel) {}
		bool operator()(DungeonDataEntry* entry)
		{
			if (entry && entry->type == LT_NORMAL && entry->level >= minLevel_ && entry->level <= maxLevel_ && 
				entry->subType == DST_NORMAL && entry->hardType == LHT_NORMAL && entry->id != 0)
			{
				datas.push_back(entry->id);
			}
			return true;
		}

		UInt32 minLevel_;
		UInt32 maxLevel_;
		std::vector<UInt32> datas;
	};

	DungeonDataEntryVisitor vistor(minLevel, maxLevel);
	DungeonDataEntryMgr::Instance()->Visit(vistor);

	if (vistor.datas.empty())
	{
		return 0;
	}

	//return vistor.datas[Avalon::Random::RandBetween(0, vistor.datas.size() - 1)];
	return 101000;
}