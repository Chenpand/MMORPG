/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_DUNGEONCLEARTIME_DATA_ENTRY_H__
#define __CL_DUNGEONCLEARTIME_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class DungeonClearTimeDataEntry : public Avalon::DataEntry
{
public:
	DungeonClearTimeDataEntry();
	virtual ~DungeonClearTimeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 地下城ID
	UInt32                                          dungeonID;
	// 地下城层数
	UInt32                                          level;
	// 准入角色分
	UInt32                                          score;
	// 29级
	UInt32                                          level29;
	// 39级
	UInt32                                          level39;
	// 49级
	UInt32                                          level49;
	// 59级
	UInt32                                          level59;
	// 69级
	UInt32                                          level69;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class DungeonClearTimeDataEntryMgr : public Avalon::DataEntryMgrBase<DungeonClearTimeDataEntry>
	, public Avalon::Singleton<DungeonClearTimeDataEntryMgr>
{
public:
	virtual bool AddEntry(DungeonClearTimeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
