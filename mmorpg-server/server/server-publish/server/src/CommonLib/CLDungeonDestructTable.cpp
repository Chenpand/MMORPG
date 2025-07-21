#include "CLDungeonDestructTable.h"

bool DungeonDestructDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    std::string dropSetIdStr = row.ReadString();
    if (dropSetIdStr != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(dropSetIdStr, strs, "|");
        for (auto& str : strs)
        {
            UInt32 dropSetId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
            dropSetIds.push_back(dropSetId);
        }
    }

    return true;
}