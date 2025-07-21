/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChijiSpawnItemDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChijiSpawnItemDataEntry::ChijiSpawnItemDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChijiSpawnItemDataEntry::~ChijiSpawnItemDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChijiSpawnItemDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChijiSpawnItemDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	times = row.ReadUInt32();
	packID = row.ReadUInt32();
	itemID = row.ReadUInt32();
	weight = row.ReadUInt32();
	minNumber = row.ReadUInt32();
	maxNumber = row.ReadUInt32();
	mapID = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChijiSpawnItemDataEntryMgr::AddEntry(ChijiSpawnItemDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChijiSpawnItemDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

