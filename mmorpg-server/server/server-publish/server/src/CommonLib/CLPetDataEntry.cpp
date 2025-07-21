#include <CLPetDataEntry.h>


PetDataEntry::PetDataEntry()
{}

PetDataEntry::~PetDataEntry()
{}

bool PetDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		ErrorLogStream << "Column(" << row.GetColNum() << ") is not match config(" << GetColNum() << ")!" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	name = row.ReadString();
	type = (PetType)row.ReadUInt32();
	quality = (PetQuality)row.ReadUInt32();
	maxLevel = row.ReadUInt16();
	itemId = row.ReadUInt32();
	devourExp = row.ReadUInt32();
	maxLevelScore = row.ReadUInt32();

	return true;
}

PetDataEntryMgr::PetDataEntryMgr()
{}

PetDataEntryMgr::~PetDataEntryMgr()
{}

bool PetDataEntryMgr::AddEntry(PetDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<PetDataEntry>::AddEntry(entry)) return false;

	if (entry->itemId)
	{
		m_PetEggMap.insert(std::make_pair(entry->itemId, entry));
	}
	return true;
}

PetDataEntry* PetDataEntryMgr::GetPetByEgg(UInt32 itemId)
{
	PetEggMap::const_iterator iter = m_PetEggMap.find(itemId);
	if (iter != m_PetEggMap.end())
	{
		return iter->second;
	}
	return NULL;
}

PetLevelDataEntry::PetLevelDataEntry()
{}

PetLevelDataEntry::~PetLevelDataEntry()
{}

bool PetLevelDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt8();
	quality = (PetQuality)row.ReadUInt32();
	upLevelExp = row.ReadUInt32();
	fatigueExp = row.ReadUInt32();
	fatigueHunger = row.ReadUInt16();

	std::istringstream stream(row.ReadString());
	char split = 0;
	while (stream)
	{
		ItemReward item;
		stream >> item.id >> split >> item.num;
		if (item.id == 0) break;
		sellRewards.push_back(item);
		stream >> split;
	}

	return true;
}

PetLevelDataEntryMgr::PetLevelDataEntryMgr()
{}

PetLevelDataEntryMgr::~PetLevelDataEntryMgr()
{}

bool PetLevelDataEntryMgr::AddEntry(PetLevelDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<PetLevelDataEntry>::AddEntry(entry)) return false;

	UInt32 preKey = CL_MAKE_PET_LEVEL_KEY(entry->level - 1, entry->quality);
	UInt32 key = CL_MAKE_PET_LEVEL_KEY(entry->level, entry->quality);

	UInt32 preExp = 0;
	std::map<UInt32, UInt32>::iterator iter = m_SumExp.find(preKey);
	if (iter != m_SumExp.end())
	{
		preExp = iter->second;
	}

	m_SumExp[key] = entry->upLevelExp + preExp;
	return true;
}

UInt32 PetLevelDataEntryMgr::GetSumExp(UInt16 level, PetQuality quality)
{
	std::map<UInt32, UInt32>::iterator iter = m_SumExp.find(CL_MAKE_PET_LEVEL_KEY(level - 1, quality));
	if (iter != m_SumExp.end())
		return iter->second;
	return 0;
}

PetFeedDataEntry::PetFeedDataEntry()
{}

PetFeedDataEntry::~PetFeedDataEntry()
{}

bool PetFeedDataEntry::Read(Avalon::DataRow& row)
{
	//等级
	id = row.ReadUInt32();

	//喂食类型
	type = (PetFeedType)row.ReadUInt32();

	std::string split = "|";
	std::string split2 = "_";

	std::string strConsume = row.ReadString();
	std::vector<std::string> feedItemVec;
	Avalon::StringUtil::Split(strConsume, feedItemVec, split);
	for (UInt32 i = 0; i < feedItemVec.size(); ++i)
	{
		std::vector<std::string> strFeeds;
		Avalon::StringUtil::Split(feedItemVec[i], strFeeds, split2);
		if (strFeeds.size() < 3) continue;

		FeedItem feedItem;
		feedItem.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(strFeeds[0]);
		feedItem.itemNum = Avalon::StringUtil::ConvertToValue<UInt32>(strFeeds[1]);
		feedItem.value = Avalon::StringUtil::ConvertToValue<UInt32>(strFeeds[2]);
		feedItems.push_back(feedItem);
	}
	return true;
}

PetFeedDataEntryMgr::PetFeedDataEntryMgr()
{}

PetFeedDataEntryMgr::~PetFeedDataEntryMgr()
{}

bool PetFeedDataEntryMgr::AddEntry(PetFeedDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<PetFeedDataEntry>::AddEntry(entry)) return false;

	if (entry->feedItems.size() < 0) return false;

	m_FeedMap[entry->type] = entry->feedItems;

	return true;
}

bool PetFeedDataEntryMgr::GetFeedItem(PetFeedType type, UInt32 index, FeedItem& result)
{
	std::map<PetFeedType, std::vector<FeedItem>>::iterator iter = m_FeedMap.find(type);
	if (iter == m_FeedMap.end())
	{
		return false;
	}

	if (iter->second.size() >= index + 1)
	{
		result = iter->second[index];
	}
	else
	{
		result = iter->second[iter->second.size() - 1];
	}
	return true;
}
