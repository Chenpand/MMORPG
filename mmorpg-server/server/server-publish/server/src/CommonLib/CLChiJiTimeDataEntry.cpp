/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiTimeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiTimeDataEntry::ChiJiTimeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
	minX = 0;
	maxX = 0;
	minY = 0;
	maxY = 0;
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiTimeDataEntry::~ChiJiTimeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiTimeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiTimeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	chickenType = (ChickenType)row.ReadUInt32();
	chickenParameter = row.ReadUInt32();
	startTime = row.ReadUInt32();
	continueTime = row.ReadUInt32();
	continueInjury = row.ReadUInt32();
	nextDuquanID = row.ReadUInt32();
	

	auto str = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() == 4)
	{
		minX = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		maxX = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		minY = Avalon::StringUtil::ConvertToValue<UInt32>(strs[2]);
		maxY = Avalon::StringUtil::ConvertToValue<UInt32>(strs[3]);
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiTimeDataEntryMgr::AddEntry(ChiJiTimeDataEntry* entry)
{
	if (!Avalon::DataEntryListMgrBase<ChiJiTimeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

