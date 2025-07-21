#ifndef _CL_MALL_PERSONAL_TAILOR_DATA_ENTRY_H_
#define _CL_MALL_PERSONAL_TAILOR_DATA_ENTRY_H_


#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <CLDefine.h>


// 私人订制表数据
struct MallPersonalTailorDataEntry : public Avalon::DataEntry
{
public:
	MallPersonalTailorDataEntry();
	~MallPersonalTailorDataEntry();

public:
	UInt32 GetKey() const { return activateCond; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; };

public:
	// 触发条件
	UInt8				activateCond;

	// 触发子条件
	UInt8				activateSubCond;

	// 商城商品id
	std::vector<UInt32>	mallGoodsIds;

	// 触发子类型
	UInt8				activateSubType;

	// 玩家等级限制
	UInt16				playerLevelLimit;

	// 限时时长(小时)	
	UInt16				limitInterval;

	// 角色触发次数限制
	UInt16				roleActivateTimesLimit;
};


class MallPersonalTailorDataEntryMgr : public Avalon::DataEntryMgrBase<MallPersonalTailorDataEntry>
	, public Avalon::Singleton<MallPersonalTailorDataEntryMgr>
{
public:
	MallPersonalTailorDataEntryMgr();
	~MallPersonalTailorDataEntryMgr();

	bool AddEntry(MallPersonalTailorDataEntry* entry);
};

#endif