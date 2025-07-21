/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_EXPEDITIONMAP_DATA_ENTRY_H__
#define __CL_EXPEDITIONMAP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ExpeditionMapDataEntry : public Avalon::DataEntry
{
public:
	ExpeditionMapDataEntry();
	virtual ~ExpeditionMapDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	// 检查玩家等级
	bool CheckPlayerLevelLimit(UInt16 level);
	// 检查佣兵团等级
	bool CheckAdventureTeamLevelLimit(UInt16 level);
	// 检查远征成员合法
	bool CheckExpiditonMemberValid(UInt32 members);
	// 检查远征时间合法
	bool CheckExpeditionDurationValid(UInt32 duration);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 地图名字
	std::string                                     mapName;
	// 角色等级要求
	UInt32                                          playerLevelLimit;
	// 佣兵团等级要求
	UInt32                                          adventureTeamLevelLimit;
	// 支持角色数量
	UInt32                                          rolesCapacity;
	// 远征时间列表
	std::string                                     expeditionTime;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 远征时间列表
	std::vector<UInt32> expeditionDurationList;
// __CUSTOM_ENTRYFIELD_END__
};

class ExpeditionMapDataEntryMgr : public Avalon::DataEntryMgrBase<ExpeditionMapDataEntry>
	, public Avalon::Singleton<ExpeditionMapDataEntryMgr>
{
public:
	virtual bool AddEntry(ExpeditionMapDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
