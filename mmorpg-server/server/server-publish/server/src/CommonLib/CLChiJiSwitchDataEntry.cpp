/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiSwitchDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiSwitchDataEntry::ChiJiSwitchDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiSwitchDataEntry::~ChiJiSwitchDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiSwitchDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiSwitchDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	openTime = row.ReadString();
	closeTime = row.ReadString();
	isOpen = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiSwitchDataEntryMgr::AddEntry(ChiJiSwitchDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiSwitchDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	// 开关关闭
	if (entry->isOpen == 0)
		return true;

	ChiJiSwitch jiSwitch;

	std::vector<std::string> startVec;
	Avalon::StringUtil::Split(entry->openTime, startVec, ":");
	if (startVec.size() >= 3)
	{
		UInt32 hour = Avalon::StringUtil::ConvertToValue<UInt32>(startVec[0]);
		UInt32 min = Avalon::StringUtil::ConvertToValue<UInt32>(startVec[1]);
		UInt32 sec = Avalon::StringUtil::ConvertToValue<UInt32>(startVec[2]);
		jiSwitch.startTime = hour * 3600 + min * 60 + sec;
	}
	else
	{
		ErrorLogStream << "ChiJiSwitchDataEntryMgr Time Format Error id:" << entry->id << LogStream::eos;
		return false;
	}

	std::vector<std::string> endVec;
	Avalon::StringUtil::Split(entry->closeTime, endVec, ":");
	if (endVec.size() >= 3)
	{
		UInt32 hour = Avalon::StringUtil::ConvertToValue<UInt32>(endVec[0]);
		UInt32 min = Avalon::StringUtil::ConvertToValue<UInt32>(endVec[1]);
		UInt32 sec = Avalon::StringUtil::ConvertToValue<UInt32>(endVec[2]);
		jiSwitch.endTime = hour * 3600 + min * 60 + sec;
	}
	else
	{
		ErrorLogStream << "ChiJiSwitchDataEntryMgr Time Format Error id:" << entry->id << LogStream::eos;
		return false;
	}

	m_ChiJiSwitchMap[entry->id] = jiSwitch;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const ChiJiSwitch* ChiJiSwitchDataEntryMgr::GetChiJiSwitch(UInt32 week)
{
	auto iter = m_ChiJiSwitchMap.find(week);
	if (iter == m_ChiJiSwitchMap.end())
		return NULL;

	return &(iter->second);
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

