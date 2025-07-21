#ifndef _CL_DUNGEON_DAILY_DROP_DATAENTRY_H_
#define _CL_DUNGEON_DAILY_DROP_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLItemDefine.h>
#include <DropItemMgr.h>

/**
*@brief 地下城每日掉落表
*/
class DungeonDailyDropDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	// ID
	UInt32	id;
	// 每日前几次掉落
	UInt32	dailyNum;
	// 掉落物品
	VDropItemElem	items;
};

typedef Avalon::DataEntryMgr<DungeonDailyDropDataEntry> DungeonDailyDropDataEntryMgr;

#endif
