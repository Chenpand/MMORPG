/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLEqSuitScoreDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

EqSuitScoreDataEntry::EqSuitScoreDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

EqSuitScoreDataEntry::~EqSuitScoreDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 EqSuitScoreDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool EqSuitScoreDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = row.ReadUInt8();
	suitEqNum = row.ReadUInt32();
	suitLev = row.ReadUInt32();
	suitScore = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool EqSuitScoreDataEntryMgr::AddEntry(EqSuitScoreDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<EqSuitScoreDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	EqSuitScoreKey key(entry->type, entry->suitEqNum, entry->suitLev);
	auto iter = suitScores.find(key);
	if (iter != suitScores.end())
	{
		ErrorLogStream << "add entry duplicate id : " << entry->GetKey() << LogStream::eos;
		return false;
	}
	suitScores.insert(std::pair<EqSuitScoreKey, EqSuitScoreDataEntry*>(key, entry));

// __CUSTOM_ADDENTRY_END__
	return true;
}

EqSuitScoreDataEntry*	EqSuitScoreDataEntryMgr::GetEntry(UInt8 type, UInt32 suitNum, UInt32 suitLv)
{
	EqSuitScoreKey key(type, suitNum, suitLv);
	auto iter = suitScores.find(key);
	if (iter == suitScores.end())
	{
		return NULL;
	}
	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

