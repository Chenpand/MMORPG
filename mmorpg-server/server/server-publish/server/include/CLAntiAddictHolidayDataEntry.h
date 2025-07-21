/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ANTIADDICTHOLIDAY_DATA_ENTRY_H__
#define __CL_ANTIADDICTHOLIDAY_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <set>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class AntiAddictHolidayDataEntry : public Avalon::DataEntry
{
public:
	AntiAddictHolidayDataEntry();
	virtual ~AntiAddictHolidayDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 日期
	UInt32                                          holidayDate;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class AntiAddictHolidayDataEntryMgr : public Avalon::DataEntryMgrBase<AntiAddictHolidayDataEntry>
	, public Avalon::Singleton<AntiAddictHolidayDataEntryMgr>
{
public:
	virtual bool AddEntry(AntiAddictHolidayDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 今天是否节假日
	bool TodayIsHoliday();

private:

	// 节假日
	std::set<UInt32> m_HolidaySet;

// __CUSTOM_MGR_END__
};

#endif
