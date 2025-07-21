#ifndef _CL_CHARGE_PACKET_DATAENTRY_H_
#define _CL_CHARGE_PACKET_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//活动data
class ChargePacketDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt8		id;
	// 排序
	UInt32		sort;
	// 名字
	std::string	name;
	// 原价
	UInt16		oldPrice;
	// 价格
	UInt16		money;
	// 购买后获得的vip积分
	UInt16		vipScore;
	// 开始时间
	UInt32		startTime;
	// 结束时间
	UInt32		endTime;
	// 每日限购次数
	UInt16		limitDailyNum;
	// 总限购次数
	UInt16		limitTotalNum;
	// 礼包内容
	ItemRewardVec rewards;
	// icon
	std::string	icon;
};

typedef Avalon::DataEntryMgr<ChargePacketDataEntry> ChargePacketDataEntryMgr;

#endif
