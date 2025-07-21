#ifndef _CL_CHARGE_MALL_DATAENTRY_H_
#define _CL_CHARGE_MALL_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum ChargeMallTag
{
	// ��
	CHARGE_MALL_TAG_NONE = 0,
	// �Ƽ�
	CHARGE_MALL_TAG_RECOMMEND = 1,
	// ����
	CHARGE_MALL_TAG_WELFARE = 2,
};

//�data
class ChargeMallDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32	id;
	//���
	UInt32	sort;
	//����
	std::string	desc;
	//��ǩ
	UInt32	tags;
	//��ֵ���
	UInt32	chargeMoney;
	//vip����
	UInt32  vipScore;
	// ÿ����������
	UInt8	dailiyLimitNum;
	//����ID
	UInt32  itemId;
	//����
	UInt32  num;
	//�׳���������
	UInt32	firstChargeAddNum;
	//���׳���������
	UInt32	unfirstChargeAddNum;
	//Icon
	std::string icon;
};

typedef Avalon::DataEntryMgr<ChargeMallDataEntry> ChargeMallDataEntryMgr;

#endif
