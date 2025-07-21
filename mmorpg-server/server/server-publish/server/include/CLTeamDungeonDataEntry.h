#ifndef _CL_TEAM_DUNGEON_DATAENTRY_H_
#define _CL_TEAM_DUNGEON_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum TeamDungeonEntryType
{
	// 关卡
	TEAM_DUNGEON_ELEM,
	// 菜单
	TEAM_DUNGEON_MENU,
	// 城镇怪物
	TEAM_CITY_MONSTER
};

//活动data
class TeamDungeonDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	// id
	UInt32					id;
	// 类型
	TeamDungeonEntryType	type;
	// 地下城ID
	UInt32					dungeonId;
	// 匹配类型
	UInt8					matchType;
	// 最小等级
	UInt16					minLevel;
	// 最少人数
	UInt8					minPlayerNum;
	// 最大人数
	UInt8					maxPlayerNum;
	// 是否自动解散
	bool                    autoKick;
};

class TeamDungeonDataEntryMgr : public Avalon::DataEntryMgrBase<TeamDungeonDataEntry>
	, public Avalon::Singleton<TeamDungeonDataEntryMgr>
{
public:
	TeamDungeonDataEntryMgr();

	virtual bool AddEntry(TeamDungeonDataEntry* entry);

	TeamDungeonDataEntry* FindEntryByDungeonID(UInt32 dungeonId);

	UInt32 GetCityMonsterTeamTargetID() const { return m_CityMonsterTeamTargetId; }
private:
	UInt32			m_CityMonsterTeamTargetId;
};

#endif
