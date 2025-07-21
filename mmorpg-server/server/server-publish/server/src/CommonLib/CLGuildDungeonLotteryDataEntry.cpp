/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonLotteryDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonLotteryDataEntry::GuildDungeonLotteryDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonLotteryDataEntry::~GuildDungeonLotteryDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonLotteryDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonLotteryDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	rewardGroup = row.ReadUInt32();
	itemID = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	weight = row.ReadUInt32();
	isHighVal = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonLotteryDataEntryMgr::AddEntry(GuildDungeonLotteryDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonLotteryDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	GuildDungeonReward rewardItem;
	rewardItem.itemId = entry->itemID;
	rewardItem.itemNum = entry->itemNum;
	rewardItem.isHighVal = entry->isHighVal;

	auto iter = m_LotteryCfgMap.find(entry->rewardGroup);
	if (iter == m_LotteryCfgMap.end())
	{
		GuildDungeonLotteryCfg lotteryCfg;
		lotteryCfg.totalWeight += entry->weight;
		lotteryCfg.lotteryCfgMap[lotteryCfg.totalWeight] = rewardItem;

		m_LotteryCfgMap[entry->rewardGroup] = lotteryCfg;
	}
	else
	{
		GuildDungeonLotteryCfg& lotteryCfg = iter->second;
		lotteryCfg.totalWeight += entry->weight;
		lotteryCfg.lotteryCfgMap[lotteryCfg.totalWeight] = rewardItem;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

GuildDungeonReward* GuildDungeonLotteryDataEntryMgr::GetGuildDungeonReward(UInt32 rewardGroup)
{
	auto iter = m_LotteryCfgMap.find(rewardGroup);
	if (iter == m_LotteryCfgMap.end())
		return NULL;

	UInt32 rand_num = Avalon::Random::RandBetween(0, iter->second.totalWeight);
	auto it = iter->second.lotteryCfgMap.lower_bound(rand_num);
	if (it == iter->second.lotteryCfgMap.end())
		return NULL;

	return &(it->second);
}

void GuildDungeonLotteryDataEntryMgr::GetGuildDungeonReward(UInt32 rewardGroup, UInt32 rewardNum, GuildDungeonRewardVec& rewardVec)
{
	auto iter = m_LotteryCfgMap.find(rewardGroup);
	if (iter == m_LotteryCfgMap.end())
		return;

	UInt32 rand_num = Avalon::Random::RandBetween(0, iter->second.totalWeight);
	auto it = iter->second.lotteryCfgMap.lower_bound(rand_num);
	if (it == iter->second.lotteryCfgMap.end())
		return;

	rewardVec.push_back(it->second);
	rewardNum--;

	GuildDungeonRewardVec tempVec;
	for (auto& tt : iter->second.lotteryCfgMap)
	{
		if (tt.second.itemId == it->second.itemId)
			continue;

		tempVec.push_back(tt.second);
	}

	std::random_shuffle(tempVec.begin(), tempVec.end());
	if (rewardNum > tempVec.size())
	{
		rewardNum = tempVec.size();
	}
	
	for (UInt32 k = 0;k < rewardNum;++k)
	{
		rewardVec.push_back(tempVec[k]);
	}
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

