#include "CLEquipChgConsDataEntry.h"

bool EquipChgConsDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	convertType = row.ReadUInt8();
	eqSubType = row.ReadUInt8();
	suitLevel = row.ReadUInt32();
	color1 = row.ReadUInt8();
	color2 = row.ReadUInt8();

	char split = 0;
	std::istringstream stream1(row.ReadString());
	while (stream1)
	{
		ItemReward item;
		item.DecodeString(stream1);
		if (item.id == 0) break;
		consumeItems.push_back(item);
		stream1 >> split;
	}
	
	{
		std::string  ss = row.ReadString();
		std::vector<std::string> ssv;
		Avalon::StringUtil::Split(ss, ssv, "_");
		if (ssv.size() == 3)
		{
			convertor.threType = Avalon::StringUtil::ConvertToValue<UInt32>(ssv[0]);
			convertor.suitLev = Avalon::StringUtil::ConvertToValue<UInt32>(ssv[1]);
			convertor.coumseNum = Avalon::StringUtil::ConvertToValue<UInt32>(ssv[2]);
		}
	}
	return true;
}

bool EquipChgConsDataEntryMgr::AddEntry(EquipChgConsDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EquipChgConsDataEntry>::AddEntry(entry))
	{
		return false;
	}
	EquipChgConsDataKey key(entry->convertType, entry->eqSubType, entry->suitLevel, entry->color1, entry->color2);
	auto it = equipequipMapSuitMap.find(key);
	if (it != equipequipMapSuitMap.end())
	{
		ErrorLogStream << "duplicate id1 = " << entry->id << ", id2 =  " << it->second->id << LogStream::eos;
		return false;
	}
	equipequipMapSuitMap[key] = entry;
	return true;
}

EquipChgConsDataEntry*  EquipChgConsDataEntryMgr::GetDataEntry(UInt8	convertType, UInt8	subtype, UInt32 level, UInt8 color1, UInt8 color2)
{
	EquipChgConsDataKey key(convertType, subtype, level, color1, color2);
	auto it = equipequipMapSuitMap.find(key);
	if (it == equipequipMapSuitMap.end())
	{
		return NULL;
	}
	return it->second;
}