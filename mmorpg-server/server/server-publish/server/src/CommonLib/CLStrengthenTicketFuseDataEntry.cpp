/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLStrengthenTicketFuseDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

StrengthenTicketFuseDataEntry::StrengthenTicketFuseDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

StrengthenTicketFuseDataEntry::~StrengthenTicketFuseDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 StrengthenTicketFuseDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool StrengthenTicketFuseDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	strengthenLv = row.ReadUInt32();
	fixM = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool StrengthenTicketFuseDataEntryMgr::AddEntry(StrengthenTicketFuseDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<StrengthenTicketFuseDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt32 StrengthenTicketFuseDataEntryMgr::GetFixM(UInt32 strenLevel)
{
	class DataVisitor : public Avalon::DataEntryVisitor<StrengthenTicketFuseDataEntry>
	{
	public:
		DataVisitor(UInt32 strenLevel) : fixM(0),m_StrenLevel(strenLevel) {}

		bool operator()(StrengthenTicketFuseDataEntry* data)
		{
			if (!data) return true;

			if (data->strengthenLv == m_StrenLevel)
			{
				fixM = data->fixM;
				return false;
			}

			return true;
		}

		UInt32 fixM;

	private:
		UInt32 m_StrenLevel;
	};

	DataVisitor visitor(strenLevel);

	StrengthenTicketFuseDataEntryMgr::Instance()->Visit(visitor);

	return visitor.fixM;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

