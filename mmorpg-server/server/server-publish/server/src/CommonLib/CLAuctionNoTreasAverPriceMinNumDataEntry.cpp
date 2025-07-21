#include"CLAuctionNoTreasAverPriceMinNumDataEntry.h"

bool AuctionNoTreasAverPriceMinNumDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	minNum = row.ReadUInt32();

	return true;
}