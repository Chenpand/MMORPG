#include "CLDungeonAreaDataEntry.h"

DungeonAreaDataEntry::DungeonAreaDataEntry()
{

}

DungeonAreaDataEntry::~DungeonAreaDataEntry()
{

}

bool DungeonAreaDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    groupId = row.ReadUInt32();
    areaId = row.ReadUInt32();
    type = (DungeonAreaPointType)row.ReadUInt8();
    dataId = row.ReadUInt32();
    summonMonsterGroupId = row.ReadUInt32();
    level = row.ReadUInt16();

    return true;
}