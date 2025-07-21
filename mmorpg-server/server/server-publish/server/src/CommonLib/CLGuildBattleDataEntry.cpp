#include "CLGuildBattleDataEntry.h"


bool GuildTerritoryDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt8();
	name = row.ReadString();
	type = (GuildTerritoryType)row.ReadUInt8();
	level = row.ReadUInt8();
	needTerritoryId = row.ReadUInt8();
	needGuildLevel = row.ReadUInt8();
	preTerritoryId = row.ReadUInt8();
	enroll = row.ReadUInt32();

	std::string split = "|";
	std::string split2 = ",";
	std::string split3 = "_";
	std::string strConsumeItems = row.ReadString();
	std::vector<std::string> consumeItemVec;
	Avalon::StringUtil::Split(strConsumeItems, consumeItemVec, split);
	for (UInt32 i = 0; i < consumeItemVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(consumeItemVec[i], strTmp, split3);
		if (strTmp.size() < 2) continue;

		ConsumeItem item;
		item.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		item.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		consumeItem.push_back(item);
	}

	std::string strScores = row.ReadString();
	std::vector<std::string> strScoreVec;
	Avalon::StringUtil::Split(strScores, strScoreVec, split);
	for (UInt32 i = 0; i < strScoreVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(strScoreVec[i], strTmp, split2);
		if (strTmp.size() < 3) continue;

		GuildBattleScore score;
		score.winScore = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		score.endScore = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		score.loserScore = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);

		guildBattleScore.push_back(score);
	}

	occupyScore = row.ReadUInt32();
	battleScoreFactor = row.ReadUInt32();

	std::string strLeaderRewards = row.ReadString();
	ParseReward(leaderRewards, strLeaderRewards);

	std::string strMemberRewards = row.ReadString();
	ParseReward(memberRewards, strMemberRewards);

	std::string LDayRewards = row.ReadString();
	ParseReward(leaderDayRewards, LDayRewards);

	std::string MDayRewards = row.ReadString();
	ParseReward(memberDayRewards, MDayRewards);

	std::string strLoseRewards = row.ReadString();
	ParseReward(loseRewards, strLoseRewards);

	std::string strMatchConsumes = row.ReadString();
	std::vector<std::string> strMatchConsumeVec;
	Avalon::StringUtil::Split(strMatchConsumes, strMatchConsumeVec, split);
	for (UInt32 i = 0; i < strMatchConsumeVec.size(); ++i)
	{
		std::vector<std::string> strItemTmpVec;
		Avalon::StringUtil::Split(strMatchConsumeVec[i], strItemTmpVec, split2);
		std::vector<ConsumeItem> consumeItems;
		for (UInt32 j = 0; j < strItemTmpVec.size(); ++j)
		{
			std::vector<std::string> strTmp;
			Avalon::StringUtil::Split(strItemTmpVec[j], strTmp, split3);
			if (strTmp.size() < 2) continue;

			ConsumeItem consume;
			consume.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			consume.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			consumeItems.push_back(consume);
		}
		matchConsumeItems.push_back(consumeItems);
	}

	{
		std::string strDungeons = row.ReadString();
		std::vector<std::string> strDungeonVec;
		Avalon::StringUtil::Split(strDungeons, strDungeonVec, split);
		for (UInt32 i = 0; i < strDungeonVec.size(); ++i)
		{
			UInt32 dungeonId = Avalon::StringUtil::ConvertToValue<UInt32>(strDungeonVec[i]);
			if (dungeonId == 0) continue;
			chestDoubleDungeons.push_back(dungeonId);
		}
	}
	

	dailyChestDoubleTimes = row.ReadUInt32();

	{
		chestDoubleFlag = 0;
		auto flagStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(flagStr, strs, "|");
		for (auto str : strs)
		{
			auto value = Avalon::StringUtil::ConvertToValue<UInt8>(str);
			if (value)
			{
				chestDoubleFlag |= 1 << (value - 1);
			}
		}
	}

	{
		std::string strDungeons = row.ReadString();
		std::vector<std::string> strDungeonVec;
		Avalon::StringUtil::Split(strDungeons, strDungeonVec, split);
		for (UInt32 i = 0; i < strDungeonVec.size(); ++i)
		{
			UInt32 dungeonId = Avalon::StringUtil::ConvertToValue<UInt32>(strDungeonVec[i]);
			if (dungeonId == 0) continue;
			dropAdditionDungeons.push_back(dungeonId);
		}
	}

	dropAddition = row.ReadUInt32();
	leaderPveAddSkill = row.ReadUInt32();
	leaderPvpAddSkill = row.ReadUInt32();

	return true;
}

const static std::vector<ConsumeItem> NullMatchConsumeItem;
std::vector<ConsumeItem> GuildTerritoryDataEntry::GetMatchConsume(UInt32 index)
{
	if (matchConsumeItems.empty()) return NullMatchConsumeItem;
	UInt32 tmpIndex = matchConsumeItems.size() - 1;
	if (index < tmpIndex) tmpIndex = index;
	return matchConsumeItems[tmpIndex];
}

bool GuildTerritoryDataEntry::IsChestDoubleDungeon(UInt32 dungeonId)
{
	std::vector<UInt32>::iterator iter = std::find(chestDoubleDungeons.begin(), chestDoubleDungeons.end(), dungeonId);
	return iter != chestDoubleDungeons.end();
}

bool GuildTerritoryDataEntry::IsDropAdditionDungeon(UInt32 dungeonId)
{
	std::vector<UInt32>::iterator iter = std::find(dropAdditionDungeons.begin(), dropAdditionDungeons.end(), dungeonId);
	return iter != dropAdditionDungeons.end();
}

void GuildTerritoryDataEntry::ParseReward(ItemRewardVec& rewardVec, std::string& strReward)
{
	std::vector<std::string> str_1;
	Avalon::StringUtil::Split(strReward, str_1, "|");
	for (UInt32 i = 0; i < str_1.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(str_1[i], strTmp, "_");
		if (strTmp.size() < 2) continue;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		rewardVec.push_back(reward);
	}
}

GuildTerritoryDataEntryMgr::GuildTerritoryDataEntryMgr() :m_MaxLevel(0)
{
}

GuildTerritoryDataEntryMgr::~GuildTerritoryDataEntryMgr()
{
}

bool GuildTerritoryDataEntryMgr::AddEntry(GuildTerritoryDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildTerritoryDataEntry>::AddEntry(entry)) return false;

	if (entry->level > m_MaxLevel) m_MaxLevel = entry->level;

	return true;
}

bool GuildInspireDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt8();
	needGuildLevel = row.ReadUInt8();

	std::string split = "|";
	std::string split2 = "_";
	std::string strConsumeItems = row.ReadString();
	std::vector<std::string> consumeItemVec;
	Avalon::StringUtil::Split(strConsumeItems, consumeItemVec, split);
	for (UInt32 i = 0; i < consumeItemVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(consumeItemVec[i], strTmp, split2);
		if (strTmp.size() < 2) continue;

		ConsumeItem item;
		item.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		item.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		consumeItem.push_back(item);
	}

	std::string strCrossConsumeItems = row.ReadString();
	std::vector<std::string> crossConsumeItemVec;
	Avalon::StringUtil::Split(strCrossConsumeItems, crossConsumeItemVec, split);
	for (UInt32 i = 0; i < crossConsumeItemVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(crossConsumeItemVec[i], strTmp, split2);
		if (strTmp.size() < 2) continue;

		ConsumeItem item;
		item.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		item.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		crossConsumeItem.push_back(item);
	}

	return true;
}

GuildInspireDataEntryMgr::GuildInspireDataEntryMgr() : m_MaxLevel(0)
{
}

GuildInspireDataEntryMgr::~GuildInspireDataEntryMgr()
{
}

bool GuildInspireDataEntryMgr::AddEntry(GuildInspireDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildInspireDataEntry>::AddEntry(entry)) return false;

	if (entry->level > m_MaxLevel) m_MaxLevel = entry->level;

	return true;
}


GuildRewardDataEntry::GuildRewardDataEntry()
{
}

GuildRewardDataEntry::~GuildRewardDataEntry()
{
}

bool GuildRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	beginScore = row.ReadUInt32();

	std::string split = "|";
	std::string split3 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split3);
		if (strTmp.size() < 2) continue;

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		itemReward.push_back(reward);
	}

	return true;
}

bool GuildBattleTimeDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	group = row.ReadUInt32();
	type = (GuildBattleType)row.ReadUInt32();
	status = (GuildBattleStatus)row.ReadUInt32();
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

bool GuildBattleTimeDataEntryMgr::AddEntry(GuildBattleTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildBattleTimeDataEntry>::AddEntry(entry)) return false;

	if (!entry->isOpen) return true;

	GuildBattleTimeMap::iterator iter = m_GuildBattleTimeMap.find(entry->group);
	if (iter != m_GuildBattleTimeMap.end())
	{
		GuildBattleStatusMap::iterator statusIter = iter->second.find(entry->status);
		if (statusIter != iter->second.end())
			statusIter->second = entry;
		else
			iter->second.insert(std::make_pair(entry->status, entry));
	}
	else
	{
		GuildBattleStatusMap tmpStatusMap;
		tmpStatusMap.insert(std::make_pair(entry->status, entry));
		m_GuildBattleTimeMap.insert(std::make_pair(entry->group, tmpStatusMap));
	}

	return true;
}

GuildBattleTimeInfo GuildBattleTimeDataEntryMgr::GetGuildBattleTimeInfo(Avalon::Time now, GuildBattleType type)
{
	GuildBattleTimeInfo info;
	GuildBattleTimeMap::iterator iter = m_GuildBattleTimeMap.begin();
	Avalon::Date nowDate(now);
	while (iter != m_GuildBattleTimeMap.end())
	{
		GuildBattleTimeInfo tmpInfo;
		GuildBattleStatusMap::iterator statusIter = iter->second.begin();

		if (statusIter == iter->second.end() || (type != GuildBattleType::GBT_INVALID && type != statusIter->second->type))
		{
			++iter;
			continue;
		}

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

			tmpInfo.type = statusIter->second->type;
			switch (statusIter->second->status)
			{
			case GuildBattleStatus::GBS_ENROLL:
				tmpInfo.enrollTime = tmpTime;
				break;
			case GuildBattleStatus::GBS_PREPARE:
				tmpInfo.preTime = tmpTime;
				break;
			case GuildBattleStatus::GBS_BATTLE:
				tmpInfo.battleTime = tmpTime;
				break;
			case GuildBattleStatus::GBS_REWARD:
				tmpInfo.rewardTime = tmpTime;
				break;
			case GuildBattleStatus::GBS_INVALID:
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
			tmpInfo.enrollTime += intervalTime;
			tmpInfo.preTime += intervalTime;
			tmpInfo.battleTime += intervalTime;
			tmpInfo.rewardTime += intervalTime;
			tmpInfo.endTime += intervalTime;
		}

		if (info.enrollTime == 0 || info.enrollTime > tmpInfo.enrollTime)
			info = tmpInfo;

		++iter;
	}
	return info;
}

UInt32 GuildBattleTimeDataEntryMgr::GetLastGuildBattleTim(Avalon::Time now, GuildBattleType type)
{
	GuildBattleTimeMap::iterator iter = m_GuildBattleTimeMap.begin();
	Avalon::Date nowDate(now);
	std::vector<UInt32> timesv;
	while (iter != m_GuildBattleTimeMap.end())
	{
		GuildBattleStatusMap::iterator statusIter = iter->second.begin();
		if (statusIter == iter->second.end() || (type != GuildBattleType::GBT_INVALID && type != statusIter->second->type))
		{
			++iter;
			continue;
		}

		GuildBattleStatusMap::iterator statusIter_perpare = iter->second.find(GBS_PREPARE);
		if (statusIter_perpare == iter->second.end() || !statusIter_perpare->second)
		{
			++iter;
			continue;
		}

		GuildBattleTimeDataEntry* entry = statusIter_perpare->second;
		
		Avalon::Date tmpDate(now);
		tmpDate.Hour(entry->hour);
		tmpDate.Min(entry->min);
		tmpDate.Sec(entry->sec);

		UInt32 tmpTime = (UInt32)tmpDate.ToTime().Sec();
		if (entry->week != 0)
		{
			tmpTime = UInt32(tmpDate.ToTime().Sec()) - (tmpDate.WDay() - entry->week) * DAY_TO_SEC;
		}
		timesv.push_back(tmpTime);
		++iter;
	}

	std::sort(timesv.begin(), timesv.end());

	UInt32 curr_time = (UInt32)now.Sec();
	for (size_t i = 0; i < timesv.size(); ++i)
	{
		if (i == 0)
		{
			if (curr_time < timesv[i])
			{
				return timesv[timesv.size() - 1] - 7 * DAY_TO_SEC;
			}
		}

		if (i + 1 < timesv.size())
		{
			if (curr_time >= timesv[i] && curr_time < timesv[i + 1])
			{
				return timesv[i];
			}
		}
		else
		{
			if (curr_time >= timesv[i])
			{
				return timesv[i];
			}
		}
	}

	return 0;
}

UInt32 GuildBattleTimeDataEntryMgr::GetGuildCrossBattleEndTime(Avalon::Time now)
{
	GuildBattleTimeInfo info = GetGuildBattleTimeInfo(now, GuildBattleType::GBT_CROSS);
	if (info.type != GuildBattleType::GBT_CROSS)
	{
		return 0;
	}

	UInt32 nowTime = (UInt32)now.Sec();

	if (info.rewardTime <= nowTime && info.endTime > nowTime)
	{
		return info.rewardTime;
	}

	return info.rewardTime - 7 * DAY_TO_SEC;
}

bool GuildBattleLotteryNumDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	num = row.ReadUInt16();
	return true;
}
