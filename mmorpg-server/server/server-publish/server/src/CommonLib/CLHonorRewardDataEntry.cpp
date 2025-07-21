/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLHonorRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

HonorRewardDataEntry::HonorRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

HonorRewardDataEntry::~HonorRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 HonorRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool HonorRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	pvpType = row.ReadUInt32();
	rewardType = row.ReadString();
	victoryReward = row.ReadUInt32();
	lostReward = row.ReadUInt32();
	rankReward = row.ReadString();
	killReward = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__

	{
		std::vector<std::string> param;
		Avalon::StringUtil::Split(rewardType, param, "|");
		for (auto& iter : param)
		{
			UInt32 tt = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
			rewardTypeVec.push_back(tt);
		}
	}

	{
		std::vector<std::string> param_1;
		Avalon::StringUtil::Split(rankReward, param_1, "|");
		for (auto& iter : param_1)
		{
			std::vector<std::string> param_2;
			Avalon::StringUtil::Split(iter, param_2, "_");

			if (param_2.size() != 3)
			{
				ErrorLogStream << "honor reward Error!" << rankReward << LogStream::eos;
				return false;
			}

			RankRewardCfg rewardCfg;
			rewardCfg.minRank = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[0]);
			rewardCfg.maxRank = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[1]);
			rewardCfg.honor = Avalon::StringUtil::ConvertToValue<UInt32>(param_2[2]);
			rankRewardVec.emplace_back(rewardCfg);
		}
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool HonorRewardDataEntryMgr::AddEntry(HonorRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<HonorRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_HonorRewardMap[entry->pvpType] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const HonorRewardDataEntry* HonorRewardDataEntryMgr::GetHonorRewardDataEntry(UInt32 pvpType)
{
	auto iter = m_HonorRewardMap.find(pvpType);
	return iter != m_HonorRewardMap.end() ? iter->second : NULL;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

