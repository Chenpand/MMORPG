#include "CLLevelAdapterDataEntry.h"

bool LevelAdapterDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		values[i] = row.ReadUInt32();
	}

	return true;
}

UInt32 LevelAdapterDataEntryMgr::GetValue(UInt32 id, UInt16 level)
{
	auto entry = FindEntry(id);
	if (entry == NULL || level >= MAX_LEVEL)
	{
		return 0;
	}

	return entry->values[level - 1];
}