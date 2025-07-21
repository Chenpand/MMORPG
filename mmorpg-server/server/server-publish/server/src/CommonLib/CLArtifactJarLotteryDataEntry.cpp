/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLArtifactJarLotteryDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ArtifactJarLotteryDataEntry::ArtifactJarLotteryDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ArtifactJarLotteryDataEntry::~ArtifactJarLotteryDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ArtifactJarLotteryDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ArtifactJarLotteryDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	jarId = row.ReadUInt32();
	rewardItemId = row.ReadUInt32();
	rewardItemNum = row.ReadUInt32();
	weight = row.ReadUInt32();
	limitNum = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ArtifactJarLotteryDataEntryMgr::AddEntry(ArtifactJarLotteryDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ArtifactJarLotteryDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	auto iter = m_ArtifactJarLotteryCfgMap.find(entry->jarId);
	if (iter == m_ArtifactJarLotteryCfgMap.end())
	{
		ArtifactJarLotteryPool tempMap;
		tempMap[entry->id] = (entry);
		m_ArtifactJarLotteryCfgMap[entry->jarId] = tempMap;
	}
	else
	{
		iter->second[entry->id] = (entry);
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

ArtifactJarLotteryPool ArtifactJarLotteryDataEntryMgr::GetArtifactJarRewardPool(UInt32 jarId)
{
	auto iter = m_ArtifactJarLotteryCfgMap.find(jarId);
	if (iter == m_ArtifactJarLotteryCfgMap.end())
		return ArtifactJarLotteryPool();

	return iter->second;
}

void ArtifactJarLotteryDataEntryMgr::GetArtifactJarLotteryCfg(std::vector<CLProtocol::ArtifactJarLotteryCfg>& cfgVec)
{
	for (auto& iter : m_ArtifactJarLotteryCfgMap)
	{
		CLProtocol::ArtifactJarLotteryCfg cfg;
		cfg.jarId = iter.first;

		for (auto& it : iter.second)
		{
			ItemReward reward;
			reward.id = it.second->rewardItemId;
			reward.num = it.second->rewardItemNum;
			cfg.rewardVec.push_back(reward);
		}

		cfgVec.push_back(cfg);
	}
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

