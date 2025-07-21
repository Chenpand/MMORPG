#include "CLAdventurePassDataEntry.h"
#include "TimeUtil.h"
#include "AvalonDataRow.h"
#include "AvalonDataTable.h"



void GenItemRewardVec(std::string str,ItemRewardVec& rewards)
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
		if (strTmp.size() == 2|| strTmp.size() == 3)
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

AdventurePassRewardDataEntry::AdventurePassRewardDataEntry()
{

}

AdventurePassRewardDataEntry::~AdventurePassRewardDataEntry()
{

}


bool AdventurePassRewardDataEntry::Read(Avalon::DataRow &row)
{
	key.season = row.ReadUInt32();
	key.lv = row.ReadUInt32();
	normal = row.ReadString();
	high = row.ReadString();
	exp = row.ReadUInt32();

	GenItemRewardVec(normal,GetNormalReward());
	GenItemRewardVec(high, GetHighReward());

	return true;
}

ItemRewardVec & AdventurePassRewardDataEntry::GetNormalReward()
{
	return m_NormalReward;
}

ItemRewardVec & AdventurePassRewardDataEntry::GetHighReward()
{
	return m_HighReward;
}


bool AdventurePassRewardDataEntryMgr::AddEntry(AdventurePassRewardDataEntry* entry)
{
	if (m_PassReward.find(entry->key) != m_PassReward.end())
	{
		return false;
	}
	m_PassReward[entry->key] = entry;

	return true;
}


bool AdventurePassRewardDataEntryMgr::ReloadData(const std::string& filename)
{
	Avalon::DataTable table;
	if (!table.LoadFile(filename))
	{
		std::stringstream ss;
		ss << "reload table(" << filename << ") failed.";
		throw ss.str();
		return false;
	}

	Avalon::DataRow row;

	while (table.GetNextRow(row))
	{
		AdventurePassRewardDataEntry* entry = new AdventurePassRewardDataEntry();
		if (!entry->Read(row))
		{
			std::stringstream ss;
			ss << "reload read table(" << filename << ") key(" << entry->GetKey() << ") failed.";
			delete entry;
			throw ss.str();
			return false;
		}
		else
		{
			if (m_PassReward.find(entry->key) != m_PassReward.end())
			{
				return false;
			}
			m_PassReward[entry->key] = entry;
		}
	}

	return true;
}



AdventurePassRewardDataEntry* AdventurePassRewardDataEntryMgr::FindEntry(UInt32 season, UInt32 lv) const
{
	auto iter = m_PassReward.find({ season,lv });
	if (iter != m_PassReward.end())
	{
		return iter->second;
	}
	return nullptr;
}

AdventurePassActivityDataEntry::AdventurePassActivityDataEntry()
{

}

AdventurePassActivityDataEntry::~AdventurePassActivityDataEntry()
{

}

UInt32 AdventurePassActivityDataEntry::GetKey() const
{
	return id;
}

bool AdventurePassActivityDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	activity = row.ReadUInt32();
	exp = row.ReadUInt32();


	return true;
}


bool AdventurePassActivityDataEntryMgr::AddEntry(AdventurePassActivityDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventurePassActivityDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

UInt32 AdventurePassActivityDataEntryMgr::GetAddExp(UInt32 oldActivity, UInt32 newActivity)
{
	class Visitor :public Avalon::DataEntryVisitor<AdventurePassActivityDataEntry>
	{
	public:
		Visitor(UInt32 oldActivity, UInt32 newActivity) : m_OldActivity(oldActivity),m_NewActivity(newActivity), exp(0) {};

		bool operator()(AdventurePassActivityDataEntry* entry)
		{
			if (m_OldActivity < entry->activity && m_NewActivity >= entry->activity)
			{
				exp += entry->exp;
			}
			return true;
		}
	private:
		UInt32 m_OldActivity;
		UInt32 m_NewActivity;
	public:
		UInt32 exp;
	};

	Visitor vis{ oldActivity, newActivity };
	AdventurePassActivityDataEntryMgr::Instance()->Visit(vis);

	return vis.exp;
}




AdventurePassBuyLevelDataEntry::AdventurePassBuyLevelDataEntry()
{

}

AdventurePassBuyLevelDataEntry::~AdventurePassBuyLevelDataEntry()
{

}

UInt32 AdventurePassBuyLevelDataEntry::GetKey() const
{
	return buyLv;
}

bool AdventurePassBuyLevelDataEntry::Read(Avalon::DataRow &row)
{
	buyLv = row.ReadUInt32();
	pointCost = row.ReadUInt32();


	return true;
}



bool AdventurePassBuyLevelDataEntryMgr::AddEntry(AdventurePassBuyLevelDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventurePassBuyLevelDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}




AdventurePassBuyRewardDataEntry::AdventurePassBuyRewardDataEntry()
{

}

AdventurePassBuyRewardDataEntry::~AdventurePassBuyRewardDataEntry()
{

}

UInt32 AdventurePassBuyRewardDataEntry::GetKey() const
{
	return season;
}

bool AdventurePassBuyRewardDataEntry::Read(Avalon::DataRow &row)
{
	season = row.ReadUInt32();
	kingLevel = row.ReadUInt32();
	kingRewardStr = row.ReadString();
	topKingLevel = row.ReadUInt32();
	topKingRewardStr = row.ReadString();
	kingPrice = row.ReadUInt32();
	topKingPrice = row.ReadUInt32();
	buyType = row.ReadUInt32();

	GenItemRewardVec(kingRewardStr, kingReward);
	GenItemRewardVec(topKingRewardStr, topKingReward);

	return true;
}

UInt32 AdventurePassBuyRewardDataEntry::GetPrice(UInt8 type)
{
	if (type == 1)
	{
		return kingPrice;
	}
	else if (type == 2)
	{
		return topKingPrice;
	}
	return 0;
}

ItemRewardVec & AdventurePassBuyRewardDataEntry::GetReward(UInt8 type)
{
	if (type == 1)
	{
		return kingReward;
	}
	else if (type == 2)
	{
		return topKingReward;
	}
	return nullReward;
}



bool AdventurePassBuyRewardDataEntryMgr::AddEntry(AdventurePassBuyRewardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventurePassBuyRewardDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

UInt32 AdventurePassBuyRewardDataEntryMgr::GetPrice(UInt32 season, UInt8 type)
{
	auto data = FindEntry(season);
	if (data == nullptr)
	{
		return 0;
	}

	if (type == 1)
	{
		return data->kingPrice;
	}
	else if (type == 2)
	{
		return data->topKingPrice;
	}
	return 0;
}

UInt32 AdventurePassBuyRewardDataEntryMgr::GetLevel(UInt32 season, UInt8 type)
{
	auto data = FindEntry(season);
	if (data == nullptr)
	{
		return 0;
	}

	if (type == 1)
	{
		return data->kingLevel;
	}
	else if (type == 2)
	{
		return data->topKingLevel;
	}
	return 0;
}

AdventurePassBuyRewardType AdventurePassBuyRewardDataEntryMgr::GetBuyType(UInt32 season)
{
	auto data = FindEntry(season);
	if (data == nullptr)
	{
		return APBRT_POINT;
	}

	return static_cast<AdventurePassBuyRewardType>(data->buyType);
}

ItemRewardVec & AdventurePassBuyRewardDataEntryMgr::GetReward(UInt32 season, UInt8 type)
{
	auto data = FindEntry(season);
	if (data == nullptr)
	{
		return nullReward;
	}
	if (type == 1)
	{
		return data->kingReward;
	}
	else if (type == 2)
	{
		return data->topKingReward;
	}
	return nullReward;
}



AdventurePassSeasonDataEntry::AdventurePassSeasonDataEntry()
{

}

AdventurePassSeasonDataEntry::~AdventurePassSeasonDataEntry()
{

}

UInt32 AdventurePassSeasonDataEntry::GetKey() const
{
	return id;
}

bool AdventurePassSeasonDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();

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
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	maxLevel = row.ReadUInt32();

	return true;
}


bool AdventurePassSeasonDataEntryMgr::AddEntry(AdventurePassSeasonDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AdventurePassSeasonDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

AdventurePassSeasonDataEntry* AdventurePassSeasonDataEntryMgr::GetSeasonData(Avalon::Time time)
{
	class Visitor :public Avalon::DataEntryVisitor<AdventurePassSeasonDataEntry>
	{
	public:
		Visitor(Avalon::Time time, AdventurePassSeasonDataEntry*& data) : m_Time(time), m_Data(data) {};

		bool operator()(AdventurePassSeasonDataEntry* entry)
		{
			if (m_Time.Sec() <= entry->endTime && m_Time.Sec() >= entry->startTime)
			{
				m_Data = entry;
			}
			return true;
		}
	private:
		Avalon::Time m_Time;
		AdventurePassSeasonDataEntry*& m_Data;
	};

	AdventurePassSeasonDataEntry* data = nullptr;
	Visitor vis{ time,data };
	AdventurePassSeasonDataEntryMgr::Instance()->Visit(vis);
	return data;
}


bool PassRewardKey::operator<(const PassRewardKey & rsh) const
{
	if (season < rsh.season)
	{
		return true;
	}
	else if (season == rsh.season)
	{
		if (lv < rsh.lv)
		{
			return true;
		}
	}
	return false;
}
