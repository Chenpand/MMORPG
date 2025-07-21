#ifndef _CL_GUILD_STATUE_DATAENTRY_H_
#define _CL_GUILD_STATUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 公会雕像数据项
*/
struct GuildStatueDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// 类型
	UInt8	type;
	//消耗道具ID
	UInt32	costItemId;
	//消耗道具数量
	UInt32	costItemNum;
	//奖励贡献
	UInt32	awardContri;
	//获得BUFFER
	UInt32	bufferId;
};

typedef Avalon::DataEntryMgr<GuildStatueDataEntry> GuildStatueEntryMgr;

#endif
