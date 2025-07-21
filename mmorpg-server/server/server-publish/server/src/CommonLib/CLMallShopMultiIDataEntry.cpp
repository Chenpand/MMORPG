/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLMallShopMultiIDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <TimeUtil.h>
// __CUSTOM_HEADER_END__

MallShopMultiIDataEntry::MallShopMultiIDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

MallShopMultiIDataEntry::~MallShopMultiIDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 MallShopMultiIDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return 0;
// __CUSTOM_GETKEY_END__
}

bool MallShopMultiIDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(startTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(endTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}
	malls = row.ReadString();
	multiple = row.ReadUInt8();

// __CUSTOM_READ_BEGIN__
	std::vector <std::string> mallsStrVec;
	Avalon::StringUtil::Split(malls, mallsStrVec, "|");
	for (std::vector<std::string>::iterator iter = mallsStrVec.begin(); iter != mallsStrVec.end(); ++iter)
	{
		MallsVec.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(*iter));
	}
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool MallShopMultiIDataEntryMgr::AddEntry(MallShopMultiIDataEntry* entry)
{
	//if (!Avalon::DataEntryMgrBase<MallShopMultiIDataEntry>::AddEntry(entry))
	//{
	//	return false;
	//}
// __CUSTOM_ADDENTRY_BEGIN__
	for (auto &i : entry->MallsVec)
	{
		m_MallDataEntry[i].push_back(entry);
	}
// __CUSTOM_ADDENTRY_END__
	return true;
}

void MallShopMultiIDataEntryMgr::GetMallItemMultiple(UInt32 id, UInt8 &multiple, UInt32 &endTime)
{
	if (m_MallDataEntry.find(id) != m_MallDataEntry.end())
	{
		for (auto &i : m_MallDataEntry[id])
		{
			if (CURRENT_TIME.Sec() > i->startTime && CURRENT_TIME.Sec() < i->endTime)
			{
				multiple += i->multiple;
				endTime = i->endTime;
				break;
			}
		}
	}
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

