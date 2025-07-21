#ifndef _CL_GUILD_DATAENTRY_H_
#define _CL_GUILD_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLGuildDefine.h>

/**
*@brief 公会数据项
*/
struct GuildDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return level; }

	bool Read(Avalon::DataRow& row);

public:
	//公会等级
	UInt8	level;
	//成员数量
	UInt32	memberNum;
	//各建筑最高等级
	UInt8	buildingMaxLevel[GUILD_BUILDING_NUM];
};

typedef Avalon::DataEntryMgr<GuildDataEntry> GuildDataEntryMgr;

#endif
