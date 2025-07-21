#ifndef _CL_DUNGEON_TIMES_DATAENTRY_H_
#define _CL_DUNGEON_TIMES_DATAENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLDefine.h"
#include "CLVipDefine.h"
#include "CLDungeonDataEntry.h"

// 地下城次数
class DungeonTimesDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// 地下城子类型
	DungeonSubType		type;
	// 基础次数
	UInt8				baseTimes;
	// 基础可购买次数
	UInt8				baseBuyTimes;
	// 额外购买次数的VIP特权
	VipPrivilegeType	buyTimesVipPrivilege;
	// 购买次数消耗的货币
	UInt32				buyTimesCostItemId;
	// 购买次数消耗货币数量
	std::vector<UInt32> buyTimesCosts;
	// 已经打过的次数counter
	std::string			usedTimesCounter;
	// 额外购买的次数counter
	std::string			buyTimesCounter;
	// 每周打过的次数counter
	std::string			weekUsedTimesCounter;
	// 每周可打次数
	UInt32				weekTimesLimit;
	// 账号每日可打次数
	UInt32				accountDailyTimesLimit;
};

typedef Avalon::DataEntryMgr<DungeonTimesDataEntry> DungeonTimesDataEntryMgr;

#endif
