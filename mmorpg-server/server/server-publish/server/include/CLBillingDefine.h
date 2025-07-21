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

	// ������(������)
	UInt32		sort;
	// ��Ʒ������
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

// ��������
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
	// ����
	std::string	name;
	// ԭ��
	UInt16		oldPrice;
	// �۸�
	UInt16		money;
	// ������õ�vip����
	UInt16		vipScore;
	// icon
	std::string	icon;
	// ��ʼʱ��
	UInt32		startTime;
	// ����ʱ��
	UInt32		endTime;
	// ÿ���޹�����
	UInt16		limitDailyNum;
	// ���޹�����
	UInt16		limitTotalNum;
	// �������
	ItemRewardVec rewards;

	// ������(������)
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


// ��ֵ�̳�����
enum ChargeMallType
{
	// ��ֵ��Ʒ
	CHARGE_MALL_CHARGE,
	// ��������
	CHARGE_MALL_PACKET,
	// ���
	CHARGE_MALL_MONEY_MANAGE,
	// ����:ÿ�ճ�ֵ
	CHARGE_DAY_CHARGE_WELFARE,
	// ð��ͨ��֤���߰�
	CHARGE_ADVENTURE_PASS_KING,
	// ������Ȩ��
	CHARGE_GIGT_RIGHT_CARD,
	// ��Ԩ��Ȩ��
	CHARGE_DUNGEON_RIGHT_CARD,
};

#endif