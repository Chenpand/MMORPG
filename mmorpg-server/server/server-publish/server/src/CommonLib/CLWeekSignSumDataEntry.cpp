/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLWeekSignSumDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

WeekSignSumDataEntry::WeekSignSumDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

WeekSignSumDataEntry::~WeekSignSumDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 WeekSignSumDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool WeekSignSumDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	opActType = row.ReadUInt32();
	weekSum = row.ReadUInt32();
	rewardId = row.ReadUInt32();
	rewardNum = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool WeekSignSumDataEntryMgr::AddEntry(WeekSignSumDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeekSignSumDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	WeekSumReward sumReward;
	sumReward.weekId = entry->weekSum;
	sumReward.rewardId = entry->rewardId;
	sumReward.rewardNum = entry->rewardNum;

	auto iter = m_WeekSumRewardMap.find(entry->opActType);
	if (iter == m_WeekSumRewardMap.end())
	{
		std::list<WeekSumReward> tempList;
		tempList.push_back(sumReward);
		m_WeekSumRewardMap[entry->opActType] = tempList;
	}
	else
	{
		iter->second.push_back(sumReward);
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const WeekSumReward* WeekSignSumDataEntryMgr::GetReward(UInt32 opActType, UInt32 weekId)
{
	auto iter = m_WeekSumRewardMap.find(opActType);
	if (iter == m_WeekSumRewardMap.end())
		return NULL;

	for (auto& it : iter->second)
	{
		if (it.weekId != weekId)
			continue;
		
		return &it;
	}

	return NULL;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

