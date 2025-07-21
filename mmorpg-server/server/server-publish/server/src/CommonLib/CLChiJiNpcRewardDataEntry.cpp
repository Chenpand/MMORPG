/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiNpcRewardDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiNpcRewardDataEntry::ChiJiNpcRewardDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiNpcRewardDataEntry::~ChiJiNpcRewardDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiNpcRewardDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiNpcRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	npcId = row.ReadUInt32();
	param = row.ReadUInt32();
	param2 = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				rewards.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiNpcRewardDataEntryMgr::AddEntry(ChiJiNpcRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiNpcRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	auto iter = m_NpcRewards.find(entry->npcId);
	if (iter == m_NpcRewards.end())
	{
		std::map<UInt32, ChiJiNpcRewardDataEntry*> tempMap;
		tempMap[entry->param] = entry;
		m_NpcRewards[entry->npcId] = tempMap;
	}
	else
	{
		iter->second[entry->param] = entry;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 ChiJiNpcRewardDataEntryMgr::GetRewards(UInt32 npcId, UInt32 param)
{
	const ChiJiNpcRewardDataEntry* entry = GetChiJiNpcRewardDataEntry(npcId, param);
	if (NULL == entry)
	{
		ErrorLogStream << "not find chiji npc reward config id:"<< npcId << " param:"<< param << LogStream::eos;
		return 0;
	}

	if (entry->rewards.empty())
	{
		return 0;
	}

	ChiJiNpcRewardDataEntry* tempEntry = const_cast<ChiJiNpcRewardDataEntry*>(entry);
	std::random_shuffle(tempEntry->rewards.begin(), tempEntry->rewards.end());

	return entry->rewards[0];
}

const ChiJiNpcRewardDataEntry* ChiJiNpcRewardDataEntryMgr::GetChiJiNpcRewardDataEntry(UInt32 npcId, UInt32 param)
{
	auto iter = m_NpcRewards.find(npcId);
	if (iter == m_NpcRewards.end())
	{
		return NULL;
	}

	if (iter->second.size() == 1)
	{
		return iter->second.begin()->second;
	}

	for (auto& tt : iter->second)
	{
		if (param <= tt.first)
		{
			return tt.second;
		}
	}

	auto tmpIt = iter->second.end();
	tmpIt--;
	return tmpIt->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

