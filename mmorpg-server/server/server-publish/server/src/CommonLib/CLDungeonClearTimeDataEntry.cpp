/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLDungeonClearTimeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

DungeonClearTimeDataEntry::DungeonClearTimeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

DungeonClearTimeDataEntry::~DungeonClearTimeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 DungeonClearTimeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return dungeonID;
// __CUSTOM_GETKEY_END__
}

bool DungeonClearTimeDataEntry::Read(Avalon::DataRow &row)
{
	dungeonID = row.ReadUInt32();
	level = row.ReadUInt32();
	score = row.ReadUInt32();
	level29 = row.ReadUInt32();
	level39 = row.ReadUInt32();
	level49 = row.ReadUInt32();
	level59 = row.ReadUInt32();
	level69 = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool DungeonClearTimeDataEntryMgr::AddEntry(DungeonClearTimeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<DungeonClearTimeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

