/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/

#include "CLUltimateChallengeRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

UltimateChallengeRewardDataEntry::UltimateChallengeRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

UltimateChallengeRewardDataEntry::~UltimateChallengeRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 UltimateChallengeRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool UltimateChallengeRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	minClearDays = row.ReadUInt32();
	maxClearDays = row.ReadUInt32();
	rewardItem = row.ReadString();
// __CUSTOM_READ_BEGIN__
	std::string split1 = "|";
	std::string split2 = "_";
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(rewardItem, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else if (strTmp.size() == 4)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.equipType = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
			reward.strenth = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[3]);
		}
		else
		{
			continue;
		}
		rewards.push_back(reward);
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool UltimateChallengeRewardDataEntryMgr::AddEntry(UltimateChallengeRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<UltimateChallengeRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
const UltimateChallengeRewardDataEntry* UltimateChallengeRewardDataEntryMgr::GetReward(UInt32 days)
{
	class UltimateChallengeRewardDataEntryVistor : public Avalon::DataEntryVisitor<UltimateChallengeRewardDataEntry>
	{
	public:
		UltimateChallengeRewardDataEntryVistor(UInt32 days) : days_(days), data(NULL) {}
		bool operator()(UltimateChallengeRewardDataEntry* dataEntry)
		{
			if (dataEntry->minClearDays <= days_ && days_ <= dataEntry->maxClearDays)
			{
				data = dataEntry;
				return false;
			}
			return true;
		}

		UInt32 days_;
		UltimateChallengeRewardDataEntry* data;
	};

	UltimateChallengeRewardDataEntryVistor vistor(days);
	Visit(vistor);

	return vistor.data;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

