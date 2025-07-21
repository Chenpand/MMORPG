/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_HEADFRAMECOMPENSATE_DATA_ENTRY_H__
#define __CL_HEADFRAMECOMPENSATE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

// 头像框补发类型
enum HFCompensateType
{
	HFCT_VIP = 1,	// vip
	HFCT_ACHIEVE = 2,	// 成就
};

// __CUSTOM_STRUCT_END__

class HeadFrameCompensateDataEntry : public Avalon::DataEntry
{
public:
	HeadFrameCompensateDataEntry();
	virtual ~HeadFrameCompensateDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 补发类型
	UInt32                                          type;
	// 补发需要的值
	UInt32                                          needVal;
	// 补发的头像框
	UInt32                                          headFrameId;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class HeadFrameCompensateDataEntryMgr : public Avalon::DataEntryMgrBase<HeadFrameCompensateDataEntry>
	, public Avalon::Singleton<HeadFrameCompensateDataEntryMgr>
{
public:
	virtual bool AddEntry(HeadFrameCompensateDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
