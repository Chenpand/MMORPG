#include "CLCurrencyCostDataEntry.h"

bool CurrencyCostDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	itemId = row.ReadUInt32();
	num = row.ReadUInt32();

	return true;
}