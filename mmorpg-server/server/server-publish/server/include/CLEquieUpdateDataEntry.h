/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EQUIEUPDATE_DATA_ENTRY_H__
#define __CL_EQUIEUPDATE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class EquieUpdateDataEntry : public Avalon::DataEntry
{
public:
	EquieUpdateDataEntry();
	virtual ~EquieUpdateDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	//材料信息
	struct MatInfo
	{
		UInt32 matId;
		UInt32 num;
	};

	struct MatInfoLimit
	{
		UInt32 minStrenthLevel;
		UInt32 maxStrenthLevel;
		std::vector<MatInfo>	needMats;
	};
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 装备ID
	UInt32                                          equieID;
	// 小职业ID
	UInt32											jobID;
	// 升级装备ID
	UInt32                                          nextLvEquieID;
	// 消耗材料及数量
	std::string                                     materialConsume;
	// 消耗材料及数量（非普通装备）
	std::string                                     increaseMaterialConsume;
	// 升级公告ID
	UInt32                                          announceID;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	//需要材料
	std::vector<MatInfoLimit>	needMatLimits;
	//需要材料（非普通装备）
	std::vector<MatInfoLimit>	increaseNeedMatLimits;
// __CUSTOM_ENTRYFIELD_END__
};

class EquieUpdateDataEntryMgr : public Avalon::DataEntryMgrBase<EquieUpdateDataEntry>
	, public Avalon::Singleton<EquieUpdateDataEntryMgr>
{
public:
	virtual bool AddEntry(EquieUpdateDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
