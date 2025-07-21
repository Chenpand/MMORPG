#ifndef _CL_CHARGE_MALL_DATAENTRY_H_
#define _CL_CHARGE_MALL_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum ChargeMallTag
{
	// 无
	CHARGE_MALL_TAG_NONE = 0,
	// 推荐
	CHARGE_MALL_TAG_RECOMMEND = 1,
	// 福利
	CHARGE_MALL_TAG_WELFARE = 2,
};

//活动data
class ChargeMallDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32	id;
	//序号
	UInt32	sort;
	//描述
	std::string	desc;
	//标签
	UInt32	tags;
	//充值金额
	UInt32	chargeMoney;
	//vip积分
	UInt32  vipScore;
	// 每日限制数量
	UInt8	dailiyLimitNum;
	//道具ID
	UInt32  itemId;
	//数量
	UInt32  num;
	//首充数量补偿
	UInt32	firstChargeAddNum;
	//非首充数量补偿
	UInt32	unfirstChargeAddNum;
	//Icon
	std::string icon;
};

typedef Avalon::DataEntryMgr<ChargeMallDataEntry> ChargeMallDataEntryMgr;

#endif
