#include "CLDungeonChestDataEntry.h"

DungeonChestDataEntry::DungeonChestDataEntry()
{
	score = 0;
}

DungeonChestDataEntry::~DungeonChestDataEntry()
{

}

bool DungeonChestDataEntry::Read(Avalon::DataRow& row)
{
    dungeonId = row.ReadUInt32();
	score = row.ReadUInt32();
    goldDropSetId = row.ReadUInt32();
    for (int i = 0; i < DCT_NUM; i++)
    {
        chestDropoSetIds[i] = row.ReadUInt32();
    }
	payChestCostItemId = row.ReadUInt32();
    payChestCost = row.ReadUInt32();

    return true;
}

DungeonChestDataEntry* DungeonChestDataEntryMgr::FindData(UInt32 dungeonId, UInt32 score)
{
	{
		UInt64 key = (static_cast<UInt64>(dungeonId) << 32) + static_cast < UInt64>(0);
		auto iter = m_Datas.find(key);
		if (iter != m_Datas.end())
		{
			return iter->second;
		}
	}
	{
		UInt64 key = (static_cast<UInt64>(dungeonId) << 32) + static_cast <UInt64>(score);
		auto iter = m_Datas.find(key);
		if (iter != m_Datas.end())
		{
			return iter->second;
		}
		return nullptr;
	}
}

bool DungeonChestDataEntryMgr::AddEntry(DungeonChestDataEntry* entry)
{
	UInt64 key = (static_cast<UInt64>(entry->dungeonId) << 32) + static_cast<UInt64>(entry->score);
	auto iter = m_Datas.find(key);
	if (iter != m_Datas.end())
	{
		return false;
	}
	m_Datas[key] = entry;
	return true;
}

DungeonChestDataEntryMgr::~DungeonChestDataEntryMgr()
{
	for (auto& i : m_Datas)
	{
		CL_SAFE_DELETE(i.second);
	}
	m_Datas.clear();
}
