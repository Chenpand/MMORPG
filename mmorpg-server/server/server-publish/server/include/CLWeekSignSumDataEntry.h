/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_WEEKSIGNSUM_DATA_ENTRY_H__
#define __CL_WEEKSIGNSUM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct WeekSumReward
{
	WeekSumReward() : weekId(0), rewardId(0), rewardNum(0) {}

	// 累计周
	UInt32 weekId;
	// 奖励
	UInt32 rewardId;
	// 数量
	UInt32 rewardNum;
};

// __CUSTOM_STRUCT_END__

class WeekSignSumDataEntry : public Avalon::DataEntry
{
public:
	WeekSignSumDataEntry();
	virtual ~WeekSignSumDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 活动类型
	UInt32                                          opActType;
	// 累计周
	UInt32                                          weekSum;
	// 奖励物品
	UInt32                                          rewardId;
	// 奖励数量
	UInt32                                          rewardNum;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class WeekSignSumDataEntryMgr : public Avalon::DataEntryMgrBase<WeekSignSumDataEntry>
	, public Avalon::Singleton<WeekSignSumDataEntryMgr>
{
public:
	virtual bool AddEntry(WeekSignSumDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const WeekSumReward* GetReward(UInt32 opActType, UInt32 weekId);

private:

	// key->活动类型
	std::map<UInt32, std::list<WeekSumReward>> m_WeekSumRewardMap;

// __CUSTOM_MGR_END__
};

#endif
