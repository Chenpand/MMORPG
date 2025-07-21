/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EXPEDITIONMAPREWARD_DATA_ENTRY_H__
#define __CL_EXPEDITIONMAPREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLItemDefine.h"
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ExpeditionMapRewardDataEntry : public Avalon::DataEntry
{
public:
	ExpeditionMapRewardDataEntry();
	virtual ~ExpeditionMapRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	bool GetExpeditionRewards(const std::vector<UInt8> occus, ItemRewardVec& expeditonRewards);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 所属地图
	UInt32                                          expeditionMapId;
	// 奖励
	std::string                                     rewards;
	// 派遣任意N个职业
	UInt32                                          requireAnyOccu;
	// 派遣任意N个相同大职业
	UInt32                                          requireAnySameBaseOccu;
	// 派遣任意N个不同大职业
	UInt32                                          requireAnyDiffBaseOccu;
	// 派遣任意N个不同小职业
	UInt32                                          requireAnyDiffChangedOccu;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	ItemRewardVec rewardVec;
// __CUSTOM_ENTRYFIELD_END__
};

class ExpeditionMapRewardDataEntryMgr : public Avalon::DataEntryMgrBase<ExpeditionMapRewardDataEntry>
	, public Avalon::Singleton<ExpeditionMapRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(ExpeditionMapRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	void GetExpeditionRewards(UInt8 mapId, const std::vector<UInt8> occus, ItemRewardVec& expeditonRewards, std::vector<UInt32>& rewardsDataIds);
// __CUSTOM_MGR_END__
};

#endif
