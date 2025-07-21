/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonRewardDataEntry::GuildDungeonRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonRewardDataEntry::~GuildDungeonRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	rewardType = row.ReadUInt32();
	dungeonType = row.ReadUInt32();
	damageWeight = row.ReadUInt32();
	ownerBonusLimit = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				rewardGroup.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				rewardCount.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				rewardWeight.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				rewardSchemeCnt.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				schemeWeight.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				fixPrice.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				auctionPrice.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				addPirce.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__

	if (rewardGroup.size() != rewardCount.size())
	{
		ErrorLogStream << "rewardGroup Size Not Equal rewardCount Size Id:"<<id<< LogStream::eos;
		return false;
	}

	if (rewardWeight.size() != 0 && rewardGroup.size() != rewardWeight.size())
	{
		ErrorLogStream << "rewardGroup Size Not Equal rewardWeight Size Id:" << id << LogStream::eos;
		return false;
	}

	if (auctionPrice.size() != 0 && auctionPrice.size() != rewardGroup.size())
	{
		ErrorLogStream << "auctionPrice Size Not Equal rewardGroup Size Id:" << id << LogStream::eos;
		return false;
	}

	if (fixPrice.size() != auctionPrice.size() || fixPrice.size() != addPirce.size() || auctionPrice.size() != addPirce.size())
	{
		ErrorLogStream << "fixPrice Size Not Equal auctionPrice Size Id:" << id << LogStream::eos;
		return false;
	}

	if (rewardSchemeCnt.size() != schemeWeight.size())
	{
		ErrorLogStream << "rewardSchemeCnt Size Not Equal schemeWeight Size Id:" << id << LogStream::eos;
		return false;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonRewardDataEntryMgr::AddEntry(GuildDungeonRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->rewardType <= E_GUILD_DUNGEON_REWARD_INVAILD || entry->rewardType >= E_GUILD_DUNGEON_REWARD_MAX)
		return true;

	RewardDetail detail;
	for (UInt32 k = 0; k < entry->rewardGroup.size(); ++k)
	{
		RewardInfo* info = new RewardInfo();
		info->rewardGroup = entry->rewardGroup[k];
		info->rewardCnt = entry->rewardCount[k];

		if (entry->auctionPrice.size() != 0)
		{
			info->auctionPrice = entry->auctionPrice[k];
			info->fixPrice = entry->fixPrice[k];
			info->addPirce = entry->addPirce[k];
		}

		detail.rewardInfo.push_back(info);

		if (entry->rewardWeight.size() != 0)
		{
			detail.rewardGroupWeight += entry->rewardWeight[k];
			detail.rewardInfoMap[detail.rewardGroupWeight] = info;
		}
	}

	for (UInt32 k = 0; k < entry->rewardSchemeCnt.size(); ++k)
	{
		detail.rewardSchemeWeight += entry->schemeWeight[k];
		detail.rewardSchemeMap[detail.rewardSchemeWeight] = entry->rewardSchemeCnt[k];
	}

	detail.ownerBonusLimit = entry->ownerBonusLimit;
	RewardTypeGroup rewardtg;
	rewardtg.rewardType = entry->rewardType;
	rewardtg.dungeonType = entry->dungeonType;

	auto iter = m_RewardMap.find(rewardtg);
	if (iter == m_RewardMap.end())
	{
		std::map<UInt32, RewardDetail> tempMap;
		tempMap[entry->damageWeight] = detail;

		m_RewardMap[rewardtg] = tempMap;
	}
	else
	{
		iter->second[entry->damageWeight] = detail;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

RewardDetail* GuildDungeonRewardDataEntryMgr::GetRewardDetail(UInt32 rewardType, UInt32 dungeonType, UInt32 damageRate)
{
	RewardTypeGroup rewardtg;
	rewardtg.rewardType = rewardType;
	rewardtg.dungeonType = dungeonType;

	auto iter = m_RewardMap.find(rewardtg);
	if (iter == m_RewardMap.end())
		return NULL;

	auto it = iter->second.lower_bound(damageRate);
	if (it == iter->second.end())
		return NULL;

	return &(it->second);
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

