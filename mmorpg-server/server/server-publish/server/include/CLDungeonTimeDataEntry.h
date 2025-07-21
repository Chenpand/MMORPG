#ifndef _CL_DUNGEON_TIME_DATAENTRY_H_
#define _CL_DUNGEON_TIME_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//活动data
class DungeonFinishTimeDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return MakeKey(dungeonId, floor); }

	static UInt32 MakeKey(UInt32 dungeonId, UInt32 floor) { return dungeonId * 100 + floor; }

	bool Read(Avalon::DataRow& row);

public:
	// 地下城ID,如果是死亡之塔代表层数
	UInt32		dungeonId;
	// 层数
	UInt16		floor;
	// 29级最小时间
	UInt32		minTime_29;
	// 39级最小时间
	UInt32		minTime_39;
	// 49级最小时间
	UInt32		minTime_49;
	// 59级最小时间
	UInt32		minTime_59;
	// 69级最小时间
	UInt32		minTime_69;
};

typedef Avalon::DataEntryMgr<DungeonFinishTimeDataEntry> DungeonFinishTimeDataEntryMgr;

#endif
