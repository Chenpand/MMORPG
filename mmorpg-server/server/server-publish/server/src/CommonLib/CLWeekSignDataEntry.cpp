/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLWeekSignDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

WeekSignDataEntry::WeekSignDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

WeekSignDataEntry::~WeekSignDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 WeekSignDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool WeekSignDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	opActType = row.ReadUInt32();
	rewardId = row.ReadUInt32();
	rewardNum = row.ReadUInt32();
	rewardWeight = row.ReadUInt32();
	serverLimit = row.ReadUInt32();
	roleLimit = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				vipRate.push_back(param);
			}
		}
	}
	isBigReward = row.ReadUInt32();
	notifyId = row.ReadUInt32();
	superLink = row.ReadUInt32();
	refreshType = row.ReadUInt32();
	rewardUid = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool WeekSignDataEntryMgr::AddEntry(WeekSignDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeekSignDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	WeekSignItem* signItem = new WeekSignItem();
	signItem->rewardUid = entry->rewardUid;
	signItem->rewardId = entry->rewardId;
	signItem->rewardNum = entry->rewardNum;
	signItem->rewardWeight = entry->rewardWeight;
	signItem->serverLimit = entry->serverLimit;
	signItem->roleLimit = entry->roleLimit;
	signItem->isBigReward = entry->isBigReward;
	signItem->notifyId = entry->notifyId;
	signItem->superLink = entry->superLink;
	signItem->refreshType = entry->refreshType;

	signItem->itemEntry = ItemDataEntryMgr::Instance()->FindEntry(entry->rewardId);
	if (NULL == signItem->itemEntry)
	{
		ErrorLogStream << "Not Find Item Id:"<< entry->rewardId << LogStream::eos;
		return false;
	}

	for (auto& iter : entry->vipRate)
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(iter, strs, ",");
		if (strs.size() != 2)
		{
			ErrorLogStream << " Vip Format Error id:"<< entry->id << LogStream::eos;
			return false;
		}

		UInt32 vipLvl = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		UInt32 rate = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		signItem->vipRate[vipLvl] = rate;
	}

	m_ActWeightMap[entry->opActType] += entry->rewardWeight;

	auto iter = m_WeekSignCfgMap.find(entry->opActType);
	if (iter == m_WeekSignCfgMap.end())
	{
		WeekSignItemMap tempMap;
		tempMap[m_ActWeightMap[entry->opActType]] = signItem;
		m_WeekSignCfgMap[entry->opActType] = tempMap;
	}
	else
	{
		iter->second[m_ActWeightMap[entry->opActType]] = signItem;
	}

	// 必出垃圾
	if (signItem->roleLimit == 0 && signItem->serverLimit == 0)
	{
		auto itTrash = m_WeekSignTrashMap.find(entry->opActType);
		if (itTrash == m_WeekSignTrashMap.end())
		{
			WeekSignItemVec tempVec;
			tempVec.push_back(signItem);
			m_WeekSignTrashMap[entry->opActType] = tempVec;
		}
		else
		{
			itTrash->second.push_back(signItem);
		}
	}

	ActItem actItem(entry->opActType, entry->rewardUid);
	m_ItemCfg[actItem] = signItem;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const WeekSignItem* WeekSignDataEntryMgr::GetWeekSignItem(UInt32 actType, UInt32 vipLvl)
{
	auto itWeight = m_ActWeightMap.find(actType);
	if (itWeight == m_ActWeightMap.end())
		return NULL;

	auto iter = m_WeekSignCfgMap.find(actType);
	if (iter == m_WeekSignCfgMap.end())
		return NULL;

	UInt32 randNum = Avalon::Random::RandBetween(0, m_ActWeightMap[actType]);
	auto reward = iter->second.lower_bound(randNum);
	if (reward == iter->second.end())
		return NULL;

	auto tt = reward->second->vipRate .find(vipLvl);
	if (tt != reward->second->vipRate.end())
	{
		UInt32 vipRand = Avalon::Random::RandBetween(0, 10000);
		if (vipRand > tt->second)
		{
			return NULL;
		}
	}

	return reward->second;
}

const WeekSignItem* WeekSignDataEntryMgr::GetWeekSignTrashItem(UInt32 actType)
{
	auto iter = m_WeekSignTrashMap.find(actType);
	if (iter == m_WeekSignTrashMap.end())
		return NULL;

	if (iter->second.empty())
		return NULL;

	std::random_shuffle(iter->second.begin(), iter->second.end());

	return iter->second[0];
}

const WeekSignItem* WeekSignDataEntryMgr::FindWeekSignItem(UInt32 actType, UInt32 itemUid)
{
	ActItem actItem(actType, itemUid);
	auto iter = m_ItemCfg.find(actItem);
	if (iter == m_ItemCfg.end())
		return NULL;

	return iter->second;
}

void WeekSignDataEntryMgr::ReSortWeekSignCfg(UInt32 actType)
{
	m_ActWeightMap[actType] = 0;
	m_WeekSignCfgMap.clear();
	for (auto& iter : m_ItemCfg)
	{
		if (iter.second->isLimit)
		{
			continue;
		}
		
		m_ActWeightMap[actType] += iter.second->rewardWeight;

		auto it = m_WeekSignCfgMap.find(actType);
		if (it == m_WeekSignCfgMap.end())
		{
			WeekSignItemMap tempMap;
			tempMap[m_ActWeightMap[actType]] = iter.second;
			m_WeekSignCfgMap[actType] = tempMap;
		}
		else
		{
			it->second[m_ActWeightMap[actType]] = iter.second;
		}
	}
}

void WeekSignDataEntryMgr::SetLimitWeekSignItem(UInt32 actType, UInt32 itemUid, UInt32 num)
{
	ActItem actItem(actType, itemUid);
	auto iter = m_ItemCfg.find(actItem);
	if (iter == m_ItemCfg.end())
		return;

	if (num >= iter->second->serverLimit)
	{
		iter->second->isLimit = true;
		ReSortWeekSignCfg(actType);
	}
}

void WeekSignDataEntryMgr::ResetSortWeekSignCfg(UInt32 actType)
{
	for (auto& iter : m_ItemCfg)
	{
		iter.second->isLimit = false;
	}

	ReSortWeekSignCfg(actType);
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

