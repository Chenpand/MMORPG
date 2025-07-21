#include "Dungeon.h"

Dungeon::Dungeon()
{
    m_Num = 0;
    m_BestScore = 0;
}

Dungeon::~Dungeon()
{

}

SceneDungeonInfo Dungeon::GetSceneDungeonInfo()
{
    SceneDungeonInfo info;
    info.id = GetDataID();
    info.bestScore = GetBestScore();
    info.bestRecordTime = GetBestRecordTime();

    return info;
}

Dungeon* Dungeon::CreateDungeon(UInt32 dungeonId)
{
    DungeonDataEntry* dataEntry = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
    if (dataEntry == NULL) return NULL;

    Dungeon* dungeon = new Dungeon();
    dungeon->SetDataID(dungeonId);
    dungeon->SetDataEntry(dataEntry);

    return dungeon;
}