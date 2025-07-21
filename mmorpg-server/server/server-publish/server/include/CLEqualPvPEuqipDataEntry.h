/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EqualPvPEUQIP_DATA_ENTRY_H__
#define __CL_EqualPvPEUQIP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>



// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class EqualPvPEuqipDataEntry : public Avalon::DataEntry
{
public:
	EqualPvPEuqipDataEntry();
	virtual ~EqualPvPEuqipDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 序号
	UInt32                                          id;
	// 职业
	UInt32                                          occu;
	// 子类别
	UInt32                                          subType;
	// 装备道具ID
	UInt32                                          equipID;
	// 装备强化等级
	UInt32                                          strengthenLv;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class EqualPvPEuqipDataEntryMgr : public Avalon::DataEntryMgrBase<EqualPvPEuqipDataEntry>
	, public Avalon::Singleton<EqualPvPEuqipDataEntryMgr>
{
public:
	virtual bool AddEntry(EqualPvPEuqipDataEntry* entry);
// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
