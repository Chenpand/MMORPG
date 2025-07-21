#ifndef __DUNGEON_AREA_H__
#define __DUNGEON_AREA_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLDropItemDefine.h>
#include <CLMonsterDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLMonsterAttributeTable.h>

#include "DungeonPlayer.h"

struct DungeonMonster
{
    DungeonMonster()
    {
        id = 0;
		areaId = 0;
        typeId = 0;
        isDead = false;
        attrDataEntry = NULL;
        summonerId = 0;
        level = 1;
		mode = 0;
        exp = 0;
        type = MT_MONSTER;
        willSummonMonster = false;
        isHell = false;
        hellWave = 0;
		activityMonsterId = 0;
		dailyMaxDropNum = 0;
		monsterId = 0;
    }

    UInt32 GetExp(DungeonPlayer* dungeonPlayer) const
    {
		// 助战情况下杀怪经验为0
		if (dungeonPlayer && dungeonPlayer->IsAssist())
		{
			return 0;
		}

        int expAddition = 0;
        if (!prefixes.empty())
        {
            // 词缀怪经验有加成
            expAddition += 10;
        }

		if (dungeonPlayer)
        {
			//expAddition += dungeonPlayer->GetExpAddition();
			expAddition += dungeonPlayer->GetDungenAdditionByType(DART_EXP_RATE);
        }

        return exp * (100 + expAddition) / 100;
    }

    UInt32 GetMonsterBaseTypeID()
    {
        return typeId / 10000 * 10000 + typeId % 100;
    }

	UInt32 GetMonsterNormalHardTypeID()
	{
		return GetMonsterBaseTypeID() / 10 * 10 + 1;
	}

    CLProtocol::SceneDungeonMonster GetInfo(ObjID_t roleId)
    {
        CLProtocol::SceneDungeonMonster dungeonMonster;
        dungeonMonster.id = id;
        dungeonMonster.pointId = pointId;
        dungeonMonster.typeId = typeId;
        dungeonMonster.prefixes = prefixes;
        dungeonMonster.summonerId = summonerId;

        for (const auto& item : dropItems)
        {
            if (item.roleId != roleId)
            {
                continue;
            }

            dungeonMonster.dropItems.push_back(item.GetSceneDungeonDropItem());
        }

        return dungeonMonster;
    }

    // 唯一ID
    UInt32                          id;
	// AreaId
	UInt32							areaId;
    // 位置ID
    UInt32                          pointId;
    // 类型ID
    UInt32                          typeId;
    // 怪物类型
    MonsterType                     type;
	// 怪物模式
	UInt8							mode;
    // 属性数据
    MonsterAttributeDataEntry*      attrDataEntry;
    // 是否已经死亡
    bool                            isDead;
    // 掉落
    VPlayerDropItem                 dropItems;
    // 词缀
    std::vector<UInt8>              prefixes;
    // 召唤者ID（如果是被召唤出来的话）
    UInt32                          summonerId;
    // 是否会召唤出怪物
    bool                            willSummonMonster;
    // 怪物等级
    UInt16                          level;
    // 怪物基础经验
    UInt32                          exp;
    // 掉落列表
    std::vector<UInt32>             dropSet;
	// 如果是活动怪物，活动ID
	UInt32							activityMonsterId;
	// 每天最多产生掉落次数（0代表无限）
	UInt32							dailyMaxDropNum;


    /*
        深渊相关
    */
    // 是不是深渊怪
    bool                            isHell;
    // 深渊波次（第一波怪，第二波怪...）
    UInt8                           hellWave;

	UInt32							monsterId;
};
typedef std::vector<DungeonMonster> VDungeonMonster;

class DungeonArea
{
    AVALON_DEFINE_OBJECTPOOL(DungeonArea, Avalon::NullMutex)
public:
    DungeonArea();
    ~DungeonArea();

    inline void SetID(UInt32 id) { m_dungeonAreaId = id; }
    inline UInt32 GetID() const { return m_dungeonAreaId; }

public:
    void AddMonster(DungeonMonster& monster);
    DungeonMonster* FindMonster(UInt32 id);

    VDungeonMonster& GetAllMonster() { return m_monsters; }

    void GetInfo(UInt64 roleId, CLProtocol::SceneDungeonArea& area);

    PlayerDropItem* FindDropItem(UInt32 itemId);

private:
    UInt32                  m_dungeonAreaId;        // 区域ID
    VDungeonMonster         m_monsters;             // 怪物列表
    std::vector<UInt64>     m_enteredPlayers;       // 进入过这个区域的玩家

};
typedef std::vector<DungeonArea*> VDungeonArea;

#endif

