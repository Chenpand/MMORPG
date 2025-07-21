/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildBuildingPointDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildBuildingPointDataEntry::GuildBuildingPointDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildBuildingPointDataEntry::~GuildBuildingPointDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildBuildingPointDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildBuildingPointDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				buildLvl.push_back(value);
			}
		}
	}
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				buildPoint.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__

	if (buildLvl.size() != buildPoint.size())
	{
		ErrorLogStream << "buildLvl size not equal buildPoint size! id:"<< id << LogStream::eos;
		return false;
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildBuildingPointDataEntryMgr::AddEntry(GuildBuildingPointDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildBuildingPointDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	std::map<UInt32, UInt32> tempMap;
	for (UInt32 k = 0; k < entry->buildLvl.size(); ++k)
	{
		tempMap[entry->buildLvl[k]] = entry->buildPoint[k];
	}

	m_BuildPointMap[entry->id] = tempMap;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 GuildBuildingPointDataEntryMgr::GetBuildTotalPointByType(UInt32 buildType, UInt32 buildLvl)
{
	auto iter = m_BuildPointMap.find(buildType);
	if (iter == m_BuildPointMap.end())
		return 0;

	auto it = iter->second.find(buildLvl);
	if (it == iter->second.end())
		return 0;

	return it->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

