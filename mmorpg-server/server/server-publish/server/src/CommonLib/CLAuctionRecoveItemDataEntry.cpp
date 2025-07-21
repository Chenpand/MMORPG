#include "CLAuctionRecoveItemDataEntry.h"

bool AuctionRecoveItemDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "AuctionRecoveItemDataEntry::Read" << "  row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	iD = row.ReadUInt32();

	sysRecoPriceRate = row.ReadUInt8();

	return true;
}