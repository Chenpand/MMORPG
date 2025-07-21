#ifndef _CL_RED_PACKET_DATAENTRY_H_
#define _CL_RED_PACKET_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum RedPacketSubType
{
	// 自费
	RPST_BUY = 1,
	// 系统
	RPST_SYSTEM = 2,
};

/**
*@brief 红包表
*/
struct RedPacketDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return (UInt32)reason; }

	bool Read(Avalon::DataRow& row);

	bool IsValidNum(UInt8 num);

public:
	// 来源
	UInt16	reason;
	// 描述
	std::string desc;
	// 类型(RedPacketType)
	UInt8	type;
	// 子类型
	RedPacketSubType subType;
	// 总金额
	UInt32	totalMoney;
	// 可选个数
	std::vector<UInt8>	nums;
	// 单个红包最小金额
	UInt32	minMoney;
	// 单个红包最大金额
	UInt32	maxMoney;
	// 发送的货币ID
	UInt32 costMoneyId;
	// 收到的货币ID
	UInt32 getMoneyId;
	// 第三类型
	UInt8	thrType;
};

typedef Avalon::DataEntryMgr<RedPacketDataEntry> RedPacketDataEntryMgr;

#endif
