#ifndef _CL_CHARGE_PACKET_DATAENTRY_H_
#define _CL_CHARGE_PACKET_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//�data
class ChargePacketDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt8		id;
	// ����
	UInt32		sort;
	// ����
	std::string	name;
	// ԭ��
	UInt16		oldPrice;
	// �۸�
	UInt16		money;
	// ������õ�vip����
	UInt16		vipScore;
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
	// icon
	std::string	icon;
};

typedef Avalon::DataEntryMgr<ChargePacketDataEntry> ChargePacketDataEntryMgr;

#endif
