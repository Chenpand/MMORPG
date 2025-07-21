#include "CLGiftBagDataEntry.h"

GiftBagDataEntry::GiftBagDataEntry()
{
}

GiftBagDataEntry::~GiftBagDataEntry()
{
}

bool GiftBagDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	std::istringstream stream(row.ReadString());
	char split = 0;
	while(stream)
	{
		ItemReward item;
		item.DecodeString(stream);
		if(item.id == 0) break;
		rewards.push_back(item);
		stream >> split;
	}
	param1 = row.ReadUInt32();
	param2 = row.ReadUInt32();
	param3 = row.ReadUInt32();
	return true;
}



GiftBagDataEntryMgr::GiftBagDataEntryMgr()
{
}

GiftBagDataEntryMgr::~GiftBagDataEntryMgr()
{
}

bool GiftBagDataEntryMgr::AddEntry(GiftBagDataEntry* dataEntry)
{
	if(!Avalon::DataEntryMgrBase<GiftBagDataEntry>::AddEntry(dataEntry)) return false;

	if(dataEntry->id > 100 && dataEntry->id < 120)
	{
		m_OnlineGiftbags.insert(std::make_pair(dataEntry->param1, dataEntry));
	}

	if(dataEntry->id > 120 && dataEntry->id < 130)
	{
		m_LevelGiftbags.insert(std::make_pair(dataEntry->param1, dataEntry));
	}

	return true;
}

const GiftBagDataEntry* GiftBagDataEntryMgr::GetOnlineGiftbag(UInt32 time) const
{
	GiftbagMap::const_iterator iter = m_OnlineGiftbags.find(time);
	if(iter != m_OnlineGiftbags.end())
	{
		return iter->second;
	}
	return NULL;
}

const GiftBagDataEntry* GiftBagDataEntryMgr::GetNextOnlineGiftbag(UInt32 bagTime, UInt32& nextBagTime, UInt32& leftTime) const
{
	if(m_OnlineGiftbags.empty()) return NULL;
	if(bagTime >= (--m_OnlineGiftbags.end())->first) return NULL;

	UInt32 preBagTime = 0;
	for(GiftbagMap::const_iterator iter = m_OnlineGiftbags.begin();
		iter != m_OnlineGiftbags.end(); ++iter)
	{
		if(iter->first > bagTime)
		{
			nextBagTime = iter->first;
			leftTime = nextBagTime - preBagTime;
			return iter->second;
		}
		preBagTime = iter->first;
	}
	return NULL;
}

const GiftBagDataEntry* GiftBagDataEntryMgr::GetLevelGiftBag(UInt16 level) const
{
	GiftbagMap::const_iterator iter = m_LevelGiftbags.find(level);
	if(iter != m_LevelGiftbags.end())
	{
		return iter->second;
	}
	return NULL;
}

const GiftBagDataEntry* GiftBagDataEntryMgr::GetNextLevelGiftbag(UInt16 level, UInt16& nextLevel) const
{
	if(m_LevelGiftbags.empty()) return NULL;
	if(level >= (--m_LevelGiftbags.end())->first) return NULL;

	for(GiftbagMap::const_iterator iter = m_LevelGiftbags.begin();
		iter != m_LevelGiftbags.end(); ++iter)
	{
		if(iter->first > level)
		{
			nextLevel = iter->first;
			return iter->second;
		}
	}
	return NULL;
}

