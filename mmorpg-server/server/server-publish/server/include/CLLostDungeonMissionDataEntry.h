/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_LOSTDUNGEONMISSION_DATA_ENTRY_H__
#define __CL_LOSTDUNGEONMISSION_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

enum LostDungeonTaskType
{
	LDTT_TASK_KILL_FIX_ROLE = 0,	// 击杀指定角色
	LDTT_TASK_KILL_ROLE_NUM = 1,	// 击杀数量角色
	LDTT_TASK_ALIVE_TIME = 2,	// 生存时间
	LDTT_TASK_CLEAR_FIX_DUNGEON = 3,// 通关指定关卡
	LDTT_TASK_KILL_FIX_MONSTER = 4,	// 击杀指定怪物
	LDTT_TASK_COLLECT_ITEM = 5,	// 收集道具
	LDTT_TASK_CLEAR_DUNGEON = 6,	// 任意关卡
};

// __CUSTOM_STRUCT_END__

class LostDungeonMissionDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonMissionDataEntry();
	virtual ~LostDungeonMissionDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 任务类型
	UInt32                                          type;
	// 任务值
	std::string                                     value;
	// 接取人数上限(按战场)
	UInt32                                          acceptLimit;
	// 前置条件1
	UInt32                                          frontCond1;
	// 积分
	UInt32                                          score;
	// 任务难度
	UInt32                                          grade;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	std::map<UInt32, UInt32>						valMap;

// __CUSTOM_ENTRYFIELD_END__
};

class LostDungeonMissionDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonMissionDataEntry>
	, public Avalon::Singleton<LostDungeonMissionDataEntryMgr>
{
public:
	virtual bool AddEntry(LostDungeonMissionDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	std::map<UInt32, std::vector<LostDungeonMissionDataEntry*>> m_MissionGradeMap;

// __CUSTOM_MGR_END__
};

#endif
