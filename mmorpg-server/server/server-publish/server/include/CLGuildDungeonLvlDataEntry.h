/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDDUNGEONLVL_DATA_ENTRY_H__
#define __CL_GUILDDUNGEONLVL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

// 地下城难度
enum GuildDungeonLvl
{
	GUILD_DUNGEON_LVL_INVAILD,

	GUILD_DUNGEON_LOW = 1,	// 初级
	GUILD_DUNGEON_MID = 2,	// 中级
	GUILD_DUNGEON_HIGH = 3,	// 高级

	GUILD_DUNGEON_LVL_MAX,
};


struct GuildDungeonCfg
{
	// 地下城ID
	UInt32 dungeonId;
	// 地下次难度
	UInt32 dungeonLvl;
	// bossID
	UInt32 bossId;
	// boss血量
	UInt64 bossBlood;
	// 奖励类型
	UInt32 rewardType;
	// 地下城名称
	std::string dungeonName;
	// 掉落的buff
	UInt32 dropBuff;

	GuildDungeonCfg() : dungeonId(0), dungeonLvl(0), bossId(0), bossBlood(0), rewardType(0), dropBuff(0){}
};

typedef std::vector<GuildDungeonCfg*>	GuildDungeonCfgVec;

// key->地下城难度
typedef std::map<UInt32, GuildDungeonCfgVec> GuildDungeonCfgMap;

// __CUSTOM_STRUCT_END__

class GuildDungeonLvlDataEntry : public Avalon::DataEntry
{
public:
	GuildDungeonLvlDataEntry();
	virtual ~GuildDungeonLvlDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 地下城类型
	UInt32                                          dungeonType;
	// 地下次难度级别
	UInt32                                          dungeonLvl;
	// 地下城ID
	UInt32                                          dungeonId;
	// bossID
	UInt32                                          bossId;
	// boss血量
	std::string                                     bossBlood;
	// 对应奖励表类型
	UInt32                                          rewardType;
	// 地下城名称
	std::string                                     dungeonName;
	// 掉落buff
	UInt32                                          dropBuff;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildDungeonLvlDataEntryMgr : public Avalon::DataEntryMgrBase<GuildDungeonLvlDataEntry>
	, public Avalon::Singleton<GuildDungeonLvlDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildDungeonLvlDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 根据地下城类型获得列表
	bool GetDungeonCfgVec(UInt32 dungeonType, GuildDungeonCfgVec& dungeonVec);
	
	// 根据地下城类型和难度获得列表
	bool GetDungeonCfgVec(UInt32 dungeonType, UInt32 dungeonLvl, GuildDungeonCfgVec& dungeonVec);

	// 根据地下城类型获得终极关卡Id
	UInt32 GetFinalDungeonId(UInt32 dungeonType);

	// 根据地下城ID查找配置
	const GuildDungeonCfg* GetGuildDungeonCfg(UInt32 dungeonId);

	// 是否大boss关卡
	bool IsFinalBossGate(UInt32 dungeonId);

private:

	// key->地下城类型(根据公会等级，可获得地下城类型)
	std::map<UInt32, GuildDungeonCfgMap> m_GuildDungeonTypeCfgMap;

	// 地下城类型对应终极地下城id
	std::map<UInt32, UInt32> m_GuildDungeonTypeFinalGate;

	// key->地下城Id
	std::map<UInt32, GuildDungeonCfg*> m_GuildDungeonCfg;

// __CUSTOM_MGR_END__
};

#endif
