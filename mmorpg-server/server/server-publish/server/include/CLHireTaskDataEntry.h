/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_HIRETASK_DATA_ENTRY_H__
#define __CL_HIRETASK_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include <CLItemDataEntry.h>

// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

// __CUSTOM_STRUCT_END__

class HireTaskDataEntry : public Avalon::DataEntry
{
public:
	HireTaskDataEntry();
	virtual ~HireTaskDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// id
	UInt32                                          id;
	// 任务类型
	UInt32                                          type;
	// 完成计数
	UInt32                                          fullCnt;
	// 奖励
	ItemRewardVec									rewards;
	// 刷新周期
	UInt32											refreshType;
	// 适用身份
	UInt32											identity;
	// 额外参数
	std::vector<UInt32>									param;
            

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class HireTaskDataEntryMgr : public Avalon::DataEntryMgrBase<HireTaskDataEntry>
	, public Avalon::Singleton<HireTaskDataEntryMgr>
{
public:
	virtual bool AddEntry(HireTaskDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__



// __CUSTOM_MGR_END__
};

#endif
