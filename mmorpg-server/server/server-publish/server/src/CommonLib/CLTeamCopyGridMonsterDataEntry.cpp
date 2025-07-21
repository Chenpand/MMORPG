/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyGridMonsterDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

TeamCopyGridMonsterDataEntry::TeamCopyGridMonsterDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyGridMonsterDataEntry::~TeamCopyGridMonsterDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyGridMonsterDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyGridMonsterDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	grade = row.ReadUInt32();
	monsterType = row.ReadUInt32();
	bornPos = row.ReadUInt32();
	moveCd = row.ReadUInt32();
	attackPath = row.ReadString();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyGridMonsterDataEntryMgr::AddEntry(TeamCopyGridMonsterDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyGridMonsterDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(entry->attackPath, strs, "|");
	for (auto& iter : strs)
	{
		UInt32 gridId = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
		entry->attackPathList.push_back(gridId);
	}

	m_BornMap[{entry->grade, entry->monsterType, entry->bornPos}] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

TeamCopyGridMonsterDataEntry* TeamCopyGridMonsterDataEntryMgr::GetGridMonsterData(UInt32 grade, UInt32 monsterType, UInt32 monsterBornId)
{
	auto iter = m_BornMap.find({ grade, monsterType, monsterBornId });
	return iter != m_BornMap.end() ? iter->second : NULL;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

