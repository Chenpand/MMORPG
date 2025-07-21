#ifndef _CL_GUILD_SKILL_DATAENTRY_H_
#define _CL_GUILD_SKILL_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 公会数据项
*/
struct GuildSkillDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return MakeKey(skillId, level); }

	bool Read(Avalon::DataRow& row);

	static UInt32 MakeKey(UInt16 skillId, UInt8 level) { return ((UInt32)skillId) * 100 + level; }

public:
	// 技能ID
	UInt16	skillId;
	// 技能等级
	UInt8	level;
	// 需要战争坊等级
	UInt8	needBattleLevel;
	// 升级消耗贡献
	UInt32	contriCost;
	// 升级消耗金币
	UInt32	goldCost;
};

typedef Avalon::DataEntryMgr<GuildSkillDataEntry> GuildSkillDataEntryMgr;

#endif
