/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONTYPE_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONTYPE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class GuildDungeonTypeDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonTypeDataEntry();
	virtual ~GuildDungeonTypeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 建筑等级
	UInt32                                          buildLvl;
	// 地下城难度
	UInt32                                          dungeonType;
	//地下城难度名称
	std::string										dungeonTypeName;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonTypeDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonTypeDataEntry>
	, public Avalon::Singleton<GuildDungeonTypeDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonTypeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 根据公会建筑等级获取地下城类型
	UInt32 GetGuildDungeonType(UInt32 buildLvl);
	// 根据地下城难度获取地下城难度名称
	std::string GetGuildDungeonTypeName(UInt32 dungeonType);
	// 获取所有难度
	void GetDungeonType(std::vector<UInt32>& vec);

private:
	std::map<UInt32, UInt32> m_GuildTypeMap;
	std::map<UInt32, std::string> m_GuildTypeNameMap;
// __CUSTOM_MGR_END__
};

#endif
