/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/

#include "CLOverlondDeviceValueDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

OverlondDeviceValueDataEntry::OverlondDeviceValueDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

OverlondDeviceValueDataEntry::~OverlondDeviceValueDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 OverlondDeviceValueDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool OverlondDeviceValueDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	itemDataID = row.ReadUInt32();
	sharpenLv = row.ReadUInt32();
	weight = row.ReadUInt32();
// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool OverlondDeviceValueDataEntryMgr::AddEntry(OverlondDeviceValueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<OverlondDeviceValueDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	if (!entry)
	{
		return false;
	}

	OverlondProbabilityData tmpData;
	tmpData.sharpenLv = entry->sharpenLv;
	tmpData.weight = entry->weight;

	probabilityMap[entry->itemDataID].push_back(tmpData);
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt32 OverlondDeviceValueDataEntryMgr::RandSharpenLevel(UInt32 itemDataID, UInt8 lvl)
{
	auto iter = probabilityMap.find(itemDataID);
	if (iter == probabilityMap.end())
	{
		return 0;
	}

	Int32 totalWeight = 0;
	UInt8 maxLvl = 0;
	for (auto& data : iter->second)
	{
		totalWeight += data.weight;
		if (data.sharpenLv > maxLvl)
		{
			maxLvl = data.sharpenLv;
		}
	}

	if (lvl >= maxLvl)
	{
		return 0;
	}

	Int32 randomNum = Avalon::Random::RandBetween(1, totalWeight);
	for (auto& item : iter->second)
	{
		if (item.weight >= randomNum)
		{
			return item.sharpenLv;
		}
		else
		{
			randomNum -= item.weight;
		}
	}

	return 0;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

