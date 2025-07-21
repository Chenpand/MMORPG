/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ADVENTURETEAM_DATA_ENTRY_H__
#define __CL_ADVENTURETEAM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLDropItemDefine.h"
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class AdventureTeamDataEntry : public Avalon::DataEntry
{
public:
	AdventureTeamDataEntry();
	virtual ~AdventureTeamDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 GetColNum() { return 4; };

	// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 等级
	UInt32                                          id;
	// 升级经验(非累加)
	std::string                                     exp;
	// 通关经验加成百分比
	UInt32                                          clearDungeonExpAddition;
	// 升级经验来源
	std::string                                     expSource;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 升级经验
	UInt64 levelUpExp;
	// 升级经验来源
	std::vector<UInt32> levelUpExpSourceVec;
// __CUSTOM_ENTRYFIELD_END__
};

class AdventureTeamDataEntryMgr : public Avalon::DataEntryMgrBase<AdventureTeamDataEntry>
	, public Avalon::Singleton<AdventureTeamDataEntryMgr>
{
public:
	virtual bool AddEntry(AdventureTeamDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	AdventureTeamDataEntryMgr() :m_MaxLevel(0) {}

	UInt32 GetMaxLevel() const { return m_MaxLevel; }

private:
	UInt32 m_MaxLevel;
// __CUSTOM_MGR_END__
};

#endif
