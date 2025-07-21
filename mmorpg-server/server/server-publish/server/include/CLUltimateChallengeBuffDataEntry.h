/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ULTIMATECHALLENGEBUFF_DATA_ENTRY_H__
#define __CL_ULTIMATECHALLENGEBUFF_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class UltimateChallengeBuffDataEntry : public Avalon::DataEntry
{
public:
	UltimateChallengeBuffDataEntry();
	virtual ~UltimateChallengeBuffDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// buffID
	UInt32                                          buffID;
	// 添加对象
	UInt32                                          target;
	// 持续几层
	UInt32                                          sustain;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class UltimateChallengeBuffDataEntryMgr : public Avalon::DataEntryMgrBase<UltimateChallengeBuffDataEntry>
	, public Avalon::Singleton<UltimateChallengeBuffDataEntryMgr>
{
public:
	virtual bool AddEntry(UltimateChallengeBuffDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
