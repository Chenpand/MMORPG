#include "CLDungeonMatchDataEntry.h"

bool DungeonMatchDataEntry::Read(Avalon::DataRow& row)
{
	dungeonId = row.ReadUInt32();
	fullPlayerNum = row.ReadUInt32();
	return true;
}