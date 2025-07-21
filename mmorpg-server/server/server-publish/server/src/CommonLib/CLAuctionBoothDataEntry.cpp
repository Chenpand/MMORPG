#include "CLAuctionBoothDataEntry.h"

bool AuctionBoothDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	costItemId = row.ReadUInt32();
	num = row.ReadUInt32();

	return true;
}