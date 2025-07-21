#include "CLDungeonTimeDataEntry.h"

bool DungeonFinishTimeDataEntry::Read(Avalon::DataRow& row)
{
	dungeonId = row.ReadUInt32();
	floor = row.ReadUInt16();
	minTime_29 = row.ReadUInt32();
	minTime_39 = row.ReadUInt32();
	minTime_49 = row.ReadUInt32();
	minTime_59 = row.ReadUInt32();
	minTime_69 = row.ReadUInt32();

	return true;
}