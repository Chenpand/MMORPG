#include "CLDungeonDataEntry.h"

DungeonDataEntry::DungeonDataEntry()
{

}

DungeonDataEntry::~DungeonDataEntry()
{

}

bool DungeonDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
	name = row.ReadString();
    level = row.ReadUInt16();
    type = (DungeonType)row.ReadUInt32();
	subType = (DungeonSubType)row.ReadUInt32();
	threeType = (DungeonThreeType)row.ReadInt32();
    hardType = (DungeonHardType)row.ReadUInt32();
	tag = row.ReadUInt32();

	if (tag >= 32)
	{
		ErrorLogStream << "dungeon(" << id << ") invalid tag(" << tag << ") max tag(32)." << LogStream::eos;
		return false;
	}

	monsterLevelAdaptType = (DungeonMonsterLevelAdaptType)row.ReadUInt32();
	maxHardAdaptLevel = row.ReadUInt32();
    minLevel = row.ReadUInt32();
    storyTaskId = row.ReadUInt32();
    preTaskId = row.ReadUInt32();
    
    std::string dungeonsStr = row.ReadString();
    if (dungeonsStr != "-")
    {
        std::vector<std::string> dungeonIdStrs;
        Avalon::StringUtil::Split(dungeonsStr, dungeonIdStrs, "|");
        for (auto dungeonIdStr : dungeonIdStrs)
        {
            UInt32 prevDungeonId = Avalon::StringUtil::ConvertToValue<UInt32>(dungeonIdStr);
            storyDungeonIds.push_back(prevDungeonId);
        }
    }

    prevDungeonId = row.ReadUInt32();
   
    std::string areaStrs = row.ReadString();
    if (areaStrs != "-")
    {
        std::vector<std::string> areaGroupStrs;
        Avalon::StringUtil::Split(areaStrs, areaGroupStrs, "|");
        for (auto areaGroupStr : areaGroupStrs)
        {
			AreaGroup group;

			std::vector<std::string> params;
			Avalon::StringUtil::Split(areaGroupStr, params, ",");
			for (auto param : params)
			{
				UInt32 areaId = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				group.areaIds.push_back(areaId);
			}
            

			if (group.areaIds.empty())
			{
				ErrorLogStream << "dungeon(" << GetKey() << ") invalid area list." << LogStream::eos;
				return false;
			}

			areaList.push_back(group);
        }
    }
    
    startAreaId = row.ReadUInt32();
	bossAreaId = row.ReadUInt32();

	std::string hellAreaListStr = row.ReadString();
	if (hellAreaListStr != "-")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(hellAreaListStr, strs, "|");
		for (auto str : strs)
		{
			UInt32 areaId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			if (areaId)
			{
				hellAreaList.push_back(areaId);
			}
		}
	}
	clearMinAreaNum = row.ReadUInt32();

	isExpLevelAdapt = row.ReadUInt32() != 0;
    expReward = row.ReadUInt32();

	normalDropItem = row.ReadUInt32();
	eliteDropItem = row.ReadUInt32();
	bossDropItem = row.ReadUInt32();
	dungeonDropItem = row.ReadUInt32();

	{
		std::string dropStr = row.ReadString();
		if (dropStr != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(dropStr, strs, "|");
			for (auto str : strs)
			{
				UInt32 dropId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
				if (dropId > 0)
				{
					activityDropItems.push_back(dropId);
				}
			}
		}
	}
	
	dungeonFirstDropItem = row.ReadUInt32();
	destructDropItem = row.ReadUInt32();
	easterEggDropItem = row.ReadUInt32();

    std::string taskDropStr = row.ReadString();
    if (taskDropStr != "-")
    {
        std::vector<std::string> strs;
        Avalon::StringUtil::Split(taskDropStr, strs, "|");
        for (auto& str : strs)
        {
            taskDropItems.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
        }
    }

	rollDropId = row.ReadUInt32();

    costFatiguePerArea = row.ReadUInt16();

    ticket.itemId = row.ReadUInt32();
    ticket.num = row.ReadUInt32();
    hellTicketNum = row.ReadUInt32();
    hellTask = row.ReadUInt32();

	//深渊难度掉落
	for (size_t i=HELL_MODE_NORMAL; i < HELL_MODE_MAX; ++i)
	{
		std::string hellDropss = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(hellDropss, strs, "|");
		for (auto& str : strs)
		{
			hellDrops[i].push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}

	//深渊活动掉落，掉在柱子上
	{
		std::string hellActivityDropString = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(hellActivityDropString, strs, "|");
		for (auto& str : strs)
		{
			hellActivityDropItems.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}

	maxReviveTimes = row.ReadUInt32();
	maxTeamCopyReviveTimes = row.ReadUInt32();

	{
		std::string raceEndDropBaseMultiStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(raceEndDropBaseMultiStr, strs, "|");
		for (auto& str : strs)
		{
			raceEndDropBaseMulti.push_back(Avalon::StringUtil::ConvertToValue<UInt8>(str));
		}
	}

	{
		std::string raceEndDropMultiCostStr = row.ReadString();
		if (raceEndDropMultiCostStr != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(raceEndDropMultiCostStr, strs, "|");
			for (auto& info : strs)
			{
				std::vector<std::string> params;
				Avalon::StringUtil::Split(info, params, ":");
				if (params.size() != 2)
				{
					return false;
				}

				RaceEndDropMultiCost cost;
				cost.multi = Avalon::StringUtil::ConvertToValue<UInt8>(params[0]);
				cost.cost = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
				raceEndDropMultiCost.push_back(cost);
			}
		}
	}

	activityId = row.ReadUInt32();
	dailyMaxTime = row.ReadUInt32();

	std::string quickItemStr = row.ReadString();
	if (quickItemStr != "-")
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(quickItemStr, strs, "|");
		for (auto& str : strs)
		{
			quickUseItems.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
		}
	}

	hellSplitLevel = row.ReadUInt32();
	hellProb = row.ReadUInt32();
	onlyRaceEndProfit = row.ReadUInt8() != 0;
	masterExpAddition = row.ReadUInt8() != 0;

	std::string guideTaskStr = row.ReadString();
	if (guideTaskStr.size() > 0)
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(guideTaskStr, strs, ",");
		for (auto& str : strs)
		{
			std::vector<std::string> vstrs;
			Avalon::StringUtil::Split(str, vstrs, "|");
			if (vstrs.size() == 2)
			{
				UInt32 key = Avalon::StringUtil::ConvertToValue<UInt32>(vstrs[0]);
				UInt32 v = Avalon::StringUtil::ConvertToValue<UInt32>(vstrs[1]);
				guideTasks[key] = v;
			}
		}
	}

	singleDungeon = row.ReadUInt32() != 0;
	onlyRaceEndSettlement = row.ReadUInt32() != 0;

	ownerEntryId = row.ReadInt32();
	weightEntry = row.ReadInt32();

	eliteDungeonPrevChapter = row.ReadInt32();
    return true;
}

bool DungeonDataEntry::IsHellEntry() const
{
	return subType == DST_HELL_ENTRY;
}

bool DungeonDataEntry::IsHellDungeon() const
{
	return subType == DST_HELL;
}

int DungeonDataEntry::GetAreaIndex(UInt32 areaId) const
{
	for (size_t i = 0; i < areaList.size(); i++)
	{
		const auto& group = areaList[i];
		if (group.HasArea(areaId))
		{
			return i;
		}
	}

	return -1;
}

bool DungeonDataEntry::IsWeekHellEntry() const
{
	return subType == DST_WEEK_HELL_ENTRY;
}

bool DungeonDataEntry::ReadDropIDs(std::string dropIDStrs, UInt32* dropIDs)
{
    if (!dropIDs)
    {
        return false;
    }

    memset(dropIDs, 0, sizeof(dropIDs[0]) * LHT_NUM);

    if (dropIDStrs == "-")
    {
        return true;
    }
    
    std::vector<std::string> strs;
    Avalon::StringUtil::Split(dropIDStrs, strs, "|");
    if (strs.size() != LHT_NUM)
    {
        return false;
    }

    for (UInt32 i = 0; i < strs.size(); i++)
    {
        dropIDs[i] = Avalon::StringUtil::ConvertToValue<UInt32>(strs[i]);
    }

    return true;
}


bool DungeonDataEntryMgr::AddEntry(DungeonDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DungeonDataEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->subType == DST_HELL)
	{
		UInt32 key = entry->hellSplitLevel * 10 + entry->hardType;
		HellDungeonInfo info;
		info.dungeonId = entry->GetKey();
		info.prob = entry->hellProb;
		m_HellDungeonMap[key].push_back(info);
	}
	else if (entry->subType == DST_HELL_ENTRY)
	{
		UInt32 key = entry->hellSplitLevel * 10 + entry->hardType;
		m_HellEntryMap[key] = entry->GetKey();
	}
	else if (entry->ownerEntryId > 0)
	{
		HellDungeonInfo info;
		info.dungeonId = entry->GetKey();
		info.prob = entry->weightEntry;
		m_DungeonMap[entry->ownerEntryId].push_back(info);
		m_DungeonMapEntry[entry->GetKey()] = entry->ownerEntryId;
	}

	if (entry->type == LT_NORMAL && entry->subType == DST_NORMAL
		&& entry->threeType == DTT_NORMAL)
	{
		if (entry->tag > 0 && entry->tag < 20)
		{
			DungeonChapterKey key(entry->tag, entry->hardType);
			auto iter = m_ChapterDungeonIds.find(key);
			if (iter == m_ChapterDungeonIds.end())
			{
				std::vector<UInt32> dungeonIds;
				dungeonIds.push_back(entry->GetKey());
				m_ChapterDungeonIds.insert(std::pair<DungeonChapterKey, std::vector<UInt32>>(key, dungeonIds));
			}
			else
			{
				iter->second.push_back(entry->GetKey());
			}
		}
	}

	return true;
}

UInt32 DungeonDataEntryMgr::GenHellDungeonByLevel(UInt32 level, DungeonHardType hard)
{
	UInt32 key = level * 10 + hard;
	auto itr = m_HellDungeonMap.find(key);
	if (itr == m_HellDungeonMap.end() || itr->second.empty())
	{
		return 0;
	}

	UInt32 totalProb = 0;
	for (auto info : itr->second)
	{
		totalProb += info.prob;
	}

	if (totalProb == 0)
	{
		return 0;
	}

	UInt32 prob = Avalon::Random::RandBetween(0, totalProb - 1);
	for (auto info : itr->second)
	{
		if (prob < info.prob)
		{
			return info.dungeonId;
		}
		prob -= info.prob;
	}

	return itr->second[0].dungeonId;
}

UInt32 DungeonDataEntryMgr::GetHellEntry(UInt32 level, DungeonHardType hard)
{
	UInt32 key = level * 10 + hard;
	auto itr = m_HellEntryMap.find(key);
	return itr == m_HellEntryMap.end() ? 0 : itr->second;
}

UInt32 DungeonDataEntryMgr::GetDungeonIdByEntryId(UInt32 entryId)
{
	auto itr = m_DungeonMap.find(entryId);
	if (itr == m_DungeonMap.end() || itr->second.empty())
	{
		return 0;
	}

	UInt32 totalProb = 0;
	for (auto info : itr->second)
	{
		totalProb += info.prob;
	}

	if (totalProb == 0)
	{
		return 0;
	}

	UInt32 prob = Avalon::Random::RandBetween(0, totalProb - 1);
	for (auto info : itr->second)
	{
		if (prob < info.prob)
		{
			return info.dungeonId;
		}
		prob -= info.prob;
	}

	return itr->second[0].dungeonId;
}

UInt32 DungeonDataEntryMgr::GetDungeonEntryIdByDungeonId(UInt32 dungeonId)
{
	auto itr = m_DungeonMapEntry.find(dungeonId);
	if (itr == m_DungeonMapEntry.end())
	{
		return 0;
	}
	return itr->second;
}

bool  DungeonDataEntryMgr::GetDungeonIdsByChapterHard(UInt32 chapter, UInt8 hard, std::vector<UInt32>& ids)
{
	DungeonChapterKey key(chapter, hard);
	auto itr = m_ChapterDungeonIds.find(key);
	if (itr != m_ChapterDungeonIds.end())
	{
		ids = itr->second;
		return true;
	}
	
	return false;
}