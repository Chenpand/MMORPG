#include "CLGuildStatueDataEntry.h"

bool GuildStatueDataEntry::Read(Avalon::DataRow& row)
{
	type = row.ReadUInt8();
	costItemId = row.ReadUInt32();
	costItemNum = row.ReadUInt32();
	awardContri = row.ReadUInt32();
	bufferId = row.ReadUInt32();

	if (costItemId != 600000007 && costItemId != 600000008)
	{
		return false;
	}

	return true;
}