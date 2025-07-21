#include "CLMonsterDataEntry.h"

MonsterDataEntry::MonsterDataEntry()
{

}

MonsterDataEntry::~MonsterDataEntry()
{

}

bool MonsterDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
	mode = row.ReadUInt8();
	type = (MonsterType)row.ReadUInt8();
    exp = row.ReadUInt32();

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
	
	dailyMaxDropNum = row.ReadUInt32();

    return true;
}