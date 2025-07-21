#ifndef _CL_DUNGEON_GUARANTEE_DROP_DATAENTRY_H_
#define _CL_DUNGEON_GUARANTEE_DROP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//活动data
class DungeonGuaranteeDropDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt32	id;
	// 地下城id
	UInt32	dungeonId;
	// 最小次数
	UInt32	minTimes;
	// 最大次数
	UInt32	maxTimes;
	// 掉落方案
	UInt32	dropSetId;
	// 重置类型
	UInt32	resetType;
	// VIP等级限制下限
	UInt32	vipLevelLimitMin;
	// VIP等级限制上限
	UInt32	vipLevelLimitMax;
};

//typedef Avalon::DataEntryMgr<DungeonGuaranteeDropDataEntry> DungeonGuaranteeDropDataEntryMgr;

class DungeonGuaranteeDropDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonGuaranteeDropDataEntry>
	, public Avalon::Singleton<DungeonGuaranteeDropDataEntryMgr>
{
public:
	std::vector<DungeonGuaranteeDropDataEntry*> GetDataEntryByDungeonID(UInt32 dungeonID);
};

#endif
