/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAccountCounterDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

AccountCounterDataEntry::AccountCounterDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AccountCounterDataEntry::~AccountCounterDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AccountCounterDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AccountCounterDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	accountCounterType = (AccountCounterType)row.ReadUInt32();
	refreshType = row.ReadUInt32();
	refreshTimePoint = row.ReadString();

// __CUSTOM_READ_BEGIN__

	if (id != (UInt32)accountCounterType)
	{
		ErrorLogStream << "ID(" << id << ") must be same with account counter type(" << (UInt32)accountCounterType << ")!" << LogStream::eos;
		return false;
	}

	if (!TimeUtil::StringConvertToRefreshDate((TimeUtil::RefreshType)refreshType, refreshTimePoint, refreshDate))
	{
		ErrorLogStream << "Refresh type(" << refreshType << ") is not match refresh tiempoint(" << refreshTimePoint << ")!" << LogStream::eos;
		return false;
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool AccountCounterDataEntryMgr::AddEntry(AccountCounterDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AccountCounterDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

