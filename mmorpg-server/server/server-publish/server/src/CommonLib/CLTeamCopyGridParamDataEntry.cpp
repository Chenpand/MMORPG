/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyGridParamDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLTeamCopyDefine.h>
// __CUSTOM_HEADER_END__

TeamCopyGridParamDataEntry::TeamCopyGridParamDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyGridParamDataEntry::~TeamCopyGridParamDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyGridParamDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyGridParamDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	grade = row.ReadUInt32();
	resId = row.ReadUInt32();
	gridType = row.ReadUInt32();
	param = row.ReadString();
	param2 = row.ReadString();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyGridParamDataEntryMgr::AddEntry(TeamCopyGridParamDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyGridParamDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(entry->param, strs, "|");
	for (auto& it : strs)
	{
		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(it);
		entry->paramVec.push_back(val);
	}

	// 参数检查
	if (entry->gridType == TC_GRID_OBJ_MAIN_CITY && entry->paramVec.size() != 1)
	{
		return false;
	}

	if (entry->gridType == TC_GRID_OBJ_REVIVE_CNT && entry->paramVec.size() != 1)
	{
		return false;
	}

	if (entry->gridType == TC_GRID_OBJ_CD_DOWN && entry->paramVec.size() != 3)
	{
		return false;
	}

	if (entry->gridType == TC_GRID_OBJ_LABORATORY && entry->paramVec.size() != 6)
	{
		return false;
	}

	std::vector<std::string> strs2;
	Avalon::StringUtil::Split(entry->param2, strs2, "|");
	for (auto& it : strs2)
	{
		UInt32 val = Avalon::StringUtil::ConvertToValue<UInt32>(it);
		entry->paramVec2.push_back(val);
	}

	m_GradeGridSourceMap[{entry->grade, entry->resId}] = entry;

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

std::vector<UInt32> TeamCopyGridParamDataEntryMgr::GetParam(UInt32 grade, UInt32 sourceId)
{
	auto iter = m_GradeGridSourceMap.find({grade, sourceId});
	return iter != m_GradeGridSourceMap.end() ? iter->second->paramVec : std::vector<UInt32>();
}

std::vector<UInt32> TeamCopyGridParamDataEntryMgr::GetParam2(UInt32 grade, UInt32 sourceId)
{
	auto iter = m_GradeGridSourceMap.find({ grade, sourceId });
	return iter != m_GradeGridSourceMap.end() ? iter->second->paramVec2 : std::vector<UInt32>();
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

