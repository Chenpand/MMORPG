#include <CLScoreWarDataEntry.h>

bool ScoreWarTimeDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	group = row.ReadUInt32();
	status = (ScoreWarStatus)row.ReadUInt32();

	week = row.ReadUInt32();

	std::string split = ":";
	std::string timeStr = row.ReadString();
	std::vector<std::string> timeValues;
	Avalon::StringUtil::Split(timeStr, timeValues, split);
	if (timeValues.size() >= 3)
	{
		hour = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[0]);
		min = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[1]);
		sec = Avalon::StringUtil::ConvertToValue<UInt32>(timeValues[2]);
	}

	UInt32 tmp = row.ReadUInt32();
	isOpen = tmp == 0 ? false : true;

	return true;
}

bool ScoreWarTimeDataEntryMgr::AddEntry(ScoreWarTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ScoreWarTimeDataEntry>::AddEntry(entry)) return false;

	if (!entry->isOpen) return true;

	ScoreWarTimeMap::iterator iter = m_ScoreWarTimeMap.find(entry->group);
	if (iter != m_ScoreWarTimeMap.end())
	{
		ScoreWarStatusMap::iterator statusIter = iter->second.find(entry->status);
		if (statusIter != iter->second.end())
			statusIter->second = entry;
		else
			iter->second.insert(std::make_pair(entry->status, entry));
	}
	else
	{
		ScoreWarStatusMap tmpStatusMap;
		tmpStatusMap.insert(std::make_pair(entry->status, entry));
		m_ScoreWarTimeMap.insert(std::make_pair(entry->group, tmpStatusMap));
	}

	return true;

}

ScoreWarTimeInfo ScoreWarTimeDataEntryMgr::GetScoreWarTimeInfo(Avalon::Time now)
{
	ScoreWarTimeInfo info;
	ScoreWarTimeMap::iterator iter = m_ScoreWarTimeMap.begin();
	Avalon::Date nowData(now);
	while (iter != m_ScoreWarTimeMap.end())
	{
		ScoreWarTimeInfo tmpInfo;
		ScoreWarStatusMap::iterator statusIter = iter->second.begin();

		bool isWeek = false;
		while (statusIter != iter->second.end())
		{
			Avalon::Date tmpDate(now);
			tmpDate.Hour(statusIter->second->hour);
			tmpDate.Min(statusIter->second->min);
			tmpDate.Sec(statusIter->second->sec);
			UInt32 tmpTime = (UInt32)tmpDate.ToTime().Sec();
			if (statusIter->second->week != 0)
			{
				tmpTime = UInt32(tmpDate.ToTime().Sec()) - (tmpDate.WDay() - statusIter->second->week) * DAY_TO_SEC;
				isWeek = true;
			}

			switch (statusIter->second->status)
			{
			case ScoreWarStatus::SWS_PREPARE:
				tmpInfo.preTime = tmpTime;
				break;
			case ScoreWarStatus::SWS_BATTLE:
				tmpInfo.battleTime = tmpTime;
				break;
			case ScoreWarStatus::SWS_WAIT_END:
				tmpInfo.waitEndTime = tmpTime;
				break;
			case ScoreWarStatus::SWS_INVALID:
				tmpInfo.endTime = tmpTime;
				break;
			default:
				break;
			}
			++statusIter;
		}

		UInt32 intervalTime = DAY_TO_SEC;
		if (isWeek)
		{
			intervalTime = 7 * DAY_TO_SEC;
		}

		if (now.Sec() > tmpInfo.endTime)
		{
			tmpInfo.preTime += intervalTime;
			tmpInfo.battleTime += intervalTime;
			tmpInfo.waitEndTime += intervalTime;
			tmpInfo.endTime += intervalTime;
		}

		if (info.preTime == 0 || info.preTime > tmpInfo.preTime)
			info = tmpInfo;

		++iter;
	}
	return info;
}

bool ScoreWarRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	rewardId = row.ReadUInt32();
	battleCount = row.ReadUInt32();
	winCount = row.ReadUInt32();
	rankingBegin = row.ReadUInt32();
	rankingEnd = row.ReadUInt32();

	std::string split = "|";
	std::string split3 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdLeaderRewardVec;
	Avalon::StringUtil::Split(strRewards, stdLeaderRewardVec, split);
	for (UInt32 i = 0; i < stdLeaderRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdLeaderRewardVec[i], strTmp, split3);
		if (strTmp.size() < 2) continue;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		rewards.push_back(reward);
	}
	return true;
}

bool ScoreWarRewardDataEntryMgr::AddEntry(ScoreWarRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ScoreWarRewardDataEntry>::AddEntry(entry)) return false;

	if (entry->rewardId != 0)
	{
		m_BaseRewords[entry->rewardId] = entry;
	}
	if (entry->rankingBegin != 0 && entry->rankingEnd != 0)
	{
		m_RankingRewards.push_back(entry);
	}
	return true;
}

ScoreWarRewardDataEntry* ScoreWarRewardDataEntryMgr::GetBaseDataEntry(UInt32 rewardId)
{
	std::map<UInt32, ScoreWarRewardDataEntry*>::iterator iter = m_BaseRewords.find(rewardId);
	if (iter != m_BaseRewords.end())
	{
		return iter->second;
	}
	return NULL;
}

ScoreWarRewardDataEntry* ScoreWarRewardDataEntryMgr::GetRankingDataEntry(UInt32 ranking)
{
	std::vector<ScoreWarRewardDataEntry*>::iterator iter = m_RankingRewards.begin();
	while (iter != m_RankingRewards.end())
	{
		if ((*iter)->rankingBegin <= ranking && (*iter)->rankingEnd >= ranking)
		{
			return (*iter);
		}
		++iter;
	}
	return NULL;
}

