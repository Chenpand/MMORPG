#include "CLChampionDataEntry.h"
#include "TimeUtil.h"



ChampionTimeDataEntry::ChampionTimeDataEntry()
{

}

ChampionTimeDataEntry::~ChampionTimeDataEntry()
{

}

UInt32 ChampionTimeDataEntry::GetKey() const
{
	return id;
}

bool ChampionTimeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	status = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(startTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(endTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(preStartTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(prepareStartTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}


	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(battleStartTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(battleEndTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}
	return true;
}



bool ChampionTimeDataEntryMgr::AddEntry(ChampionTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChampionTimeDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}





ChampionRewardDataEntry::ChampionRewardDataEntry()
{

}

ChampionRewardDataEntry::~ChampionRewardDataEntry()
{

}

UInt32 ChampionRewardDataEntry::GetKey() const
{
	return id;
}

bool ChampionRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	playerState = row.ReadUInt32();
	championState = row.ReadUInt32();
	{
		auto str = row.ReadString();
		std::string split1 = "|";
		std::string split2 = "_";
		std::vector<std::string> stdRewardVec;
		Avalon::StringUtil::Split(str, stdRewardVec, split1);
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
			else
			{
				continue;
			}
			itemRewards.push_back(reward);
		}
	}

	title = row.ReadUInt32();
	sender = row.ReadUInt32();
	content = row.ReadUInt32();

	return true;
}



bool ChampionRewardDataEntryMgr::AddEntry(ChampionRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChampionRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}
	m_ChampionStateRewardMap[entry->championState].push_back(entry);
	return true;
}

std::vector<ChampionRewardDataEntry*> ChampionRewardDataEntryMgr::GetChampionStateReward(UInt32 state)
{
	if (m_ChampionStateRewardMap.count(state) == 0)
	{
		return {};
	}
	return m_ChampionStateRewardMap[state];
}



ChampionGambleDataEntry::ChampionGambleDataEntry()
{

}

ChampionGambleDataEntry::~ChampionGambleDataEntry()
{

}

UInt32 ChampionGambleDataEntry::GetKey() const
{
	return id;
}

bool ChampionGambleDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(startTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(endTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	state = row.ReadUInt32();


	groupStart = row.ReadUInt32();
	groupEnd = row.ReadUInt32();
	return true;
}



bool ChampionGambleDataEntryMgr::AddEntry(ChampionGambleDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChampionGambleDataEntry>::AddEntry(entry))
	{
		return false;
	}
	m_ProjectDataMap[entry->type][entry->state] = entry;
	return true;
}

ChampionGambleDataEntry * ChampionGambleDataEntryMgr::GetDataEntryByStateAndType(UInt32 state, UInt32 type)
{
	auto mapIter = m_ProjectDataMap.find(type);
	if (mapIter == m_ProjectDataMap.end())
	{
		return nullptr;
	}
	auto dataIter = mapIter->second.find(state);
	if (dataIter == mapIter->second.end())
	{
		return nullptr;
	}
	return dataIter->second;
}
