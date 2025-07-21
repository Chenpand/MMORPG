#include "CLEquipChangeDataEntry.h"

bool EquipChangeDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");

		for (size_t i = 0; i < strs.size(); ++i)
		{
			jobIds.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i].c_str()));
		}
	}

	convertType = row.ReadUInt8();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");

		for (size_t i = 0; i < strs.size(); ++i)
		{
			suitEquips.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i].c_str()));
		}
	}

	suitLevel = row.ReadUInt32();
	return true;
}

bool EquipChangeDataEntryMgr::AddEntry(EquipChangeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquipChangeDataEntry>::AddEntry(entry))
	{
		return false;
	}
	for (auto equipId : entry->suitEquips)
	{
		auto it = equipMapSuit.find(equipId);
		if (it != equipMapSuit.end())
		{
			ErrorLogStream << "equip id is duplicate id = "  << equipId << ", suit id1 =  " << entry->id
				<< ", id2 = " << it->second << LogStream::eos;
			return false;
		}
		equipMapSuit[equipId] = entry->id;
	}
	return true;
}

UInt32  EquipChangeDataEntryMgr::GetSuitIdByEqId(UInt32 eqId)
{
	auto it = equipMapSuit.find(eqId);
	if (it == equipMapSuit.end())
	{
		return 0;
	}
	return it->second;
}