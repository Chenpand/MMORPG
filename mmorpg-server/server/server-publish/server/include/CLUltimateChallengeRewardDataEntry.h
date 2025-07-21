/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ULTIMATECHALLENGEREWARD_DATA_ENTRY_H__
#define __CL_ULTIMATECHALLENGEREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//自定义头文件	
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举	
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class UltimateChallengeRewardDataEntry : public Avalon::DataEntry
{
public:
	UltimateChallengeRewardDataEntry();
	virtual ~UltimateChallengeRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口	
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	//ID
	UInt32                                        id;
	//通关使用最少天数
	UInt32                                        minClearDays;
	//通关使用最多天数
	UInt32                                        maxClearDays;
	//奖励道具宝箱
	std::string                                   rewardItem;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	std::vector<ItemReward> rewards;
// __CUSTOM_ENTRYFIELD_END__
};

class UltimateChallengeRewardDataEntryMgr : public Avalon::DataEntryMgrBase<UltimateChallengeRewardDataEntry>
	, public Avalon::Singleton<UltimateChallengeRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(UltimateChallengeRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	const UltimateChallengeRewardDataEntry* GetReward(UInt32 days);
// __CUSTOM_MGR_END__
};

#endif
