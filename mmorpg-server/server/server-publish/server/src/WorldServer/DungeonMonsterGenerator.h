#ifndef __DUNGEON_MONSTER_GENERATOR_H__
#define __DUNGEON_MONSTER_GENERATOR_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include <CLDungeonAreaDataEntry.h>
#include <CLMonsterGroupDataEntry.h>
#include <CLMonsterAttributeTable.h>
#include <CLMonsterPrefixDataEntry.h>
#include <CLDungeonHellDataEntry.h>
#include "DungeonArea.h"
#include <set>

// 刷怪点
struct DungeonAreaPoint
{
    UInt32                  id;
    DungeonAreaPointType    type;
    UInt32                  dataId;
    UInt32                  summonMonsterGroupId;
    UInt16                  level;
};
typedef std::vector<DungeonAreaPoint> VDungeonAreaPoint;

// 刷怪组
struct DungeonAreaPointGroup
{
    UInt32                  id;
    VDungeonAreaPoint       points;
};
typedef std::vector<DungeonAreaPointGroup> VDungeonAreaPointGroup;

// 区域信息
struct DungeonAreaInfo
{
    UInt32                  id;
    VDungeonAreaPointGroup  groups;
};
typedef HashMap<UInt32, DungeonAreaInfo*> MapDungeonAreaInfo;

// 刷怪单元
struct MonsterGroupElem
{
    UInt32                  prob;
    UInt32                  lossyNum;
    std::vector<UInt32>     monsterList;
};

// 怪物组
struct MonsterGroup
{
    UInt8                               id;
	UInt32								prob;
    std::vector<MonsterGroupElem>       elems;
};

// 刷怪组
struct MonsterSet
{
    UInt32                              id;
    MonsterGroupLossyType               lossyType;
    std::vector<MonsterGroup>           groups;
};
typedef HashMap<UInt32, MonsterSet> MapMonsterSet;

// 活动怪物组
struct ActivityMonsterGroup
{
	ActivityMonsterGroup()
	{
		activityId = 0;
		groupId = 0;
		prob = 0;
		remainNum = 0;
	}

	UInt32 activityId;
	UInt32 groupId;
	UInt32 prob;
	UInt32 remainNum;
};

// 怪物生成起器的环境，主要保存刷怪的信息
struct DungeonMonsterGenerateContext
{
    DungeonMonsterGenerateContext()
    {
        monsterIdSeed = 0;
        adapterLevel = 0;
		dungeonLevel = 0;
        hardType = LHT_NORMAL;
        hellMode = HELL_MODE_NULL;
        hellMonsterLevel = 0;
		playerNum = 1;
		memset(activityMonsterGroups, 0, sizeof(activityMonsterGroups));
		dungeonData = NULL;
    }

    UInt32 GenMonsterID() { return ++monsterIdSeed; }

    MonsterGroup* FindMonsterGroup(UInt32 id)
    {
        auto itr = areaLossyMonsterGroup.find(id);
        if (itr != areaLossyMonsterGroup.end())
        {
            return &itr->second;
        }

        itr = globalLossyMonsterGroup.find(id);
        if (itr != globalLossyMonsterGroup.end())
        {
            return &itr->second;
        }

        return NULL;
    }

    MonsterGroup* AddMonsterGroup(MonsterGroupLossyType type, UInt32 id, MonsterGroup group)
    {
        if (type == MGLT_AREA)
        {
            areaLossyMonsterGroup[id] = group;
        }
        else if (type == MGLT_GLOBAL)
        {
            globalLossyMonsterGroup[id] = group;
        }

        return FindMonsterGroup(id);
    }

    UInt32                              monsterIdSeed;

    // 关卡适配等级
    UInt16                              adapterLevel;
	// 关卡等级
	UInt16								dungeonLevel;

	// 地下城表
	DungeonDataEntry*					dungeonData;
    // 副本难度
    DungeonHardType                     hardType;

    // 房间内有损的怪物组，Key是刷怪组的ID，Value为随机出来的怪物组
    HashMap<UInt32, MonsterGroup>       areaLossyMonsterGroup;

    // 全局有损的怪物组，Key是刷怪组的ID，Value为随机出来的怪物组
    HashMap<UInt32, MonsterGroup>       globalLossyMonsterGroup;

    /*
        深渊相关
    */

    // 深渊怪等级
    UInt16                              hellMonsterLevel;
    // 深渊难度
    DungeonHellMode                     hellMode;
	
	// 玩家数量
	UInt32								playerNum;
	// 活动怪刷怪组
	ActivityMonsterGroup				activityMonsterGroups[DAPT_NUM];
	// 所有生成的活动怪,key是怪物ID，value是活动ID
	std::map<UInt32, UInt32>			activityMonsters;
};

class DungeonMonsterGenerator : public Avalon::Singleton<DungeonMonsterGenerator>
{
public:
    DungeonMonsterGenerator();
    ~DungeonMonsterGenerator();

public:
    bool Init();

    void GenMonstersByArea(DungeonMonsterGenerateContext& context, UInt32 areaId, UInt16 adapterLevel, VDungeonMonster& monsters);
    bool GenHellMonsters(DungeonMonsterGenerateContext& context, VDungeonMonster& monsters);
    std::vector<UInt8> GenMonsterPrefix(UInt32 lowPrefixNum, UInt32 seniorPrefixNum);

private:
    bool _InitMonsterSet();
    bool _InitMonsterPrefixSet();

    DungeonAreaInfo* FindDungeonAreaInfo(UInt32 areaId);
    bool GenMonstersFromSet(UInt32 summonerId, DungeonMonsterGenerateContext& context, UInt32 pointId, UInt16 pointLevel, UInt32 id, VDungeonMonster& monsters);
    UInt16 GenMonsterLevel(DungeonMonsterGenerateContext& context, UInt16 pointLevel);
    UInt32 GenMonsterExp(DungeonMonsterGenerateContext& context, DungeonMonster& monster);
    UInt32 _Rand();

    MonsterAttributeDataEntry* _GetMonsterAttributeDataEntry(DungeonHardType hardType, MonsterType type, UInt16 level);
    std::vector<UInt8> _GenMonsterPrefix(MonsterPrefixType type, UInt32 num);
    UInt32 _EncodeMonsterTypeID(UInt32 id, UInt16 level);
    bool _GenMonsterData(DungeonMonsterGenerateContext& context, DungeonMonster& monster, UInt32 monsterSetId, UInt32 summonerId, UInt32 monsterId, UInt32 pointId, UInt16 level);

    // 获取对应的深渊数据
    DungeonHellDataEntry* _GenHellModeData(DungeonMonsterGenerateContext& context);

public:
	void GenHellMonstersForGm(UInt32	dungeonId, UInt32 times);

private:
    MapDungeonAreaInfo                              m_areas;
    MapMonsterSet                                   m_monsterSet;

    typedef std::vector<UInt8> MonsterPrefixList;
    HashMap<UInt8, MonsterPrefixList>               m_monsterPrefixSet;
};

#endif
