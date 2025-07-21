#ifndef __CL_BILLING_DEFINE_H__
#define __CL_BILLING_DEFINE_H__

#include "CLDefine.h"
#include "CLItemDefine.h"

struct ChargeGoods
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & desc & tags & money & vipScore & itemId & num & firstAddNum & unfirstAddNum & isFirstCharge & icon & remainDays & dailyLimitNum;
	}

	UInt8		id;
	std::string	desc;
	UInt32		tags;
	UInt16		money;
	UInt16		vipScore;
	UInt32		itemId;
	UInt16		num;
	UInt16		firstAddNum;
	UInt16		unfirstAddNum;
	UInt8		isFirstCharge;
	std::string	icon;
	UInt32		remainDays;
	UInt8		dailyLimitNum;

	// 排序用(不发送)
	UInt32		sort;
	// 物品子类型
	UInt32		itemSubType;

	bool operator<(const ChargeGoods& other)
	{
		if (sort != other.sort)
		{
			return sort < other.sort;
		}
		return id < other.id;
	}
};

// 人民币礼包
struct ChargePacket
{
	ChargePacket()
	: id(0), money(0), vipScore(0), startTime(0), endTime(0), limitDailyNum(0), limitTotalNum(0), sort(0)
	{

	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & oldPrice & money & vipScore & icon & startTime & endTime & limitDailyNum & limitTotalNum & rewards;
	}

	// id
	UInt8		id;
	// 名字
	std::string	name;
	// 原价
	UInt16		oldPrice;
	// 价格
	UInt16		money;
	// 购买后获得的vip积分
	UInt16		vipScore;
	// icon
	std::string	icon;
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

	// 排序用(不发送)
	UInt32		sort;

	bool operator<(const ChargePacket& other)
	{
		if (sort != other.sort)
		{
			return sort < other.sort;
		}
		return id < other.id;
	}
};


// 充值商城类型
enum ChargeMallType
{
	// 充值商品
	CHARGE_MALL_CHARGE,
	// 人民币礼包
	CHARGE_MALL_PACKET,
	// 理财
	CHARGE_MALL_MONEY_MANAGE,
	// 福利:每日充值
	CHARGE_DAY_CHARGE_WELFARE,
	// 冒险通行证王者版
	CHARGE_ADVENTURE_PASS_KING,
	// 礼遇特权卡
	CHARGE_GIGT_RIGHT_CARD,
	// 深渊特权卡
	CHARGE_DUNGEON_RIGHT_CARD,
};

#endif