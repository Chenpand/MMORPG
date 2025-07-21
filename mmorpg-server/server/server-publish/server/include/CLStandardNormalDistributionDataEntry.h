/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_STANDARDNORMALDISTRIBUTION_DATA_ENTRY_H__
#define __CL_STANDARDNORMALDISTRIBUTION_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class StandardNormalDistributionDataEntry : public Avalon::DataEntry
{
public:
	StandardNormalDistributionDataEntry();
	virtual ~StandardNormalDistributionDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 函数输入x
	UInt32                                          inputValue;
	// 函数输出φ（x）(千分制)
	UInt32                                          outputValue;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class StandardNormalDistributionDataEntryMgr : public Avalon::DataEntryMgrBase<StandardNormalDistributionDataEntry>
	, public Avalon::Singleton<StandardNormalDistributionDataEntryMgr>
{
public:
	virtual bool AddEntry(StandardNormalDistributionDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	StandardNormalDistributionDataEntryMgr() :m_InputMax(0) {}

	UInt32 GetOutputValue(UInt32 inputValue);

private:
	UInt32 m_InputMax;
	HashMap<UInt32, UInt32> m_InputOutputMap;
// __CUSTOM_MGR_END__
};

#endif
