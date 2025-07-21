#include "CLAdventWeeklyTaskAddRewardDataEntry.h"

AdventWeeklyTaskAddRewardDataEntry::AdventWeeklyTaskAddRewardDataEntry()
{
}

AdventWeeklyTaskAddRewardDataEntry::~AdventWeeklyTaskAddRewardDataEntry()
{

}


bool AdventWeeklyTaskAddRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	hard = row.ReadUInt8();
	weight = row.ReadUInt32();
	
	std::string ss = row.ReadString();
	std::vector<std::string> items;
	Avalon::StringUtil::Split(ss, items, "|");
	for (auto s : items)
	{
		std::vector<std::string> sv;
		Avalon::StringUtil::Split(s, sv, "_");
		
		if (sv.size() == 3)
		{
			AdventAwardItem awardItem;
			awardItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sv[2]);
			awardItem.item.id = Avalon::StringUtil::ConvertToValue<UInt32>(sv[0]);
			awardItem.item.num = Avalon::StringUtil::ConvertToValue<UInt32>(sv[1]);
			itemRewards.push_back(awardItem);
		}
	}

	return true;
}

bool AdventWeeklyTaskAddRewardDataEntryMgr::AddEntry(AdventWeeklyTaskAddRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventWeeklyTaskAddRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}

	UInt8 hard = entry->hard;
	auto iter = m_dataEntrys.find(hard);
	if (iter == m_dataEntrys.end())
	{
		AdventWeeklyTaskAddRewardDataEntryList entrylist;
		entrylist.push_back(entry);
		m_dataEntrys[hard] = entrylist;
	}
	else
	{
		m_dataEntrys[hard].push_back(entry);
	}
	return true;
}

bool AdventWeeklyTaskAddRewardDataEntryMgr::GetRewardByTaskHard(UInt8 taskHard, ItemReward& itemReward)
{
	auto iter = m_dataEntrys.find(taskHard);
	if (iter == m_dataEntrys.end())
	{
		return false;
	}

	UInt32 totleWeight = 0;
	for (auto entry : iter->second)
	{
		totleWeight += entry->weight;
	}

	AdventWeeklyTaskAddRewardDataEntry* selectEntry = NULL;
	UInt32 randWeight = Avalon::Random::RandBetween(1, totleWeight);
	for (auto entry : iter->second)
	{
		if (entry->weight >= randWeight)
		{
			selectEntry = entry;
			break;
		}
		else
		{
			randWeight -= entry->weight;
		}
	}
	if (!selectEntry)
	{
		return false;
	}
	
	UInt32 totleWt = 0;
	for (auto item : selectEntry->itemRewards)
	{
		totleWt += item.weight;
	}
	UInt32 randWt = Avalon::Random::RandBetween(1, totleWt);
	for (auto item : selectEntry->itemRewards)
	{
		if (item.weight >= randWt)
		{
			itemReward = item.item;
			return true;
		}
		else
		{
			randWt -= item.weight;
		}
	}
	return false;
}