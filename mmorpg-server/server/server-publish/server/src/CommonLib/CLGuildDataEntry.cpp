#include "CLGuildDataEntry.h"

bool GuildDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt8();
	memberNum = row.ReadUInt32();
	buildingMaxLevel[0] = level;
	for (size_t i = 1; i < sizeof(buildingMaxLevel) / sizeof(buildingMaxLevel[0]); i++)
	{
		buildingMaxLevel[i] = row.ReadUInt8();
	}

	return true;
}