/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiRegionDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiRegionDataEntry::ChiJiRegionDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiRegionDataEntry::~ChiJiRegionDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiRegionDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiRegionDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiRegionDataEntryMgr::AddEntry(ChiJiRegionDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiRegionDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

