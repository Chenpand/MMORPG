/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAntiAddictHolidayDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

AntiAddictHolidayDataEntry::AntiAddictHolidayDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AntiAddictHolidayDataEntry::~AntiAddictHolidayDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AntiAddictHolidayDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AntiAddictHolidayDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	holidayDate = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool AntiAddictHolidayDataEntryMgr::AddEntry(AntiAddictHolidayDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AntiAddictHolidayDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_HolidaySet.insert(entry->holidayDate);

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

bool AntiAddictHolidayDataEntryMgr::TodayIsHoliday()
{
	Int32 year = CURRENT_DATE.Year();
	Int32 month = CURRENT_DATE.Month();
	Int32 day = CURRENT_DATE.Day();

	UInt32 date = year * 10000 + month * 100 + day;

	auto iter = m_HolidaySet.find(date);
	return iter != m_HolidaySet.end();
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

