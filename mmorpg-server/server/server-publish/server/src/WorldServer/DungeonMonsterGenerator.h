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

// ˢ�ֵ�
struct DungeonAreaPoint
{
    UInt32                  id;
    DungeonAreaPointType    type;
    UInt32                  dataId;
    UInt32                  summonMonsterGroupId;
    UInt16                  level;
};
typedef std::vector<DungeonAreaPoint> VDungeonAreaPoint;

// ˢ����
struct DungeonAreaPointGroup
{
    UInt32                  id;
    VDungeonAreaPoint       points;
};
typedef std::vector<DungeonAreaPointGroup> VDungeonAreaPointGroup;

// ������Ϣ
struct DungeonAreaInfo
{
    UInt32                  id;
    VDungeonAreaPointGroup  groups;
};
typedef HashMap<UInt32, DungeonAreaInfo*> MapDungeonAreaInfo;

// ˢ�ֵ�Ԫ
struct MonsterGroupElem
{
    UInt32                  prob;
    UInt32                  lossyNum;
    std::vector<UInt32>     monsterList;
};

// ������
struct MonsterGroup
{
    UInt8                               id;
	UInt32								prob;
    std::vector<MonsterGroupElem>       elems;
};

// ˢ����
struct MonsterSet
{
    UInt32                              id;
    MonsterGroupLossyType               lossyType;
    std::vector<MonsterGroup>           groups;
};
typedef HashMap<UInt32, MonsterSet> MapMonsterSet;

// �������
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

// �������������Ļ�������Ҫ����ˢ�ֵ���Ϣ
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

    // �ؿ�����ȼ�
    UInt16                              adapterLevel;
	// �ؿ��ȼ�
	UInt16								dungeonLevel;

	// ���³Ǳ�
	DungeonDataEntry*					dungeonData;
    // �����Ѷ�
    DungeonHardType                     hardType;

    // ����������Ĺ����飬Key��ˢ�����ID��ValueΪ��������Ĺ�����
    HashMap<UInt32, MonsterGroup>       areaLossyMonsterGroup;

    // ȫ������Ĺ����飬Key��ˢ�����ID��ValueΪ��������Ĺ�����
    HashMap<UInt32, MonsterGroup>       globalLossyMonsterGroup;

    /*
        ��Ԩ���
    */

    // ��Ԩ�ֵȼ�
    UInt16                              hellMonsterLevel;
    // ��Ԩ�Ѷ�
    DungeonHellMode                     hellMode;
	
	// �������
	UInt32								playerNum;
	// ���ˢ����
	ActivityMonsterGroup				activityMonsterGroups[DAPT_NUM];
	// �������ɵĻ��,key�ǹ���ID��value�ǻID
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

    // ��ȡ��Ӧ����Ԩ����
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
