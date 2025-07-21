#include "CLAuctionPageStruct.h"

bool AuctionPageStructDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	bPubDisplay = row.ReadUInt8();
	return true;
}

bool AuctionPageStructDataEntryMgr::AddEntry(AuctionPageStructDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AuctionPageStructDataEntry>::AddEntry(entry)) return false;

	OnAddEntry(entry);

	return true;
}

void AuctionPageStructDataEntryMgr::OnAddEntry(AuctionPageStructDataEntry* entry)
{
	if (entry && entry->bPubDisplay == 1)
	{
		m_PubDisplayIds.push_back(entry->id);
	}
}