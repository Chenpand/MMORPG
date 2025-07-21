/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLFashionDecomposeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

FashionDecomposeDataEntry::FashionDecomposeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

FashionDecomposeDataEntry::~FashionDecomposeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 FashionDecomposeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return MakeKey(subType, thirdType, color);
// __CUSTOM_GETKEY_END__
}

bool FashionDecomposeDataEntry::Read(Avalon::DataRow &row)
{
	subType = row.ReadUInt32();
	thirdType = row.ReadUInt32();
	color = row.ReadUInt32();
	fashionID = row.ReadUInt32();
	groupID = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
UInt32 FashionDecomposeDataEntry::MakeKey(UInt32 subType, UInt32 thirdType, UInt32 color)
{
	UInt32 key = (color << 24) + (subType << 16) + thirdType;
	return key;
}
// __CUSTOM_ENTRYFUNCTION_END__

bool FashionDecomposeDataEntryMgr::AddEntry(FashionDecomposeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<FashionDecomposeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

