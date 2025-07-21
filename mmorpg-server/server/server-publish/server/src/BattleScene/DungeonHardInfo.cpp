#include "DungeonHardInfo.h"

DungeonHardInfo::DungeonHardInfo()
{
    m_dungeonId = 0;
    SetUnlockedHardType(LHT_NORMAL);
}

DungeonHardInfo::~DungeonHardInfo()
{

}

SceneDungeonHardInfo DungeonHardInfo::GetSceneDungeonHardInfo()
{
    SceneDungeonHardInfo info;
    info.id = GetDungeonID();
    info.unlockedHardType = GetUnlockedHardType();

    return info;
}

DungeonHardInfo* DungeonHardInfo::CreateDungeonHardInfo(UInt32 dungeonId)
{
    DungeonHardInfo* hard = new DungeonHardInfo();
    return hard;
}