#include "CLEqualItemDataEntry.h"

bool EqualItemDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	{
		auto str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (auto param : strs)
		{
			UInt32 itemId = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			if (!itemId)
			{
				return false;
			}

			equalItemIds.push_back(itemId);
		}
	}

	if (equalItemIds.empty())
	{
		return false;
	}

	return true;
}
bool EqualItemDataEntryMgr::AddEntry(EqualItemDataEntry * entry)
{
	if (!Avalon::DataEntryMgrBase<EqualItemDataEntry>::AddEntry(entry))
	{
		return false;
	}
	for (auto id : entry->equalItemIds)
	{
		if (itemIdByEqualItemID.count(id) > 0)
		{
			ErrorLogStream << "(itemid:"<<id <<"), "<<"bind Equal Item Reusable " << LogStream::eos;
			return false;
		}
		itemIdByEqualItemID[id] = entry->id;
	}
	return true;
}

UInt32 EqualItemDataEntryMgr::GetItemIdByEqualItemID(UInt32 id)
{
	if (itemIdByEqualItemID.count(id) == 0)
	{
		return 0;
	}
	return itemIdByEqualItemID[id];
}
