/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLGuildProsperityDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

GuildProsperityDataEntry::GuildProsperityDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

GuildProsperityDataEntry::~GuildProsperityDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 GuildProsperityDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return guildLevel;
// __CUSTOM_GETKEY_END__
}

bool GuildProsperityDataEntry::Read(Avalon::DataRow &row)
{
	guildLevel = row.ReadUInt32();
	dismissLevel = row.ReadUInt32();
	midLevel = row.ReadUInt32();
	highLevel = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool GuildProsperityDataEntryMgr::AddEntry(GuildProsperityDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<GuildProsperityDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__
// __CUSTOM_MGRFUNCTIONDEFINE_END__

