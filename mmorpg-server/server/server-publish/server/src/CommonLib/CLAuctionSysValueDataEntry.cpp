#include "CLAuctionSysValueDataEntry.h"

AuctionSystemValueDataEntry::AuctionSystemValueDataEntry()
{
}

AuctionSystemValueDataEntry::~AuctionSystemValueDataEntry()
{
}

bool AuctionSystemValueDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = (AuctionSystemValueType)row.ReadUInt32();
	value = row.ReadUInt32();
	return true;
}

AuctionSystemValueDataEntryMgr::AuctionSystemValueDataEntryMgr()
{
}

AuctionSystemValueDataEntryMgr::~AuctionSystemValueDataEntryMgr()
{
}

bool AuctionSystemValueDataEntryMgr::AddEntry(AuctionSystemValueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AuctionSystemValueDataEntry>::AddEntry(entry)) return false;

	OnAddEntry(entry);

	return true;

}

void AuctionSystemValueDataEntryMgr::OnReload()
{
	class SystemValueDataEntryVisitor :public Avalon::DataEntryVisitor<AuctionSystemValueDataEntry>
	{
	public:
		SystemValueDataEntryVisitor(){}

		bool operator()(AuctionSystemValueDataEntry* entry)
		{
			AuctionSystemValueDataEntryMgr::Instance()->OnAddEntry(entry);
			return true;
		}
	};
	
	SystemValueDataEntryVisitor visitor;
	AuctionSystemValueDataEntryMgr::Instance()->Visit(visitor);
}

void AuctionSystemValueDataEntryMgr::OnAddEntry(AuctionSystemValueDataEntry* entry)
{
	if (entry->type != 0)
	{
		m_SystemValues[entry->type] = entry;
	}
}

UInt32 AuctionSystemValueDataEntryMgr::GetSystemValue(AuctionSystemValueType type)
{
	SystemValueMap::iterator iter = m_SystemValues.find(type);
	if (iter != m_SystemValues.end())
		return iter->second->value;
	return 0;
}

