#include "CLRetinueDataEntry.h"

RetinueDataEntry::RetinueDataEntry() {}

RetinueDataEntry::~RetinueDataEntry() {}

bool RetinueDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	groupId = row.ReadUInt32();
	name = row.ReadString();
	quality = (RetinueQuality)row.ReadUInt32();
	level = row.ReadUInt8();
	maxStar = row.ReadUInt8();
	itemId = row.ReadUInt32();

	std::string split = "|";
	std::string split2 = ",";
	std::string split3 = "_";

	std::string strConsumeNum = row.ReadString();
	std::vector<std::string> consumeNumVec;
	Avalon::StringUtil::Split(strConsumeNum, consumeNumVec, split);
	for (UInt32 i = 0; i < consumeNumVec.size(); ++i)
	{
		UInt32 tmpVal = Avalon::StringUtil::ConvertToValue<UInt32>(consumeNumVec[i]);
		consumeNum.push_back(tmpVal);
	}

	std::string strSkillConsume = row.ReadString();
	std::vector<std::string> skillConsumeVec;
	Avalon::StringUtil::Split(strSkillConsume, skillConsumeVec, split);
	for (UInt32 i = 0; i < skillConsumeVec.size(); ++i)
	{
		std::vector<std::string> skillConsume;
		Avalon::StringUtil::Split(skillConsumeVec[i], skillConsume, split2);
		std::vector<UnlockItem> unlockItems;
		for (UInt32 j = 0; j < skillConsume.size(); ++j)
		{
			std::vector<std::string> consumeItems;
			Avalon::StringUtil::Split(skillConsume[j], consumeItems, split3);
			if (consumeItems.size() < 2) continue;

			UnlockItem item;
			item.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(consumeItems[0]);
			item.num = Avalon::StringUtil::ConvertToValue<UInt32>(consumeItems[1]);
			unlockItems.push_back(item);
		}
		changeSkillConsume.push_back(unlockItems);
	}

	notLockSkillCount = row.ReadUInt32();
	changeSkillUnlock = row.ReadUInt16();

	return true;
}


bool RetinueDataEntry::GetUnlockItem(UnlockItem& unlockItem, UInt8 starLevel)
{
	if (starLevel >= consumeNum.size())
	{
		return false;
	}
	unlockItem.num = consumeNum[starLevel];
	unlockItem.itemId = itemId;
	return true;
}

bool RetinueDataEntry::GetChangeSkillConsume(std::vector<UnlockItem>& unlockItems, UInt32 lockSkillCount)
{
	UInt32 size = changeSkillConsume.size();
	if (size > lockSkillCount) size = lockSkillCount;
	for (UInt32 i = 0; i < changeSkillConsume[size].size(); ++i)
	{
		unlockItems.push_back(changeSkillConsume[size][i]);
	}
	return true;
}

RetinueDataEntryMgr::RetinueDataEntryMgr()
{
}

RetinueDataEntryMgr::~RetinueDataEntryMgr()
{
}

bool RetinueDataEntryMgr::AddEntry(RetinueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<RetinueDataEntry>::AddEntry(entry)) return false;

	if (entry->itemId != 0)
	{
		m_RetinueItemId.insert(std::make_pair(entry->itemId, entry));
	}

	return true;
}

RetinueDataEntry* RetinueDataEntryMgr::GetRetinueDataEntry(UInt32 itemId)
{
	RetinueItemIdMap::const_iterator iter = m_RetinueItemId.find(itemId);
	if (iter != m_RetinueItemId.end())
	{
		return iter->second;
	}
	return NULL;
}

RetinueLevelDataEntry::RetinueLevelDataEntry()
{
}

RetinueLevelDataEntry::~RetinueLevelDataEntry()
{
}

bool RetinueLevelDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt8();
	quality = (RetinueQuality)row.ReadUInt32();
	itemNum = row.ReadUInt32();

	std::istringstream stream(row.ReadString());
	char split = 0;
	while (stream)
	{
		UnlockItem item;
		stream >> item.itemId >> split >> item.num;
		if (item.itemId == 0) break;
		unlockItems.push_back(item);
		stream >> split;
	}

	playerLevel = row.ReadUInt16();

	return true;
}

bool RetinueSkillDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	groupId = row.ReadUInt32();
	groupType = RetinueGroupType(row.ReadUInt32());
	dataId = row.ReadUInt32();
	starLevel = row.ReadUInt8();

	std::string split = "|";
	std::string strOccu = row.ReadString();
	std::vector<std::string> occuVec;
	Avalon::StringUtil::Split(strOccu, occuVec, split);
	for (UInt32 i = 0; i < occuVec.size(); ++i)
	{
		if (occuVec[i].empty()) continue;
		Occupation val = (Occupation)Avalon::StringUtil::ConvertToValue<UInt32>(occuVec[i]);
		occus.push_back(val);
	}

	lockSkillCount = row.ReadUInt32();
	dataNum = row.ReadUInt32();
	weight = row.ReadUInt32();
	fix1 = row.ReadUInt32();
	fix2 = row.ReadUInt32();
	fix2Num = row.ReadUInt32();
	fixMax = row.ReadUInt32();
	fixMin = row.ReadUInt32();

	return true;
}

RetinueSkillDataEntryMgr::RetinueSkillDataEntryMgr() {}
RetinueSkillDataEntryMgr::~RetinueSkillDataEntryMgr() {}

bool RetinueSkillDataEntryMgr::AddEntry(RetinueSkillDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<RetinueSkillDataEntry>::AddEntry(entry)) return false;

	for (UInt32 i = 0; i < entry->occus.size(); ++i)
	{
		m_GroupOccuRetinueSkillMap[entry->groupId][entry->occus[i]].push_back(entry);
	}

	m_GroupRetinueSkillMap[entry->groupId].push_back(entry);


	return true;
}

const static RetinueSkillDataEntryMgr::RetinueSkillVec NullSkillDatas;

std::vector<RetinueSkillDataEntry*> RetinueSkillDataEntryMgr::GetRetinueSkills(UInt32 groupId, Occupation occu)
{
	GroupOccuRetinueSkillMap::iterator groupIter = m_GroupOccuRetinueSkillMap.find(groupId);
	if (groupIter != m_GroupOccuRetinueSkillMap.end())
	{
		OccuRetinueSkillMap::iterator occuIter = groupIter->second.find(occu);
		if (occuIter != groupIter->second.end())
		{
			return occuIter->second;
		}
	}
	return NullSkillDatas;
}

std::vector<RetinueSkillDataEntry*> RetinueSkillDataEntryMgr::GetRetinueSkills(UInt32 groupId)
{
	GroupRetinueSkillMap::iterator groupIter = m_GroupRetinueSkillMap.find(groupId);
	if (groupIter != m_GroupRetinueSkillMap.end())
	{
		return groupIter->second;
	}
	return NullSkillDatas;
}

bool RetinueSkillDataEntryMgr::CheckData()
{
	GroupRetinueSkillMap::iterator groupIter = m_GroupRetinueSkillMap.begin();
	
	while (groupIter != m_GroupRetinueSkillMap.end())
	{
		std::vector<UInt32> solutiones;
		std::map<UInt32, RetinueSkillPoint> retinueSkillMap;

		solutiones.push_back(groupIter->first);

		//Ω®Õº
		do
		{
			std::vector<UInt32> subSolutiones;
			for (auto groupId : solutiones)
			{
				RetinueSkillPoint& p = retinueSkillMap[groupId];

				std::vector<RetinueSkillDataEntry*> retinueSkillVec = GetRetinueSkills(groupId);

				for (auto retinueSkill : retinueSkillVec)
				{
					if (retinueSkill->groupType == RGT_SOLUTION)
					{
						if (p.subRetinueSkill.find(retinueSkill->dataId) != p.subRetinueSkill.end())
						{
							continue;
						}
						p.subRetinueSkill.insert(retinueSkill->dataId);

						if (retinueSkillMap.find(retinueSkill->dataId) == retinueSkillMap.end())
						{
							subSolutiones.push_back(retinueSkill->dataId);
						}

						retinueSkillMap[retinueSkill->dataId].degree++;
					}
				}
			}

			solutiones = subSolutiones;
		} while (!solutiones.empty());
		

		// Õÿ∆À≈≈–ÚÀ„∑®£¨ºÏ≤‚ «∑Ò”–ª∑
		UInt32 pNum = retinueSkillMap.size();

		while (true)
		{
			std::map<UInt32, RetinueSkillPoint>::iterator iter = retinueSkillMap.begin();

			int zeroDegreePoint = 0;
			while (iter != retinueSkillMap.end())
			{
				std::map<UInt32, RetinueSkillPoint>::iterator tmp = iter;
				++iter;

				RetinueSkillPoint& p = tmp->second;
				if (p.degree != 0) continue;

				for (auto subRetinueSkill : p.subRetinueSkill)
				{
					retinueSkillMap[subRetinueSkill].degree--;
				}
				retinueSkillMap.erase(tmp);
				zeroDegreePoint++;
			}
			pNum -= zeroDegreePoint;
			if (zeroDegreePoint == 0) break;
		}

		if (pNum != 0)
		{
			ErrorLogStream << "retinue skill(" << groupIter->first << ") has circular reference." << LogStream::eos;
			return false;
		}

		++groupIter;
	}

	return true;
}
