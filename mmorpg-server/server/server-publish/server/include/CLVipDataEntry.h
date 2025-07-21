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

	// 值
	UInt32 value;
	// 权重
	UInt32 prob;
	// 方案
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
	//vip 等级
	UInt8	level;

	//礼包ID
	UInt8	giftId;

	//升级需要经验
	UInt32	exp;

	//礼包
	std::vector<ItemReward> giftItems;

	//礼包原价
	UInt32 giftPrePrice;

	//礼包现价
	UInt32 giftDiscountPrice;

	// 神器罐子折扣率信息
	DiscountRandInfoVec artifactJarDiscountRateInfo;

	// 神器罐子折扣次数信息
	DiscountRandInfoVec artifactJarDiscountEffectTimesInfo;

	// 神器罐子折扣次数信息2
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

	// 随机神器罐子折扣
	bool RandArtifactJarDiscount(UInt32 vipLevel, UInt32& discountRate, UInt32& discountEffectTimes);

private:
	UInt32 m_MaxVipLevel;

};

/**
 *@brief vip特权数据项
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
	//特权类型
	VipPrivilegeType	privilegeType;
	//权限
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
