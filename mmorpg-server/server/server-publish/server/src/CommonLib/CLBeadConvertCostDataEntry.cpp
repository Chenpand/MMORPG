/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLBeadConvertCostDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

BeadConvertCostDataEntry::BeadConvertCostDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

BeadConvertCostDataEntry::~BeadConvertCostDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 BeadConvertCostDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return colour;
// __CUSTOM_GETKEY_END__
}

bool BeadConvertCostDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	colour = row.ReadUInt32();
	moneyCostId = row.ReadUInt32();
	moneyCostNum = row.ReadUInt32();
	beadCost = row.ReadUInt32();
	beadCostNum = row.ReadUInt32();
	dropId = row.ReadUInt32();
	bindDropId = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool BeadConvertCostDataEntryMgr::AddEntry(BeadConvertCostDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BeadConvertCostDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

