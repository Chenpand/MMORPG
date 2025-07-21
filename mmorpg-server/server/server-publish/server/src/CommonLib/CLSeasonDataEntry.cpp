#include "CLSeasonDataEntry.h"

bool SeasonDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	eventType = (SeasonEventType)row.ReadUInt32();
	cyclesType = (SeasonCyclesType)row.ReadUInt32();
	day = row.ReadUInt32();
	startTime = row.ReadString();
	return true;
}

SeasonDataEntryMgr::SeasonDataEntryMgr()
{
	m_SeasonCyclesType = SCT_INVALID;
}

SeasonDataEntryMgr::~SeasonDataEntryMgr()
{
}

bool SeasonDataEntryMgr::AddEntry(SeasonDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SeasonDataEntry>::AddEntry(entry)) return false;

	if (m_SeasonCyclesType == SCT_INVALID)
		m_SeasonCyclesType = entry->cyclesType;
	else if (m_SeasonCyclesType != entry->cyclesType)
		return false;

	m_SeasonDayMap[entry->day] = entry;

	return true;
}

UInt64 SeasonDataEntryMgr::GetNextSeasonTime(SeasonEventType eventType, Avalon::Date nowDate)
{
	UInt64 result = 0;
	if (m_SeasonDayMap.size() <= 0) return result;
	std::map<UInt32, SeasonDataEntry*>::iterator iter = m_SeasonDayMap.begin();
	while (iter != m_SeasonDayMap.end())
	{
		if (!(iter->second->eventType & eventType))
		{
			++iter;
			continue;
		}
		Avalon::Date tmpTime = nowDate;
		tmpTime.Day(iter->first);
		std::istringstream	is(iter->second->startTime);
		UInt32 nVal = 0;
		char split = 0;
		is >> nVal >> split;
		tmpTime.Hour(nVal);
		is >> nVal >> split;
		tmpTime.Min(nVal);
		is >> nVal;
		tmpTime.Sec(nVal);
		tmpTime.MSec(0);

		if (tmpTime.ToTime().MSec() > nowDate.ToTime().MSec())
		{
			result = tmpTime.ToTime().MSec();
			break;
		}
		iter++;
	}
	if (result == 0)
	{
		iter = m_SeasonDayMap.begin();
		while (iter != m_SeasonDayMap.end())
		{
			if (iter->second->eventType & eventType)
			{
				break;
			}
			++iter;
		}
		Avalon::Date tmpTime = nowDate;
		tmpTime.Month(tmpTime.Month() + 1);
		tmpTime.Day(iter->first);
		std::istringstream	is(iter->second->startTime);
		UInt32 nVal = 0;
		char split = 0;
		is >> nVal >> split;
		tmpTime.Hour(nVal);
		is >> nVal >> split;
		tmpTime.Min(nVal);
		is >> nVal;
		tmpTime.Sec(nVal);
		tmpTime.MSec(0);
		result = tmpTime.ToTime().MSec();
	}
	return result;
}

UInt64 SeasonDataEntryMgr::GetNowSeasonTime(SeasonEventType eventType, Avalon::Date nowDate)
{
	UInt64 result = 0;
	if (m_SeasonDayMap.size() <= 0) return result;
	std::map<UInt32, SeasonDataEntry*>::iterator iter = m_SeasonDayMap.begin();
	while (iter != m_SeasonDayMap.end())
	{
		if (!(iter->second->eventType & eventType))
		{
			++iter;
			continue;
		}
		Avalon::Date tmpTime = nowDate;
		tmpTime.Day(iter->first);
		std::istringstream	is(iter->second->startTime);
		UInt32 nVal = 0;
		char split = 0;
		is >> nVal >> split;
		tmpTime.Hour(nVal);
		is >> nVal >> split;
		tmpTime.Min(nVal);
		is >> nVal;
		tmpTime.Sec(nVal);
		tmpTime.MSec(0);

		if (nowDate.ToTime().MSec() > tmpTime.ToTime().MSec())
		{
			result = tmpTime.ToTime().MSec();
		}
		iter++;
	}
	if (result == 0)
	{
		std::map<UInt32, SeasonDataEntry*>::reverse_iterator iter = m_SeasonDayMap.rbegin();
		while (iter != m_SeasonDayMap.rend())
		{
			if (iter->second->eventType & eventType)
			{
				break;
			}
			++iter;
		}

		Avalon::Date tmpTime = nowDate;
		tmpTime.Month(tmpTime.Month() - 1);
		tmpTime.Day(iter->first);
		std::istringstream	is(iter->second->startTime);
		UInt32 nVal = 0;
		char split = 0;
		is >> nVal >> split;
		tmpTime.Hour(nVal);
		is >> nVal >> split;
		tmpTime.Min(nVal);
		is >> nVal;
		tmpTime.Sec(nVal);
		tmpTime.MSec(0);
		result = tmpTime.ToTime().MSec();
	}
	return result;
}

bool SeasonLevelDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	preId = row.ReadUInt32();
	afterId = row.ReadUInt32();
	mainLevel = row.ReadUInt32();
	smallLevel = row.ReadUInt32();
	star = row.ReadUInt32();
	maxExp = row.ReadUInt32();
	hideScore = row.ReadUInt32();

	UInt32 tmp = row.ReadUInt32();
	isRobot = tmp == 0 ? false : true;

	tmp = row.ReadUInt32();
	isDownLevel = tmp == 0 ? false : true;

	tmp = row.ReadUInt32();
	isPromotion = tmp == 0 ? false : true;

	std::string uplevelBattleStr = row.ReadString();
	std::vector<std::string> uplevelBattleVec;
	Avalon::StringUtil::Split(uplevelBattleStr, uplevelBattleVec, ",");
	if (uplevelBattleVec.size() == 4)
	{
		uplevelBattleWinCount = Avalon::StringUtil::ConvertToValue<UInt32>(uplevelBattleVec[0]);
		uplevelBattleCount = Avalon::StringUtil::ConvertToValue<UInt32>(uplevelBattleVec[1]);
		uplevelBattleWinExp = Avalon::StringUtil::ConvertToValue<UInt32>(uplevelBattleVec[2]);
		uplevelBattleLoseExp = Avalon::StringUtil::ConvertToValue<UInt32>(uplevelBattleVec[3]);
	}
	else
	{
		uplevelBattleWinCount = 0;
		uplevelBattleCount = 0;
		uplevelBattleWinExp = 0;
		uplevelBattleLoseExp = 0;
	}

	std::string tmpStr = row.ReadString();
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(tmpStr, tmpVec, ",");
	if (tmpVec.size() == 2)
	{
		battleDayNumber = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[0]);
		battleCount = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec[1]);
	}
	else
	{
		battleDayNumber = 0;
		battleCount = 0;
	}
	downLevel = row.ReadUInt32();

	std::istringstream dailyStream(row.ReadString());
	char split = 0;
	while (dailyStream)
	{
		ItemReward item;
		item.DecodeString(dailyStream);
		if (item.id == 0) break;
		dailyRewards.push_back(item);
		dailyStream >> split;
	}

	std::istringstream seasonStream(row.ReadString());
	while (seasonStream)
	{
		ItemReward item;
		item.DecodeString(seasonStream);
		if (item.id == 0) break;
		seasonRewards.push_back(item);
		seasonStream >> split;
	}

	attrId = row.ReadUInt32();
	return true;
}

bool SeasonLevelDataEntry::IsUplevelBattle()
{
	return isPromotion && uplevelBattleCount > 0 && uplevelBattleWinCount > 0;
}

SeasonLevelDataEntryMgr::SeasonLevelDataEntryMgr() 
{
	m_SeasonExpVec.push_back(SeasonExpEntry(100, 100));
	m_SeasonExpVec.push_back(SeasonExpEntry(50, 150));
	m_SeasonExpVec.push_back(SeasonExpEntry(20, 180));
	m_SeasonExpVec.push_back(SeasonExpEntry(5, 195));
	
	m_SeasonExpVecSize = m_SeasonExpVec.size();
}

SeasonLevelDataEntryMgr::~SeasonLevelDataEntryMgr() { }

bool SeasonLevelDataEntryMgr::AddEntry(SeasonLevelDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SeasonLevelDataEntry>::AddEntry(entry)) return false;

	m_MatchScoreVec.push_back(entry);

	if (entry->preId == 0)
	{
		m_MinSeasonLevel = entry;
	}
	else
	{
		//m_AfterSeasonMap[entry->preId] = entry;
	}

	if (entry->afterId == 0)
	{
		m_MaxSeasonLevel = entry;
	}
	else
	{
		//m_PreSeasonMap[entry->afterId] = entry;
	}

	SeasonLevelDataEntry* preEntry = FindEntry(entry->preId);
	if (preEntry == NULL)
	{
		m_SeasonLevelExpMap[entry->id] = m_SeasonLevelExpMap[entry->preId] + 0;
	}
	else
	{
		m_SeasonLevelExpMap[entry->id] = m_SeasonLevelExpMap[entry->preId] + preEntry->maxExp;
	}
	m_SeasonExpLevelMap[m_SeasonLevelExpMap[entry->id] + entry->maxExp] = entry->id;

	return true;
}

SeasonLevelDataEntry* SeasonLevelDataEntryMgr::GetSeasonLevel(UInt32 score)
{
	SeasonLevelDataEntry* result = NULL;
	for (UInt32 i = 0; i < m_MatchScoreVec.size(); ++i)
	{
		if (m_MatchScoreVec[i]->hideScore <= score)
			result = m_MatchScoreVec[i];
		else
			break;
	}
	return result;
}

SeasonLevelDataEntry* SeasonLevelDataEntryMgr::GetMinSeasonLevel()
{
	return m_MinSeasonLevel;
}

UInt32 SeasonLevelDataEntryMgr::GetMinSeasonLevelValue()
{
	if (m_MinSeasonLevel != NULL)
	{
		return m_MinSeasonLevel->id;
	}
	return 0;
}

UInt32 SeasonLevelDataEntryMgr::GetMaxSeasonLevelValue()
{
	if (m_MaxSeasonLevel != NULL)
	{
		return m_MaxSeasonLevel->id;
	}
	return 0;
}

/*
SeasonLevelDataEntry* SeasonLevelDataEntryMgr::GetPreSeason(UInt32 id)
{
	std::map<UInt32, SeasonLevelDataEntry*>::iterator iter = m_PreSeasonMap.find(id);
	if (iter != m_PreSeasonMap.end())
		return iter->second;
	return NULL;
}
*/

/*
SeasonLevelDataEntry* SeasonLevelDataEntryMgr::GetAfterSeason(UInt32 id)
{
	std::map<UInt32, SeasonLevelDataEntry*>::iterator iter = m_AfterSeasonMap.find(id);
	if (iter != m_AfterSeasonMap.end())
		return iter->second;
	return NULL;
}
*/

ItemRewardVec SeasonLevelDataEntryMgr::GetDailyItemRewardVec(UInt32 id)
{
	ItemRewardVec result;
	SeasonLevelDataEntry* dataEntry = FindEntry(id);
	if (dataEntry != NULL)
		result = dataEntry->dailyRewards;
	return result;
}

UInt32 SeasonLevelDataEntryMgr::GetSeasonExp(UInt32 winMainLevel, UInt32 loseMainLevel)
{
	if (winMainLevel >= loseMainLevel)
	{
		UInt32 index = winMainLevel - loseMainLevel;
		if (index > m_SeasonExpVecSize - 1) index = m_SeasonExpVecSize - 1;
		return m_SeasonExpVec[index].winExp;
	}
	else
	{
		UInt32 index = loseMainLevel - winMainLevel;
		if (index > m_SeasonExpVecSize - 1) index = m_SeasonExpVecSize - 1;
		return m_SeasonExpVec[index].loseExp;
	}
}

bool SeasonLevelDataEntryMgr::GetSeasonLevelByExp(UInt32& seasonLevel, UInt32& seasonStar, UInt32& seasonExp, Int32 incExp)
{
	SeasonLevelDataEntry* dataEntry = FindEntry(seasonLevel);
	if (dataEntry != NULL) return false;

	if (incExp < 0)
	{
		while (incExp > 0)
		{
			if ((Int32)seasonExp >= incExp)
			{
				seasonExp = seasonExp - incExp;
				incExp = 0;
			}
			else
			{
				incExp = incExp - seasonExp;
				UInt32 downLevel = 1;
				if (seasonStar > downLevel)
				{
					seasonStar = seasonStar - downLevel;
					downLevel = 0;
				}
				else
				{
					downLevel = downLevel - seasonStar;
					seasonStar = 0;
					if (dataEntry->afterId == 0)
					{
						downLevel = downLevel + 1;
					}
				}

				SeasonLevelDataEntry* tmpDataEntry = dataEntry;
				while (downLevel > 0)
				{
					tmpDataEntry = FindEntry(dataEntry->preId);
					if (tmpDataEntry == NULL)
					{
						tmpDataEntry = SeasonLevelDataEntryMgr::Instance()->GetMinSeasonLevel();
						break;
					}
					downLevel--;
				}
				dataEntry = tmpDataEntry;
				seasonLevel = dataEntry->id;
				seasonExp = dataEntry->maxExp;
			}
		}
	}
	else
	{
		seasonExp = seasonExp + incExp;

		while (seasonExp >= dataEntry->maxExp)
		{
			seasonExp = seasonExp - dataEntry->maxExp;
			if (dataEntry->afterId == 0)
			{
				seasonStar = seasonStar + 1;
			}
			else
			{
				SeasonLevelDataEntry* tmpDataEntry = FindEntry(dataEntry->afterId);
				if (tmpDataEntry == NULL)
				{
					return false;
				}
				seasonLevel = tmpDataEntry->id;
				dataEntry = tmpDataEntry;
				if (dataEntry->afterId == 0)
				{
					seasonStar = 1;
				}
			}
		}
		return true;
	}
	return true;
}

UInt32 SeasonLevelDataEntryMgr::GetSeasonLevelSumExp(UInt32 seasonLevel)
{
	std::map<UInt32, UInt32>::iterator iter = m_SeasonLevelExpMap.find(seasonLevel);
	if (iter == m_SeasonLevelExpMap.end())
		return 0;
	return iter->second;
}

SeasonMainLevel SeasonLevelDataEntryMgr::GetSeasonMainLevelBySumExp(UInt32 exp)
{
	std::map<UInt32, UInt32>::iterator iter = m_SeasonExpLevelMap.upper_bound(exp);
	if (iter == m_SeasonExpLevelMap.end())
		return SeasonMainLevel::SML_KING;
	SeasonLevelDataEntry* dataEntry = FindEntry(iter->second);
	if (dataEntry == NULL)
		return SeasonMainLevel::SML_BRONZE;
	return (SeasonMainLevel)dataEntry->mainLevel;
}

UInt32 SeasonLevelDataEntryMgr::GetSeasonLevelBySumExp(UInt32 exp)
{
	std::map<UInt32, UInt32>::iterator iter = m_SeasonExpLevelMap.upper_bound(exp);
	if (iter == m_SeasonExpLevelMap.end())
	{
		if (m_MaxSeasonLevel != NULL) return m_MaxSeasonLevel->id;
		return 0;
	}

	SeasonLevelDataEntry* dataEntry = FindEntry(iter->second);
	if (dataEntry == NULL)
	{
		if (m_MinSeasonLevel != NULL) return m_MinSeasonLevel->id;
		return 0;
	}
	return dataEntry->id;
}

/*
bool SeasonAttrDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	std::string strTmp = row.ReadString();
	std::vector<std::string> vecTmp;
	Avalon::StringUtil::Split(strTmp, vecTmp, "|");
	for (UInt32 i = 0; i < vecTmp.size(); ++i)
	{
		UInt32 buffId = Avalon::StringUtil::ConvertToValue<UInt32>(vecTmp[i]);
		buffIds.push_back(buffId);
	}
}

bool SeasonAttrDataEntryMgr::AddEntry(SeasonAttrDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SeasonAttrDataEntry>::AddEntry(entry)) return false;
}

SeasonAttrDataEntry* SeasonAttrDataEntryMgr::GetSeasonSortListAttr()
{
	return FindEntry(7);
}
*/
