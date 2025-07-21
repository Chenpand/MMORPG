#include "CLGuildBuildingDataEntry.h"

bool GuildBuildingDataEntry::Read(Avalon::DataRow& row)
{
	level = row.ReadUInt8();
	for (size_t i = 0; i < sizeof(buildingCost) / sizeof(buildingCost[0]); i++)
	{
		buildingCost[i] = row.ReadUInt32();
	}
	shopId = row.ReadUInt8();
	welfareGiftId = row.ReadUInt32();

	return true;
}