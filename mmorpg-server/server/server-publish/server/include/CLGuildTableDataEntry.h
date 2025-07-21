#ifndef _CL_GUILD_TABLE_DATAENTRY_H_
#define _CL_GUILD_TABLE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLItemDefine.h"

/**
*@brief 公会圆桌会议
*/
struct GuildTableDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)type; }

	bool Read(Avalon::DataRow& row);

public:
	// 类型(是否援助)
	UInt8	type;
	// 每日次数
	UInt32	maxTimes;
	// 奖励
	std::vector<ItemReward> rewards;
	// 邮件内容ID
	UInt32	mailContentId;
};

typedef Avalon::DataEntryMgr<GuildTableDataEntry> GuildTableDataEntryMgr;

#endif
