/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLAntiAddictTimeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

AntiAddictTimeDataEntry::AntiAddictTimeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

AntiAddictTimeDataEntry::~AntiAddictTimeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 AntiAddictTimeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool AntiAddictTimeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	isHoliday = row.ReadUInt32();
	age = row.ReadUInt32();
	gameTime = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool AntiAddictTimeDataEntryMgr::AddEntry(AntiAddictTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<AntiAddictTimeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	auto iter = m_AntiAddictTimeMap.find(entry->isHoliday);
	if (iter == m_AntiAddictTimeMap.end())
	{
		std::map<UInt32, UInt32> tempMap;
		tempMap[entry->age] = entry->gameTime;
		m_AntiAddictTimeMap[entry->isHoliday] = tempMap;
	}
	else
	{
		iter->second[entry->age] = entry->gameTime;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 AntiAddictTimeDataEntryMgr::GetAntiAddictTime(bool isHoliday, UInt32 age)
{
	UInt32 dateType = isHoliday ? 1:0;
	std::map<UInt32, UInt32>& ageMap = m_AntiAddictTimeMap[dateType];
	auto iter = ageMap.lower_bound(age);
	return iter != ageMap.end() ? iter->second : 0;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

