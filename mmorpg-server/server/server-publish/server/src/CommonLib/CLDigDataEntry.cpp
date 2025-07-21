#include <CLDigDataEntry.h>

bool DigMapDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	goldDigMinNum = row.ReadUInt32();
	goldDigMaxNum = row.ReadUInt32();

	std::string split = "|";
	std::string strDigRefreshHour = row.ReadString();
	std::vector<std::string> digRefreshHourVec;
	Avalon::StringUtil::Split(strDigRefreshHour, digRefreshHourVec, split);
	for (UInt32 i = 0; i < digRefreshHourVec.size(); ++i)
	{
		UInt32 hour = Avalon::StringUtil::ConvertToValue<UInt32>(digRefreshHourVec[i]);
		goldRefreshHours.push_back(hour);
	}

	silverDigMinNum = row.ReadUInt32();
	silverDigMaxNum = row.ReadUInt32();
	std::string strSilverDigRefreshHour = row.ReadString();
	std::vector<std::string> silverDigRefreshHourVec;
	Avalon::StringUtil::Split(strSilverDigRefreshHour, silverDigRefreshHourVec, split);
	for (UInt32 i = 0; i < silverDigRefreshHourVec.size(); ++i)
	{
		UInt32 hour = Avalon::StringUtil::ConvertToValue<UInt32>(silverDigRefreshHourVec[i]);
		silverRefreshHours.push_back(hour);
	}

	digMaxNum = row.ReadUInt32();

	return true;
}

bool DigMapDataEntry::IsGoldRefreshHour(UInt32 hour)
{
	std::vector<UInt32>::iterator iter = std::find(goldRefreshHours.begin(), goldRefreshHours.end(), hour);
	if (iter != goldRefreshHours.end())
	{
		return true;
	}
	return false;
}

bool DigMapDataEntry::IsSilverRefreshHour(UInt32 hour)
{
	std::vector<UInt32>::iterator iter = std::find(silverRefreshHours.begin(), silverRefreshHours.end(), hour);
	if (iter != silverRefreshHours.end())
	{
		return true;
	}
	return false;
}

bool DigMapDataEntryMgr::AddEntry(DigMapDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DigMapDataEntry>::AddEntry(entry)) return false;
	return true;
}

bool DigItemPoolDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	digType = (DigType)row.ReadUInt32();
	digItemType = (DigItemType)row.ReadUInt32();
	group = row.ReadUInt32();
	priority = row.ReadUInt32();
	itemId = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	boxWeight = row.ReadUInt32();
	digWeight = row.ReadUInt32();
	rewardId = row.ReadUInt32();

	singleDigWeight = digWeight / itemNum;
	return true;
}

bool DigItemPoolDataEntryMgr::AddEntry(DigItemPoolDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DigItemPoolDataEntry>::AddEntry(entry)) return false;

	DigItemPoolTypeMap::iterator iter = m_DigItemPoolType.find(entry->digType);
	if (iter != m_DigItemPoolType.end())
	{
		DigItemPoolGroupMap::iterator groupMapIter = iter->second.find(entry->group);
		if (groupMapIter != iter->second.end())
		{
			groupMapIter->second.weight += entry->boxWeight;
			groupMapIter->second.digItemPools.push_back(entry);
		}
		else
		{
			DigItemPoolGroup group;
			group.weight += entry->boxWeight;
			group.digItemPools.push_back(entry);

			iter->second.insert(std::make_pair(entry->group, group));
		}
	}
	else
	{
		DigItemPoolGroup group;
		group.weight += entry->boxWeight;
		group.digItemPools.push_back(entry);

		DigItemPoolGroupMap groupMap;
		groupMap.insert(std::make_pair(entry->group, group));

		m_DigItemPoolType.insert(std::make_pair(entry->digType, groupMap));
	}

	return true;
}

bool DigItemPoolDataEntryMgr::GetDigItemPoolType(DigType digType, DigItemPoolGroupMap& digItemPoolGroup)
{
	DigItemPoolTypeMap::iterator iter = m_DigItemPoolType.find(digType);
	if (iter != m_DigItemPoolType.end())
	{
		digItemPoolGroup = iter->second;
		return true;
	}
	return false;
}

bool DigItemRandomDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	group = row.ReadUInt32();
	itemId = row.ReadUInt32();
	weight = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	UInt32 tmp = row.ReadUInt32();
	isAnnounce = tmp == 1 ? true : false;

	return true;
}

bool DigItemRandomDataEntryMgr::AddEntry(DigItemRandomDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DigItemRandomDataEntry>::AddEntry(entry)) return false;

	std::map<UInt32, DigItemRandomGroup>::iterator iter = m_DigItemRandomGroups.find(entry->group);
	if (iter != m_DigItemRandomGroups.end())
	{
		iter->second.digItemRandoms.push_back(entry);
		iter->second.weight += entry->weight;
	}
	else
	{
		DigItemRandomGroup group;
		group.digItemRandoms.push_back(entry);
		group.weight += entry->weight;
		m_DigItemRandomGroups.insert(std::make_pair(entry->group, group));
	}
	return true;
}

bool DigItemRandomDataEntryMgr::GetRandomItem(UInt32 group, UInt32& itemId, UInt32& itemNum, bool& isAnn)
{
	std::map<UInt32, DigItemRandomGroup>::iterator iter = m_DigItemRandomGroups.find(group);
	if (iter != m_DigItemRandomGroups.end())
	{
		UInt32 random = Avalon::Random::RandBetween(1, iter->second.weight);
		for (UInt32 i = 0; i < iter->second.digItemRandoms.size(); ++i)
		{
			if (random > iter->second.digItemRandoms[i]->weight)
			{
				random = random - iter->second.digItemRandoms[i]->weight;
			}
			else
			{
				itemId = iter->second.digItemRandoms[i]->itemId;
				itemNum = iter->second.digItemRandoms[i]->itemNum;
				isAnn = iter->second.digItemRandoms[i]->isAnnounce;
				return true;
			}
		}
	}
	return false;
}
