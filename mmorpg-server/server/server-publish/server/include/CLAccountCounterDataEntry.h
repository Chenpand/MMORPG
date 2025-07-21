/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ACCOUNTCOUNTER_DATA_ENTRY_H__
#define __CL_ACCOUNTCOUNTER_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include "TimeUtil.h"
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class AccountCounterDataEntry : public Avalon::DataEntry
{
public:
	AccountCounterDataEntry();
	virtual ~AccountCounterDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 计数类型
	AccountCounterType                              accountCounterType;
	// 刷新类型
	UInt32                                          refreshType;
	// 刷新时间点
	std::string                                     refreshTimePoint;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 刷新日期
	TimeUtil::RefreshDate refreshDate;
// __CUSTOM_ENTRYFIELD_END__
};

class AccountCounterDataEntryMgr : public Avalon::DataEntryMgrBase<AccountCounterDataEntry>
	, public Avalon::Singleton<AccountCounterDataEntryMgr>
{
public:
	virtual bool AddEntry(AccountCounterDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
