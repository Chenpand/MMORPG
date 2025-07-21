#include "CLDungeonPrefixLimitDataEntry.h"

bool DungeonPrefixLimitDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    std::string levelStr = row.ReadString();
    if (levelStr != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(levelStr, strs, "|");
        if (strs.size() == 1)
        {
            minLevel = maxLevel = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
        }
        else if (strs.size() == 2)
        {
            minLevel = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
            maxLevel = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    hardType = row.ReadUInt8();
    prefixMonsterTotalNum = row.ReadUInt32();
    seniorPrefixNum = row.ReadUInt32();
    lowPrefixNum = row.ReadUInt32();
    return true;
}