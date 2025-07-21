/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyFlopDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

TeamCopyFlopDataEntry::TeamCopyFlopDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyFlopDataEntry::~TeamCopyFlopDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyFlopDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyFlopDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	teamCopyID = row.ReadUInt32();
	teamGrade = row.ReadUInt32();
	stage = row.ReadUInt32();
	param = row.ReadUInt32();
	dropId = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyFlopDataEntryMgr::AddEntry(TeamCopyFlopDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyFlopDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	TeamCopyFlopKey key(entry->teamCopyID, entry->teamGrade, entry->stage, entry->param);
	m_FlopMap[key] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const TeamCopyFlopDataEntry* TeamCopyFlopDataEntryMgr::GetFlopReward(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 param)
{
	TeamCopyFlopKey key(copyId, teamGrade, stage, param);
	auto iter = m_FlopMap.find(key);
	if (iter == m_FlopMap.end())
	{
		return NULL;
	}

	return iter->second;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

