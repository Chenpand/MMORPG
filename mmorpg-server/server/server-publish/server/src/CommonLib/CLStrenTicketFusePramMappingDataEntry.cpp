/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLStrenTicketFusePramMappingDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

StrenTicketFusePramMappingDataEntry::StrenTicketFusePramMappingDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

StrenTicketFusePramMappingDataEntry::~StrenTicketFusePramMappingDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 StrenTicketFusePramMappingDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool StrenTicketFusePramMappingDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	muLowerRange = row.ReadUInt32();
	muUpperRange = row.ReadUInt32();
	sigmal = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool StrenTicketFusePramMappingDataEntryMgr::AddEntry(StrenTicketFusePramMappingDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<StrenTicketFusePramMappingDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt32 StrenTicketFusePramMappingDataEntryMgr::GetSigmal(UInt32 mu)
{
	class DataVisitor : public Avalon::DataEntryVisitor<StrenTicketFusePramMappingDataEntry>
	{
	public:
		DataVisitor(UInt32 mu) : sigmal(0), m_Mu(mu) {}

		bool operator()(StrenTicketFusePramMappingDataEntry* data)
		{
			if (!data) return true;

			if (data->muLowerRange <= m_Mu && m_Mu < data->muUpperRange)
			{
				sigmal = data->sigmal;
				return false;
			}

			return true;
		}

		UInt32 sigmal;

	private:
		UInt32 m_Mu;
	};

	DataVisitor visitor(mu);
	Visit(visitor);

	return visitor.sigmal;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

