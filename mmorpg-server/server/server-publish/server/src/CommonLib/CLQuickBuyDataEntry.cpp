#include "CLQuickBuyDataEntry.h"

bool QuickBuyDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadInt32();
	costItemId = row.ReadInt32();
	costNum = row.ReadInt32();
	multiple = row.ReadUInt8();
	return true;
}