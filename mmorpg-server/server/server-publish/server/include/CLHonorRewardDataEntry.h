/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_HONORREWARD_DATA_ENTRY_H__
#define __CL_HONORREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

enum HonorRewardType
{
	HONOR_RT_RESULT = 1,	// 胜败奖励
	HONOR_RT_RANK = 2,	// 排名奖励
	HONOR_RT_KILL = 3,	// 击杀奖励
	HONOR_RT_WINNUM = 4,	// 胜场数
};

struct RankRewardCfg
{
	RankRewardCfg():minRank(0), maxRank(0), honor(0) {}

	UInt32 minRank;
	UInt32 maxRank;
	UInt32 honor;
};

// __CUSTOM_STRUCT_END__

class HonorRewardDataEntry : public Avalon::DataEntry
{
public:
	HonorRewardDataEntry();
	virtual ~HonorRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 序号
	UInt32                                          id;
	// pvp类型
	UInt32                                          pvpType;
	// 奖励类型
	std::string                                     rewardType;
	// 胜利奖励
	UInt32                                          victoryReward;
	// 失败奖励
	UInt32                                          lostReward;
	// 排名奖励
	std::string                                     rankReward;
	// 击杀奖励
	UInt32                                          killReward;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 奖励类型
	std::vector<UInt32> rewardTypeVec;
	// 排名奖励
	std::vector<RankRewardCfg> rankRewardVec;

// __CUSTOM_ENTRYFIELD_END__
};

class HonorRewardDataEntryMgr : public Avalon::DataEntryMgrBase<HonorRewardDataEntry>
	, public Avalon::Singleton<HonorRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(HonorRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const HonorRewardDataEntry* GetHonorRewardDataEntry(UInt32 pvpType);

private:

	// (key->pvpType)
	std::map<UInt32, HonorRewardDataEntry*> m_HonorRewardMap;

// __CUSTOM_MGR_END__
};

#endif
