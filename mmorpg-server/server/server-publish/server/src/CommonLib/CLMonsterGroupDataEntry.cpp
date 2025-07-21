#include "CLMonsterGroupDataEntry.h"

bool MonsterGroupDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    projID = row.ReadUInt32();
    groupID = row.ReadUInt32();
	groupProb = row.ReadUInt32();
    prob = row.ReadUInt32();
    lossyType = (MonsterGroupLossyType)row.ReadUInt8();
    lossyNum = row.ReadUInt8();

    std::string monsterListStr = row.ReadString();
    if (monsterListStr != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(monsterListStr, strs, "|");
        for (auto str : strs)
        {
            monsterList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
        }
    }

    return true;
}