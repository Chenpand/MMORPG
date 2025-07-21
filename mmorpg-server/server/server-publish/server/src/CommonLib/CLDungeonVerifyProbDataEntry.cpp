#include "CLDungeonVerifyProbDataEntry.h"

bool DungeonVerifyProbDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	prob = row.ReadUInt32();
	return true;
}