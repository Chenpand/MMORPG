#ifndef _CL_GUILD_BUILDING_COST_DATAENTRY_H_
#define _CL_GUILD_BUILDING_COST_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLGuildDefine.h>

/**
*@brief 公会建筑数据项
*/
struct GuildBuildingDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	// 等级
	UInt8	level;
	// 各建筑升到这级消耗
	UInt32	buildingCost[GUILD_BUILDING_NUM];
	// 商店ID
	UInt8	shopId;
	// 福利社礼盒ID
	UInt32	welfareGiftId;
};

typedef Avalon::DataEntryMgr<GuildBuildingDataEntry> GuildBuildingDataMgr;

#endif
