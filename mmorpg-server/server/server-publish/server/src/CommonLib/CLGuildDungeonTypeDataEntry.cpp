/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonTypeDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonTypeDataEntry::GuildDungeonTypeDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonTypeDataEntry::~GuildDungeonTypeDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonTypeDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonTypeDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	buildLvl = row.ReadUInt32();
	dungeonType = row.ReadUInt32();
	dungeonTypeName = row.ReadString();
// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonTypeDataEntryMgr::AddEntry(GuildDungeonTypeDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonTypeDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->buildLvl == 0)
	{
		ErrorLogStream << "GuildDungeonTypeDataEntryMgr guildLvl Error id:" << entry->id << LogStream::eos;
		return false;
	}

	m_GuildTypeMap[entry->buildLvl] = entry->dungeonType;
	m_GuildTypeNameMap[entry->dungeonType] = entry->dungeonTypeName;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

UInt32 GuildDungeonTypeDataEntryMgr::GetGuildDungeonType(UInt32 buildLvl)
{
	if (m_GuildTypeMap.empty())
		return 0;

	auto iter = m_GuildTypeMap.lower_bound(buildLvl);
	if (iter == m_GuildTypeMap.end())
		return m_GuildTypeMap.begin()->second;

	return iter->second;
}

void GuildDungeonTypeDataEntryMgr::GetDungeonType(std::vector<UInt32>& vec)
{
	for (auto& itr : m_GuildTypeMap)
	{
		vec.push_back(itr.second);
	}
}
std::string GuildDungeonTypeDataEntryMgr::GetGuildDungeonTypeName(UInt32 dungeonType)
{
	if (m_GuildTypeNameMap.count(dungeonType) == 0)return "";
	return m_GuildTypeNameMap[dungeonType];
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

