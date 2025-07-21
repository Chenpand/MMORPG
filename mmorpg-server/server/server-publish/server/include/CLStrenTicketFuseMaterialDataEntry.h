/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_STRENTICKETFUSEMATERIAL_DATA_ENTRY_H__
#define __CL_STRENTICKETFUSEMATERIAL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLItemDefine.h>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class StrenTicketFuseMaterialDataEntry : public Avalon::DataEntry
{
public:
	StrenTicketFuseMaterialDataEntry();
	virtual ~StrenTicketFuseMaterialDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 融合所需材料
	std::string                                     material;
	// 选择条件
	std::string                                     pickCondOfStrenLv;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 消耗材料
	ItemRewardVec costMaterials;
	// 等级下限
	UInt32 lowerStrenLevelRange;
	// 等级上限
	UInt32 upperStrenLevelRange;
// __CUSTOM_ENTRYFIELD_END__
};

class StrenTicketFuseMaterialDataEntryMgr : public Avalon::DataEntryMgrBase<StrenTicketFuseMaterialDataEntry>
	, public Avalon::Singleton<StrenTicketFuseMaterialDataEntryMgr>
{
public:
	virtual bool AddEntry(StrenTicketFuseMaterialDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	ItemRewardVec GetFuseMaterialsByStrenLevel(UInt32 strenLevel);
// __CUSTOM_MGR_END__
};

#endif
