#include "CLSeasonRewardDataEntry.h"

SeasonRewardDataEntry::SeasonRewardDataEntry()
{

}

SeasonRewardDataEntry::~SeasonRewardDataEntry()
{

}

bool SeasonRewardDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();

	{
		std::string rankRangeStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(rankRangeStr, strs, "|");
		if (strs.size() == 1)
		{
			startRank = endRank = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		}
		else if (strs.size() == 2)
		{
			startRank = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			endRank = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		}
		else
		{
			return false;
		}
	}
	
	{
		std::string rewardStr = row.ReadString();
		std::vector<std::string> rewardStrs;
		Avalon::StringUtil::Split(rewardStr, rewardStrs, "|");
		for (auto& str : rewardStrs)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, ",");
			if (strs.size() != 2)
			{
				return false;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			reward.qualityLv = 0;
			rewards.push_back(reward);
		}
	}

	return true;
}

SeasonDailyDataEntry::SeasonDailyDataEntry()
{
}

SeasonDailyDataEntry::~SeasonDailyDataEntry()
{
}


bool SeasonDailyDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();

	{
		std::string rankRangeStr = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(rankRangeStr, strs, "|");
		if (strs.size() == 1)
		{
			startScore = endScore = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		}
		else if (strs.size() == 2)
		{
			startScore = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			endScore = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		}
		else
		{
			return false;
		}
	}

	{
		std::string rewardStr = row.ReadString();
		std::vector<std::string> rewardStrs;
		Avalon::StringUtil::Split(rewardStr, rewardStrs, "|");
		for (auto& str : rewardStrs)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, ",");
			if (strs.size() != 2)
			{
				return false;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			reward.qualityLv = 0;
			rewards.push_back(reward);
		}
	}

	return true;
}

SeasonDailyDataEntryMgr::SeasonDailyDataEntryMgr() : minValue(UInt32(-1))
{}

SeasonDailyDataEntryMgr::~SeasonDailyDataEntryMgr()
{}

bool SeasonDailyDataEntryMgr::AddEntry(SeasonDailyDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SeasonDailyDataEntry>::AddEntry(entry)) return false;
	if (minValue > entry->startScore) minValue = entry->startScore;
	return true;
}

ItemRewardVec SeasonDailyDataEntryMgr::GetItemRewardVec(UInt32 matchScore)
{
	class SeasonDailyVistor : public Avalon::DataEntryVisitor<SeasonDailyDataEntry>
	{
	public:
		SeasonDailyVistor(UInt32 matchScore) :m_MatchScore(matchScore) {}

		bool operator()(SeasonDailyDataEntry* dataEntry)
		{
			if (dataEntry->startScore <= m_MatchScore && (dataEntry->endScore >= m_MatchScore || dataEntry->endScore == 0))
			{
				m_Rewards = dataEntry->rewards;
				return false;
			}
			return true;
		}

		ItemRewardVec GetRewards(){ return m_Rewards; }

	private:
		UInt32 m_MatchScore;
		ItemRewardVec		m_Rewards;
	};

	SeasonDailyVistor vistor(matchScore);
	Visit(vistor);

	return vistor.GetRewards();

}
