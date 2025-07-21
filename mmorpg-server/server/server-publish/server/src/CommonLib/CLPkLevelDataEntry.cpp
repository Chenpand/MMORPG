#include "CLPkLevelDataEntry.h"

PkLevelDataEntry::PkLevelDataEntry()
{

}

PkLevelDataEntry::~PkLevelDataEntry()
{

}

bool PkLevelDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    type = (PkLevelType)row.ReadUInt8();
    level = row.ReadUInt8();
    minPkValue = row.ReadUInt32();
    return true;
}

bool PkLevelDataEntryMgr::AddEntry(PkLevelDataEntry* data)
{
	if (!data)
	{
		return false;
	}

	if (!Avalon::DataEntryMgrBase<PkLevelDataEntry>::AddEntry(data))
	{
		return false;
	}

	PkLevelInfo info;
	info.type = data->type;
	info.level = data->level;

	m_pkLevelInfoes[data->minPkValue] = info;
	return true;
}

bool PkLevelDataEntryMgr::GetLevelByPkValue(UInt32 pkValue, PkLevelType& type, UInt8& level)
{
	for (auto itr = m_pkLevelInfoes.rbegin(); itr != m_pkLevelInfoes.rend(); ++itr)
	{
		if (pkValue >= itr->first)
		{
			type = itr->second.type;
			level = itr->second.level;
			return true;
		}
	}

	return false;
}

UInt32 PkLevelDataEntryMgr::GenRobotPkValue(UInt32 pkValue)
{
	for (auto itr = m_pkLevelInfoes.rbegin(); itr != m_pkLevelInfoes.rend(); ++itr)
	{
		if (pkValue < itr->first)
		{
			continue;
		}

		std::vector<UInt32> pkValues;

		// 上一级
		auto next = itr;
		next++;
		auto end = m_pkLevelInfoes.rend();
		if (next != end)
		{
			pkValues.push_back(next->first);
		}

		// 当前级
		pkValues.push_back(itr->first);

		// 下一级
		if (itr != m_pkLevelInfoes.rbegin())
		{
			auto prev = itr;
			prev--;
			pkValues.push_back(prev->first);
		}

		if (pkValues.empty())
		{
			return pkValue;
		}

		return pkValues[Avalon::Random::RandBetween(0, pkValues.size() - 1)];
	}

	return pkValue;
}