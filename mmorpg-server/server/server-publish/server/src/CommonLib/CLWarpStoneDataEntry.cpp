#include "CLWarpStoneDataEntry.h"


EnergyStoneDataEntry::EnergyStoneDataEntry()
{
}

EnergyStoneDataEntry::~EnergyStoneDataEntry()
{
}

bool EnergyStoneDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	energyNum = row.ReadUInt32();
	type = (WarpStoneType)row.ReadUInt8();
	itemId = row.ReadUInt32();

	return true;
}

WarpStoneLevelDataEntry::WarpStoneLevelDataEntry()
{
}

WarpStoneLevelDataEntry::~WarpStoneLevelDataEntry()
{
}

bool WarpStoneLevelDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt32();
	energy = row.ReadUInt32();
	return true;
}

WarpStoneLevelDataEntryMgr::WarpStoneLevelDataEntryMgr()
{
}

WarpStoneLevelDataEntryMgr::~WarpStoneLevelDataEntryMgr()
{
}

bool WarpStoneLevelDataEntryMgr::AddEntry(WarpStoneLevelDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WarpStoneLevelDataEntry>::AddEntry(entry)) return false;

	auto sumEnergy = GetSumEnergy(entry->level - 1);
	sumEnergy += entry->energy;
	m_SumEnergyMap.insert(std::make_pair(entry->level, sumEnergy));
	return true;
}

UInt32 WarpStoneLevelDataEntryMgr::GetSumEnergy(UInt32 level)
{
	SumEnergyMap::iterator iter = m_SumEnergyMap.find(level);
	if (iter != m_SumEnergyMap.end())
	{
		return iter->second;
	}
	return 0;
}

WarpStoneDataEntry::WarpStoneDataEntry()
{
}

WarpStoneDataEntry::~WarpStoneDataEntry()
{
}

bool WarpStoneDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	name = row.ReadString();
	type = (WarpStoneType)row.ReadUInt8();
	level = row.ReadUInt16();
	gold = row.ReadInt32();
	isUnlock = (row.ReadUInt32() != 0);//(bool)row.ReadUInt32();
	return true;
}


WarpStoneDataEntryMgr::WarpStoneDataEntryMgr()
{
}

WarpStoneDataEntryMgr::~WarpStoneDataEntryMgr()
{
}

bool WarpStoneDataEntryMgr::AddEntry(WarpStoneDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WarpStoneDataEntry>::AddEntry(entry)) return false;

	if (entry->level != 0)
	{
		m_LvlWarpStones[entry->level].push_back(entry);
	}

	return true;
}

const static WarpStoneDataEntryMgr::WarpStoneDataVec NullWarpStoneDataVec;

const WarpStoneDataEntryMgr::WarpStoneDataVec& WarpStoneDataEntryMgr::GetLvlWarpStones(UInt16 level) const
{
	WarpStoneDataMap::const_iterator iter = m_LvlWarpStones.find(level);
	if (iter != m_LvlWarpStones.end())
	{
		return iter->second;
	}
	return NullWarpStoneDataVec;
}
