/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildDungeonDamageCoeffDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildDungeonDamageCoeffDataEntry::GuildDungeonDamageCoeffDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildDungeonDamageCoeffDataEntry::~GuildDungeonDamageCoeffDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildDungeonDamageCoeffDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool GuildDungeonDamageCoeffDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	coefficient = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildDungeonDamageCoeffDataEntryMgr::AddEntry(GuildDungeonDamageCoeffDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildDungeonDamageCoeffDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	m_OccuDamageCoeff[entry->id] = entry->coefficient;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
UInt32 GuildDungeonDamageCoeffDataEntryMgr::GetDamageCoeff(UInt32 occu)
{
	auto iter = m_OccuDamageCoeff.find(occu);
	if (iter == m_OccuDamageCoeff.end())
	{
		return 0;
	}

	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

