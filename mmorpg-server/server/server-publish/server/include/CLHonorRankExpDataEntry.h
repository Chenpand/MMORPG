/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_HONORRANKEXP_DATA_ENTRY_H__
#define __CL_HONORRANKEXP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class HonorRankExpDataEntry : public Avalon::DataEntry
{
public:
	HonorRankExpDataEntry();
	virtual ~HonorRankExpDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 序号
	UInt32                                          id;
	// 排名比例（万分比）
	UInt32                                          rank;
	// 奖励经验
	UInt32                                          rewardExp;
	// 排名级别
	UInt32                                          rankLvl;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class HonorRankExpDataEntryMgr : public Avalon::DataEntryMgrBase<HonorRankExpDataEntry>
	, public Avalon::Singleton<HonorRankExpDataEntryMgr>
{
public:
	virtual bool AddEntry(HonorRankExpDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

public:

	const HonorRankExpDataEntry* GetRankReward(UInt32 rank);

private:

	// 荣誉奖励经验表(key->排名，val->奖励经验)
	std::map<UInt32, HonorRankExpDataEntry*> m_RankRewardExpMap;

// __CUSTOM_MGR_END__
};

#endif
