#ifndef _CL_VIP_DATAENTRY_H_
#define _CL_VIP_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include "CLVipDefine.h"
#include "CLItemDefine.h"


struct ArtifactJarDidscountRandInfo
{
	ArtifactJarDidscountRandInfo()
	: value(0), prob(0), plan(0)
	{}

	// ֵ
	UInt32 value;
	// Ȩ��
	UInt32 prob;
	// ����
	UInt8  plan;
};

typedef std::vector<ArtifactJarDidscountRandInfo> DiscountRandInfoVec;

struct VipDataEntry : public Avalon::DataEntry
{
public:
	VipDataEntry();
	~VipDataEntry();

public:
	UInt32 GetKey() const{ return level; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 12; };

public:
	//vip �ȼ�
	UInt8	level;

	//���ID
	UInt8	giftId;

	//������Ҫ����
	UInt32	exp;

	//���
	std::vector<ItemReward> giftItems;

	//���ԭ��
	UInt32 giftPrePrice;

	//����ּ�
	UInt32 giftDiscountPrice;

	// ���������ۿ�����Ϣ
	DiscountRandInfoVec artifactJarDiscountRateInfo;

	// ���������ۿ۴�����Ϣ
	DiscountRandInfoVec artifactJarDiscountEffectTimesInfo;

	// ���������ۿ۴�����Ϣ2
	DiscountRandInfoVec artifactJarDiscountEffectTimesInfo2;
};

class VipDataEntryMgr : public Avalon::DataEntryMgrBase<VipDataEntry>, public Avalon::Singleton<VipDataEntryMgr>
{
public:
	VipDataEntryMgr();
	~VipDataEntryMgr();

	UInt32 GetMaxVipLevel();

public:
	bool AddEntry(VipDataEntry* entry);

	// ������������ۿ�
	bool RandArtifactJarDiscount(UInt32 vipLevel, UInt32& discountRate, UInt32& discountEffectTimes);

private:
	UInt32 m_MaxVipLevel;

};

/**
 *@brief vip��Ȩ������
 */
struct VipPrivilegeDataEntry : public Avalon::DataEntry
{
public:
	VipPrivilegeDataEntry();
	~VipPrivilegeDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32 id;
	//��Ȩ����
	VipPrivilegeType	privilegeType;
	//Ȩ��
	std::vector<UInt32> values;
};

class VipPrivilegeDataEntryMgr : public Avalon::DataEntryMgrBase<VipPrivilegeDataEntry>
	, public Avalon::Singleton<VipPrivilegeDataEntryMgr >
{
public:
	VipPrivilegeDataEntryMgr();
	~VipPrivilegeDataEntryMgr();

public:
	bool AddEntry(VipPrivilegeDataEntry* entry);

	UInt32 FindPrivilegeValue(VipPrivilegeType type, UInt32 level);

	UInt32 FindPrivilegeValueById(UInt32 id, UInt32 level);

private:
	std::map<VipPrivilegeType, VipPrivilegeDataEntry*> m_VipPrivilegeMap;

};



#endif
