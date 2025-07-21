/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_BEADCONVERTCOST_DATA_ENTRY_H__
#define __CL_BEADCONVERTCOST_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class BeadConvertCostDataEntry : public Avalon::DataEntry
{
public:
	BeadConvertCostDataEntry();
	virtual ~BeadConvertCostDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 序号
	UInt32                                          id;
	// 宝珠品质
	UInt32                                          colour;
	// 货币消耗
	UInt32                                          moneyCostId;
	// 货币消耗
	UInt32                                          moneyCostNum;
	// 消耗宝珠品质
	UInt32                                          beadCost;
	// 宝珠消耗数量
	UInt32                                          beadCostNum;
	// 生成的宝珠掉落组
	UInt32                                          dropId;
	// 生成的宝珠掉落组(绑定)
	UInt32                                          bindDropId;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class BeadConvertCostDataEntryMgr : public Avalon::DataEntryMgrBase<BeadConvertCostDataEntry>
	, public Avalon::Singleton<BeadConvertCostDataEntryMgr>
{
public:
	virtual bool AddEntry(BeadConvertCostDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
