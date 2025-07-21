/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLStandardNormalDistributionDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

StandardNormalDistributionDataEntry::StandardNormalDistributionDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

StandardNormalDistributionDataEntry::~StandardNormalDistributionDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 StandardNormalDistributionDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool StandardNormalDistributionDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	inputValue = row.ReadUInt32();
	outputValue = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool StandardNormalDistributionDataEntryMgr::AddEntry(StandardNormalDistributionDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<StandardNormalDistributionDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	m_InputOutputMap.insert(std::make_pair(entry->inputValue, entry->outputValue));
	if (m_InputMax < entry->inputValue)
	{
		m_InputMax = entry->inputValue;
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt32 StandardNormalDistributionDataEntryMgr::GetOutputValue(UInt32 inputValue)
{
	if (inputValue >= m_InputMax)
	{
		return m_InputOutputMap[m_InputMax];
	}

	auto itr = m_InputOutputMap.find(inputValue);
	if (itr == m_InputOutputMap.end())
	{
		return 0;
	}
	return itr->second;
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

