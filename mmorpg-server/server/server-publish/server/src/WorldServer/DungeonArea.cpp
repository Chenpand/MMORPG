#include "DungeonArea.h"
#include "DropItemMgr.h"
#include <CLDungeonProtocol.h>

DungeonArea::DungeonArea()
{
    m_dungeonAreaId = 0;
}

DungeonArea::~DungeonArea()
{

}

void DungeonArea::AddMonster(DungeonMonster& monster)
{
	monster.areaId = m_dungeonAreaId;
    m_monsters.push_back(monster);
}

DungeonMonster* DungeonArea::FindMonster(UInt32 id)
{
    for (auto& monster : m_monsters)
    {
        if (monster.id == id)
        {
            return &monster;
        }
    }

    return NULL;
}

void DungeonArea::GetInfo(UInt64 roleId, CLProtocol::SceneDungeonArea& area)
{
    area.id = GetID();

    for (const auto& monster : m_monsters)
    {
        // 深渊怪不处理
        if (monster.isHell)
        {
            continue;
        }

        CLProtocol::SceneDungeonMonster dungeonMonster;
        dungeonMonster.id = monster.id;
        dungeonMonster.pointId = monster.pointId;
        dungeonMonster.typeId = monster.typeId;
        dungeonMonster.prefixes = monster.prefixes;
        dungeonMonster.summonerId = monster.summonerId;
        
        for (const auto& item : monster.dropItems)
        {
            if (item.roleId != roleId)
            {
                continue;
            }

            dungeonMonster.dropItems.push_back(item.GetSceneDungeonDropItem());
        }

        if (monster.type == MT_BOX)
        {
            area.destructs.push_back(dungeonMonster);
        }
        else
        {
            area.monsters.push_back(dungeonMonster);
        }
    }
}

PlayerDropItem* DungeonArea::FindDropItem(UInt32 itemId)
{
    for (auto& monster : m_monsters)
    {
        for (auto& dropItem : monster.dropItems)
        {
            if (dropItem.id == itemId)
            {
                return &dropItem;
            }
        }
    }

    return NULL;
}