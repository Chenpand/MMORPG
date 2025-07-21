#include "CLDungeonDailyDropDataEntry.h"

bool DungeonDailyDropDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	dailyNum = row.ReadUInt32();
	{
		auto str = row.ReadString();
		std::vector<std::string> itemStrs;
		Avalon::StringUtil::Split(str, itemStrs, "|");
		for (auto& itemStr : itemStrs)
		{
			std::vector<std::string> params;
			Avalon::StringUtil::Split(itemStr, params, ":");

			if (params.size() != 3)
			{
				return false;
			}

			UInt32 itemDataId = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
			UInt32 itemNum = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);

			if (itemDataId == 0 || itemNum == 0)
			{
				continue;
			}

			DropItemElem item;
			item.id = itemDataId;
			item.minNum = itemNum;
			item.maxNum = itemNum;
			item.itemType = (DropItemType)Avalon::StringUtil::ConvertToValue<UInt32>(params[2]);;

			items.push_back(item);
		}
	}
	

	return true;
}