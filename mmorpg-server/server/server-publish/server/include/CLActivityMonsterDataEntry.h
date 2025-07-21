#ifndef __ACTIVITY_MONSTER_DATA_ENTRY_H__
#define __ACTIVITY_MONSTER_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDropItemDefine.h"
#include "CLDungeonAreaDataEntry.h"

struct ActivityOpenTime
{
	UInt32 startHour;
	UInt32 startMin;
	UInt32 endHour;
	UInt32 endMin;
};

struct ActivityMonsterDropInfo
{
	UInt16 minLevel;
	UInt16 maxLevel;
	UInt32 dropSetId;
};

/**
*@brief 活动怪物表
*/
class ActivityMonsterDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// 名字
	std::string				name;
	// 活动开启日期
	UInt32					startDate;
	// 活动结束日期
	UInt32					endDate;
	// 每日开启时间
	ActivityOpenTime		openTime;
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
	// 掉落
	std::vector<ActivityMonsterDropInfo>	drops;
	// 可能掉落
	std::vector<DropItem>	mayDropItems;
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

typedef Avalon::DataEntryVisitor<ActivityMonsterDataEntry> ActivityMonsterDataEntryVisitor;
typedef Avalon::DataEntryMgr<ActivityMonsterDataEntry> ActivityMonsterDataEntryMgr;

#endif