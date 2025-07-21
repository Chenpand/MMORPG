#include "CLMonthSignAwardDataEntry.h"


MonthSignAwardDataEntry::MonthSignAwardDataEntry()
{

}

MonthSignAwardDataEntry::~MonthSignAwardDataEntry()
{

}

UInt32 MonthSignAwardDataEntry::GetKey() const
{
	return 0;
}

bool MonthSignAwardDataEntry::Read(Avalon::DataRow &row)
{
	key.month = row.ReadUInt8();
	key.month = row.ReadUInt8();
	key.day = row.ReadUInt8();
	vipDouble = row.ReadUInt8();
	itemID = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	return true;
}


bool MonthSignAwardDataEntryMgr::AddEntry(MonthSignAwardDataEntry* entry)
{
	m_MonthSignMap.insert(std::make_pair(entry->key, entry));
	return true;
}



MonthSignAwardDataEntry* MonthSignAwardDataEntryMgr::FindEntry(UInt8 month, UInt8 day) const
{
	auto data = m_MonthSignMap.find(MonthSignKey{ month, day });
	if (data != m_MonthSignMap.cend())
	{
		return data->second;
	}
	return nullptr;
}

MonthSignCollectAwardDataEntry::MonthSignCollectAwardDataEntry()
{

}

MonthSignCollectAwardDataEntry::~MonthSignCollectAwardDataEntry()
{

}

UInt32 MonthSignCollectAwardDataEntry::GetKey() const
{
	return collectDay;
}

bool MonthSignCollectAwardDataEntry::Read(Avalon::DataRow &row)
{
	collectDay = row.ReadUInt8();
	itemID = row.ReadUInt32();
	itemNum = row.ReadUInt32();


	return true;
}

bool MonthSignCollectAwardDataEntryMgr::AddEntry(MonthSignCollectAwardDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<MonthSignCollectAwardDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}


bool MonthSignKey::operator<(const MonthSignKey &rsh) const
{
	if (month < rsh.month)
	{
		return true;
	}
	else if (month == rsh.month)
	{
		if (day < rsh.day)
		{
			return true;
		}
	}
	return false;
}
