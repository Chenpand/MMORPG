/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONTIME_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONTIME_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

// 公会地下城状态
enum GuildDungeonStatus
{
	GUILD_DUNGEON_END,			// 结束

	GUILD_DUNGEON_PREPARE,		// 准备
	GUILD_DUNGEON_START,		// 开始
	GUILD_DUNGEON_REWARD,		// 领奖

	GUILD_DUNGEON_MAX,
};

// key->(时间戳), val->(状态)
typedef std::map<UInt64, UInt32>  TimeStatusMap;

// __CUSTOM_STRUCT_END__

class GuildDungeonTimeDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonTimeDataEntry();
	virtual ~GuildDungeonTimeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 周
	UInt32                                          week;
	// 状态
	UInt32                                          status;
	// 时间
	std::string                                     time;
	// 是否启用
	UInt32                                          isOpen;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	UInt32											hour;
	UInt32											min;
	UInt32											sec;
	UInt32											timeInterval;

// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonTimeDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonTimeDataEntry>
	, public Avalon::Singleton<GuildDungeonTimeDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonTimeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取当前状态
	UInt32 GetCurDungeonStatus(const Avalon::Time &now, UInt64& stamp);

	// 获取当天的时间map
	TimeStatusMap* GetCurTimeStatusMap(UInt32 week);

	// 获取上次某时间，某状态的时间戳
	UInt64 GetLastStatusTimeStamp(UInt64 time, UInt32 status);

	// 获取上一次活动时间戳
	UInt32 GetLastBattleTim(Avalon::Time now);

private:

	typedef std::map<UInt32, TimeStatusMap> GuildDungeonTimeMap;

	// 公会地下城时间(key->周几)
	GuildDungeonTimeMap m_GuildDungeonTimeMap;

	// 公会地下城准备时间(key->周几)
	std::map<UInt32, GuildDungeonTimeDataEntry*> m_PrepareTimeMap;

// __CUSTOM_MGR_END__
};

#endif
