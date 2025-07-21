#ifndef __ACTIVITY_SUIT_ADDITION_DATA_ENTRY_H__
#define __ACTIVITY_SUIT_ADDITION_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>

enum ActivityMonsterAdditionType
{
	// 时装
	AMAT_FASHION,
	// 月卡
	AMAT_MONTH_CARD,
};

/**
*@brief 活动时装加成表
*/
class ActivitySuitAdditionDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32					id;
	// 加成类型
	ActivityMonsterAdditionType		type;
	// 装备列表
	std::vector<std::vector<UInt32>> equipList;
	// 活动怪物列表
	std::vector<UInt32>		activityMonsterList;
	// 翻倍掉落物
	std::set<UInt32>		doubleDropItems;
	// 翻倍掉落概率
	UInt32					doubleDropRate;
	// 怪物刷新概率加成
	UInt32					monsterRateAddition[3];
};

typedef Avalon::DataEntryVisitor<ActivitySuitAdditionDataEntry> ActivitySuitAdditionDataEntryVisitor;
typedef Avalon::DataEntryMgr<ActivitySuitAdditionDataEntry> ActivitySuitAdditionDataEntryMgr;

#endif