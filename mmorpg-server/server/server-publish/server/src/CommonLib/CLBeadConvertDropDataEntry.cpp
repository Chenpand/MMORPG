/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLBeadConvertDropDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

BeadConvertDropDataEntry::BeadConvertDropDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

BeadConvertDropDataEntry::~BeadConvertDropDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 BeadConvertDropDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool BeadConvertDropDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	dropPackID = row.ReadUInt32();
	itemID = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	weight = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool BeadConvertDropDataEntryMgr::AddEntry(BeadConvertDropDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BeadConvertDropDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	auto iter = m_ConvertDropCfg.find(entry->dropPackID);
	if (iter != m_ConvertDropCfg.end())
	{
		iter->second.totalWeight += entry->weight;
		iter->second.dropMap[iter->second.totalWeight] = entry;
	}
	else
	{
		BeadConvertDropCfg cfg;
		cfg.totalWeight = entry->weight;
		cfg.dropMap[cfg.totalWeight] = entry;
		m_ConvertDropCfg[entry->dropPackID] = cfg;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

void BeadConvertDropDataEntryMgr::GetConvertReward(UInt32 dropId, UInt32& itemId, UInt32& itemNum)
{
	auto iter = m_ConvertDropCfg.find(dropId);
	if (iter == m_ConvertDropCfg.end())
	{
		return;
	}

	BeadConvertDropCfg& dropCfg = iter->second;

	UInt32 randNum = Avalon::Random::RandBetween(0, dropCfg.totalWeight);
	auto it = dropCfg.dropMap.lower_bound(randNum);
	if (it == dropCfg.dropMap.end())
	{
		return;
	}

	itemId = it->second->itemID;
	itemNum = it->second->itemNum;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

