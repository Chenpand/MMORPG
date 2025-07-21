/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_LOSTDUNGEONBATTLE_DATA_ENTRY_H__
#define __CL_LOSTDUNGEONBATTLE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class LostDungeonBattleDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonBattleDataEntry();
	virtual ~LostDungeonBattleDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__

	bool ParseTask(const std::string& taskStr, std::map<UInt32, UInt32>& taskMap);
	std::map<UInt32, UInt32>& GetTaskMap(UInt32 grade);

// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 战场包括普通任务
	std::string                                     commonTask;
	// 战场包括困难任务
	std::string                                     diffTask;
	// 玩家上限
	UInt32                                          maxPlayerNum;
	// 地图城镇
	UInt32                                          mapId;
	//通关离开门id
	UInt32											passExitDoorId;
// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 普通任务
	std::map<UInt32, UInt32>	commonTaskMap;

	// 困难任务
	std::map<UInt32, UInt32>	diffTaskMap;

// __CUSTOM_ENTRYFIELD_END__
};

class LostDungeonBattleDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonBattleDataEntry>
	, public Avalon::Singleton<LostDungeonBattleDataEntryMgr>
{
public:
	virtual bool AddEntry(LostDungeonBattleDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
