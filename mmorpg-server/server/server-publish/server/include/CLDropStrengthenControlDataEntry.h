/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_DROPSTRENGTHENCONTROL_DATA_ENTRY_H__
#define __CL_DROPSTRENGTHENCONTROL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class DropStrengthenControlDataEntry : public Avalon::DataEntry
{
public:
	DropStrengthenControlDataEntry();
	virtual ~DropStrengthenControlDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 GenKey(UInt32 level, UInt32 color, UInt32 color2);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 装备LV
	UInt32                                          lv;
	// 颜色
	UInt32                                          color;
	// 颜色
	UInt32                                          color2;
	// 强化等级
	std::vector<UInt32>                             strengthen;
	// 不同强化等级权重
	std::vector<UInt32>                             weight;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class DropStrengthenControlDataEntryMgr : public Avalon::DataEntryMgrBase<DropStrengthenControlDataEntry>
	, public Avalon::Singleton<DropStrengthenControlDataEntryMgr>
{
public:
	virtual bool AddEntry(DropStrengthenControlDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	/**
	*@brief 随机物品的强化等级
	*/
	UInt8 RandomItemStrenth(UInt32 itemId);
// __CUSTOM_MGR_END__
};

#endif
