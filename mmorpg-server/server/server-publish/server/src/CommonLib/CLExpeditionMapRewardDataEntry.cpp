/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLExpeditionMapRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLJobDataEntry.h"
// __CUSTOM_HEADER_END__

ExpeditionMapRewardDataEntry::ExpeditionMapRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ExpeditionMapRewardDataEntry::~ExpeditionMapRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ExpeditionMapRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ExpeditionMapRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	expeditionMapId = row.ReadUInt32();
	rewards = row.ReadString();
	requireAnyOccu = row.ReadUInt32();
	requireAnySameBaseOccu = row.ReadUInt32();
	requireAnyDiffBaseOccu = row.ReadUInt32();
	requireAnyDiffChangedOccu = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
	std::vector<std::string> tmpVec;
	Avalon::StringUtil::Split(rewards, tmpVec, "|");
	for (const auto& elem : tmpVec)
	{
		std::vector<std::string> tmpVec2;
		Avalon::StringUtil::Split(elem, tmpVec2, ":");
		if (tmpVec2.size() != 2)
		{
			ErrorLogStream << "Rewards format is error!" << LogStream::eos;
			return false;
		}
		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(tmpVec2[1]);
		rewardVec.push_back(reward);
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
bool ExpeditionMapRewardDataEntry::GetExpeditionRewards(const std::vector<UInt8> occus, ItemRewardVec& expeditonRewards)
{
	if (requireAnyOccu > 0)
	{
		if (occus.size() < requireAnyOccu)
		{
			return false;
		}
	}

	if (requireAnySameBaseOccu > 0)
	{
		bool meetRequirement = false;
		std::map<Occupation, UInt32> baseOccuCount;
		for (auto occu : occus)
		{
			Occupation baseOccu = JobDataEntry::GetBaseOccu(occu);
			baseOccuCount[baseOccu] += 1;
			if (baseOccuCount[baseOccu] >= requireAnySameBaseOccu)
			{
				meetRequirement = true;
				break;
			}
		}

		if (!meetRequirement) return false;
	}

	if (requireAnyDiffBaseOccu > 0)
	{
		bool meetRequirement = false;
		std::set<Occupation> diffBaseOccus;
		for (auto occu : occus)
		{
			Occupation baseOccu = JobDataEntry::GetBaseOccu(occu);
			diffBaseOccus.insert(baseOccu);
			if (diffBaseOccus.size() >= requireAnyDiffBaseOccu)
			{
				meetRequirement = true;
				break;
			}
		}

		if (!meetRequirement) return false;
	}

	if (requireAnyDiffChangedOccu > 0)
	{
		bool meetRequirement = false;
		std::set<UInt8> diffOccus;
		for (auto occu : occus)
		{
			Occupation baseOccu = JobDataEntry::GetBaseOccu(occu);
			if (occu == (UInt8)baseOccu)
			{
				continue;
			}
			
			diffOccus.insert(occu);
			if (diffOccus.size() >= requireAnyDiffChangedOccu)
			{
				meetRequirement = true;
				break;
			}
		}

		if (!meetRequirement) return false;
	}

	expeditonRewards.insert(expeditonRewards.end(), rewardVec.begin(), rewardVec.end());
	return true;
}
// __CUSTOM_ENTRYFUNCTION_END__

bool ExpeditionMapRewardDataEntryMgr::AddEntry(ExpeditionMapRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ExpeditionMapRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
void ExpeditionMapRewardDataEntryMgr::GetExpeditionRewards(UInt8 mapId, const std::vector<UInt8> occus, ItemRewardVec& expeditonRewards, std::vector<UInt32>& rewardsDataIds)
{
	class ExpeditionMapRewardDataEntryVisitor :public Avalon::DataEntryVisitor<ExpeditionMapRewardDataEntry>
	{
	public:
		ExpeditionMapRewardDataEntryVisitor(UInt32 mapId, const std::vector<UInt8> occus) 
			:m_MapId(mapId),m_Occus(occus)
		{}

		bool operator()(ExpeditionMapRewardDataEntry* entry)
		{
			if (entry != NULL && entry->expeditionMapId == m_MapId)
			{
				if (entry->GetExpeditionRewards(m_Occus, rewards))
				{
					rewardDataIdsVec.push_back(entry->id);
				}
			}

			return true;
		}

	public:
		ItemRewardVec rewards;
		std::vector<UInt32> rewardDataIdsVec;

	private:
		UInt32 m_MapId;
		std::vector<UInt8> m_Occus;
	};

	ExpeditionMapRewardDataEntryVisitor visitor(mapId, occus);
	Visit(visitor);

	expeditonRewards = visitor.rewards;
	rewardsDataIds = visitor.rewardDataIdsVec;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

