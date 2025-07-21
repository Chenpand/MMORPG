#ifndef __CL_DUNGEON_AREA_DATA_ENTRY_H__
#define __CL_DUNGEON_AREA_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum DungeonAreaPointType
{
    DAPT_MONSTER,						// 怪物
    DAPT_MONSTER_GROUP,					// 怪物组
    DAPT_DESTRUCT,						// 可破坏物
	DAPT_ACTIVITY_MONSTER,				// 活动小怪
	DAPT_ACTIVITY_MONSTER_SMALL_BOSS,	// 活动小BOSS
	DAPT_ACTIVITY_MONSTER_BIG_BOSS,		// 活动大BOSS
	DAPT_NUM,
};

/**
*@brief 地下城区域数据项
*/
class DungeonAreaDataEntry : public Avalon::DataEntry
{
public:
    DungeonAreaDataEntry();
    ~DungeonAreaDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // id
    UInt32	                id;
    // 区域ID
    UInt32                  areaId;
    // 组ID
    UInt32                  groupId;
    // 刷怪类型
    DungeonAreaPointType    type;
    // 具体ID（ID含义根据type决定）
    UInt32                  dataId;
    // 可召唤出的怪物(对应MonsterGroup表中的ID)
    UInt32                  summonMonsterGroupId;
    // 怪物等级
    UInt16                  level;
};

typedef Avalon::DataEntryMgr<DungeonAreaDataEntry> DungeonAreaDataEntryMgr;
typedef Avalon::DataEntryVisitor<DungeonAreaDataEntry> DungeonAreaDataEntryVisitor;

#endif