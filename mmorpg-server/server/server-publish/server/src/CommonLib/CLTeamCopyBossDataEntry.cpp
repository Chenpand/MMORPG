/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyBossDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

TeamCopyBossDataEntry::TeamCopyBossDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyBossDataEntry::~TeamCopyBossDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyBossDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyBossDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	teamCopyID = row.ReadUInt32();
	teamGrade = row.ReadUInt32();
	stage = row.ReadUInt32();
	appearField = row.ReadUInt32();
	closeField = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyBossDataEntryMgr::AddEntry(TeamCopyBossDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyBossDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__
	
	TeamCopyBossKey key(entry->teamCopyID, entry->teamGrade, entry->stage);
	m_BossMap[key] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const TeamCopyBossDataEntry* TeamCopyBossDataEntryMgr::GetBossData(UInt32 copyId, UInt32 teamGrade, UInt32 stage)
{
	TeamCopyBossKey key(copyId, teamGrade, stage);
	auto iter = m_BossMap.find(key);
	if (iter == m_BossMap.end())
	{
		return NULL;
	}

	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

