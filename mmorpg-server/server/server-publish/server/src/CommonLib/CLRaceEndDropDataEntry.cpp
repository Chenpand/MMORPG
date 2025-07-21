#include "CLRaceEndDropDataEntry.h"

RaceEndDropDataEntry::RaceEndDropDataEntry()
{

}

RaceEndDropDataEntry::~RaceEndDropDataEntry()
{

}

bool RaceEndDropDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	dungeonId = row.ReadUInt32();

	{
		std::string levelRangeStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(levelRangeStr, strs, "|");
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

	{
		std::string dropStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(dropStr, strs, "|");
		for (auto& str : strs)
		{
			RaceEndDrop drop;
			std::vector<std::string> dropSetStrs;
			Avalon::StringUtil::Split(str, dropSetStrs, ",");
			if (dropSetStrs.size() == 1)
			{
				drop.dropSetId = Avalon::StringUtil::ConvertToValue<UInt32>(dropSetStrs[0]);
				drop.num = 1;
			}
			else if (dropSetStrs.size() == 2)
			{
				drop.dropSetId = Avalon::StringUtil::ConvertToValue<UInt32>(dropSetStrs[0]);
				drop.num = Avalon::StringUtil::ConvertToValue<UInt32>(dropSetStrs[1]);
			}
			else
			{
				return false;
			}

			dropSets.push_back(drop);
		}
	}

	return true;
}