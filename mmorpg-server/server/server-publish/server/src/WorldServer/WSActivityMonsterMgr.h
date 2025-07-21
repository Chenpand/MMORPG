#ifndef __WS_ACTIVITY_MONSTER_MGR_H__
#define __WS_ACTIVITY_MONSTER_MGR_H__

#include <CLDefine.h>
#include <CLDungeonAreaDataEntry.h>
#include <AvalonSingleton.h>
#include <CLActivityMonsterDataEntry.h>
#include <CLActivityDefine.h>
#include <AvalonSimpleTimer.h>

#include "DungeonMonsterGenerator.h"

// 活动怪物每一轮信息
struct WSActivityMonsterRoll
{
	WSActivityMonsterRoll() : activity(false), startTime(0), endTime(0), isOver(false) {}

	bool					activity;
	// 开始时间
	UInt32					startTime;
	// 结束时间
	UInt32					endTime;
	// 提示定时器
	Avalon::SimpleTimer		noticeTimer;
	// 是否已经刷完了
	bool					isOver;
};
typedef std::vector<WSActivityMonsterRoll> VWSActivityMonsterRoll;

// 当前激活的活动怪物
struct WSActivityMonster
{
	WSActivityMonster() : activity(false) {}

	UInt32 GetDropSetByLevel(UInt32 level)
	{
		for (auto& drop : drops)
		{
			if (drop.minLevel <= level && drop.maxLevel >= level)
			{
				return drop.dropSetId;
			}
		}

		return 0;
	}

	// 是否激活状态
	bool					activity;
	// 活动模板ID
	UInt32					id;
	// 名字
	std::string				name;
	// 地下城ID
	UInt32					dungeonId;
	// 每轮怪物数量
	UInt32					perRollMonsterNum;
	// 每轮持续时间(s)
	UInt32					perRollDurningSec;
	// 刷怪点类型
	DungeonAreaPointType	pointType;
	// 刷怪表ID
	UInt32					groupId;
	// 概率
	UInt32					prob;
	// 掉落ID
	std::vector<ActivityMonsterDropInfo>	drops;
	// 可能掉落
	std::vector<DropItem>	mayDropItems;
	// 各轮信息
	VWSActivityMonsterRoll	rolls;
	// 活动开始提示
	UInt32					startNotice;
	// 杀怪提示
	UInt32					killNotice;
	// 怪刷完提示
	UInt32					clearNotice;
	// 活动结束提示
	UInt32					overNotice;
	// 地下城等级要求(最小等级)
	UInt32					needDungeonLevel;
};

class ActivitySuitAdditionDataEntry;
class DungeonPlayer;
class WSActivityMonsterMgr : public Avalon::Singleton<WSActivityMonsterMgr>
{
public:
	WSActivityMonsterMgr();
	~WSActivityMonsterMgr();

public:
	bool Init();
	void OnTick(const Avalon::Time& now);

	void Reload();

	void SetOpen(bool open);

public:
	ActivityMonsterGroup GetMonsterGroupID(UInt32 dungeonId, DungeonAreaPointType type);
	ActivityMonsterInfo GetActivityMonsterInfo(UInt32 id);
	WSActivityMonster* FindMonster(UInt32 id);

	/**
	*@brief 获取时装加成
	*/
	ActivitySuitAdditionDataEntry* GetActivitySuitAddition(UInt32 activityMonsterId, DungeonPlayer* player);

	/**
	*@brief 获取月卡加成
	*/
	ActivitySuitAdditionDataEntry* GetActivityMonthCardAddition(UInt32 activityMonsterId, DungeonPlayer* player);

public:
	void OnMonsterActive(WSActivityMonster& monster);
	void OnMonsterDeActive(WSActivityMonster& monster);

protected:
	std::vector<WSActivityMonster> _GenMonsters();
	UInt32 _GenMonsterDungeonID(UInt32 minLevel, UInt32 maxLevel);

private:
	// 是否开启活动怪物
	bool									m_Open;
	// 激活了的活动
	std::vector<WSActivityMonster>			m_Monsters;
};

#endif