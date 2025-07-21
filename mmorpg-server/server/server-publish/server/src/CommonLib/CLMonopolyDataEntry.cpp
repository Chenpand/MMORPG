#include "CLMonopolyDataEntry.h"




void MonopolyGenItemRewardVec(std::string str, ItemRewardVec& rewards)
{
	if (str.empty())
	{
		return;
	}

	std::string split1 = ",";
	std::string split2 = "_";
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(str, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2 || strTmp.size() == 3)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else
		{
			continue;
		}
		rewards.push_back(reward);
	}
}


MonopolyCardDataEntry::MonopolyCardDataEntry()
{

}

MonopolyCardDataEntry::~MonopolyCardDataEntry()
{

}

UInt32 MonopolyCardDataEntry::GetKey() const
{
	return card;
}

bool MonopolyCardDataEntry::Read(Avalon::DataRow &row)
{
	card = row.ReadUInt32();
	coinNum = row.ReadUInt32();

	return true;
}



bool MonopolyCardDataEntryMgr::AddEntry(MonopolyCardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MonopolyCardDataEntry>::AddEntry(entry))
	{
		return false;
	}
	cardIDVec.push_back(entry->card);
	return true;
}




MonopolyCardRewardDataEntry::MonopolyCardRewardDataEntry()
{


}

MonopolyCardRewardDataEntry::~MonopolyCardRewardDataEntry()
{


}

UInt32 MonopolyCardRewardDataEntry::GetKey() const
{
	return id;

}

bool MonopolyCardRewardDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();

	std::string str;
	{
		str = row.ReadString();
		MonopolyGenItemRewardVec(str, costCard);
	}

	{
		str = row.ReadString();
		MonopolyGenItemRewardVec(str, reward);
	}

	limit = row.ReadUInt32();
	type = row.ReadUInt32();
	return true;
}



bool MonopolyCardRewardDataEntryMgr::AddEntry(MonopolyCardRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MonopolyCardRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}


	return true;
}



MonopolyRandomDataEntry::MonopolyRandomDataEntry()
{


}

MonopolyRandomDataEntry::~MonopolyRandomDataEntry()
{


}

UInt32 MonopolyRandomDataEntry::GetKey() const
{
	return id;

}

bool MonopolyRandomDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	gridType = row.ReadUInt32();
	type = row.ReadUInt32();
	param = row.ReadUInt32();
	num = row.ReadUInt32();
	weight = row.ReadUInt32();
	randomType = row.ReadUInt32();

	return true;
}


bool MonopolyRandomDataEntryMgr::AddEntry(MonopolyRandomDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MonopolyRandomDataEntry>::AddEntry(entry))
	{
		return false;
	}
	if (m_GridMap.count(entry->gridType) == 0)
	{
		m_GridMap[entry->gridType].randomType = entry->randomType;
	}

	auto gridIter = m_GridMap.find(entry->gridType);
	if (gridIter == m_GridMap.end())
	{
		return false;
	}

	auto& grid = gridIter->second;
	MonopolyGridEventDataEntry event;
	event.id = entry->id;
	event.eventType = entry->type;
	event.param = entry->param;
	event.weight = entry->weight;
	event.num = entry->num;
	grid.events.push_back(event);
	grid.totalWeight += event.weight;

	return true;
}

MonopolyGridDataEntry* MonopolyRandomDataEntryMgr::GetGridData(UInt32 gridType)
{
	auto gridIter = m_GridMap.find(gridType);
	if (gridIter == m_GridMap.end())
	{
		return nullptr;
	}
	return &(gridIter->second);
}


MonopolyMapDataEntry::MonopolyMapDataEntry()
{


}

MonopolyMapDataEntry::~MonopolyMapDataEntry()
{


}

UInt32 MonopolyMapDataEntry::GetKey() const
{
	return turn;

}

bool MonopolyMapDataEntry::Read(Avalon::DataRow &row)
{
	turn = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				if (value > 0) content.push_back(value);
			}
		}
	}
	return true;
}

UInt32 MonopolyMapDataEntry::GetMapGridContent(UInt32 grid) const
{
	if (grid >= content.size())
	{
		return 0;
	}
	return content[grid];
}

bool MonopolyMapDataEntryMgr::AddEntry(MonopolyMapDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MonopolyMapDataEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->turn > m_MaxTurn)
	{
		m_MaxTurn = entry->turn;
	}

	return true;
}

UInt32 MonopolyMapDataEntryMgr::GetMapGridContent(UInt32 turn, UInt32 grid)
{
	auto data = FindEntry(turn);
	if (data == nullptr)
	{
		return 0;
	}
	return data->GetMapGridContent(grid);
}




