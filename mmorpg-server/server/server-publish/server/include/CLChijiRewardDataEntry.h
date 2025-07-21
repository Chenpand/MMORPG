/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJIREWARD_DATA_ENTRY_H__
#define __CL_CHIJIREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ChijiRewardDataEntry : public Avalon::DataEntry
{
public:
	ChijiRewardDataEntry();
	virtual ~ChijiRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 最低名次
	UInt32                                          minRank;
	// 最高名次
	UInt32                                          maxRank;
	// 奖励宝箱ID
	UInt32                                          rewardBoxID;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ChijiRewardDataEntryMgr : public Avalon::DataEntryMgrBase<ChijiRewardDataEntry>
	, public Avalon::Singleton<ChijiRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(ChijiRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	const ChijiRewardDataEntry* GetChiJiRewardDataEntry(UInt32 rank);
// __CUSTOM_MGR_END__
};

#endif
