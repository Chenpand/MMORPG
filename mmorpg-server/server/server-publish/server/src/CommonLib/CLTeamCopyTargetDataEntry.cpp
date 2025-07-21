/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyTargetDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include <CLTeamCopyFieldDataEntry.h>

// __CUSTOM_HEADER_END__

TeamCopyTargetDataEntry::TeamCopyTargetDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyTargetDataEntry::~TeamCopyTargetDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyTargetDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyTargetDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	teamCopyId = row.ReadUInt32();
	teamGrade = row.ReadUInt32();
	stage = row.ReadUInt32();
	targetDifficulty = row.ReadUInt32();
	targetType = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				feildId.push_back(param);
			}
		}
	}
	nextTarget = row.ReadUInt32();

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__

bool LessSort(const TeamCopyTargetCfg* a, const TeamCopyTargetCfg* b)
{
	return a->id < b->id;
}

// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyTargetDataEntryMgr::AddEntry(TeamCopyTargetDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyTargetDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	TeamCopyTargetCfg* cfg = new TeamCopyTargetCfg();
	cfg->id = entry->id;
	cfg->stage = entry->stage;
	cfg->targetType = entry->targetType;
	cfg->nextTarget = entry->nextTarget;

	for (auto& tt : entry->feildId)
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(tt, strs, "_");
		if (strs.size() != 2)
		{
			ErrorLogStream << "team copy target config fieldId error! id" << entry->id << LogStream::eos;
			return false;
		}

		TeamCopyFieldCfg fieldCfg;
		fieldCfg.fieldId = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		fieldCfg.totalNum = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
		fieldCfg.entry = TeamCopyFieldDataEntryMgr::Instance()->FindEntry(fieldCfg.fieldId);
		if (NULL == fieldCfg.entry)
		{
			ErrorLogStream << "not find team copy field config id:" << fieldCfg.fieldId << LogStream::eos;
			return false;
		}

		cfg->feildVec.push_back(fieldCfg);;
	}

	m_TargetMap[entry->id] = cfg;

	TCTargetStageGrade stageGrade(entry->teamCopyId, entry->teamGrade, entry->stage, entry->targetDifficulty);

	auto iter = m_TargetStageGradeMap.find(stageGrade);
	if (iter == m_TargetStageGradeMap.end())
	{
		std::vector<TeamCopyTargetCfg*> tempVec;
		tempVec.push_back(cfg);
		m_TargetStageGradeMap[stageGrade] = tempVec;
	}
	else
	{
		iter->second.push_back(cfg);
		std::sort(iter->second.begin(), iter->second.end(), LessSort);
	}

	TeamCopyStageField stageField(entry->teamGrade, entry->stage);
	auto tt = m_GradeStageFieldMap.find(stageField);
	if (tt == m_GradeStageFieldMap.end())
	{
		std::set<UInt32> tempSet;
		for (auto it : cfg->feildVec)
		{
			tempSet.insert(it.fieldId);
		}
		m_GradeStageFieldMap[stageField] = tempSet;
	}
	else
	{
		for (auto it : cfg->feildVec)
		{
			tt->second.insert(it.fieldId);
		}
	}

	auto t1 = m_GradeFieldMap.find(entry->teamGrade);
	if (t1 == m_GradeFieldMap.end())
	{
		std::set<UInt32> tempSet;
		for (auto it : cfg->feildVec)
		{
			tempSet.insert(it.fieldId);
		}
		m_GradeFieldMap[entry->teamGrade] = tempSet;
	}
	else
	{
		for (auto it : cfg->feildVec)
		{
			t1->second.insert(it.fieldId);
		}
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

const TeamCopyTargetCfg* TeamCopyTargetDataEntryMgr::GetTarget(UInt32 targetId)
{
	auto iter = m_TargetMap.find(targetId);
	if (iter == m_TargetMap.end())
	{
		return NULL;
	}

	return iter->second;
}

const TeamCopyTargetCfg* TeamCopyTargetDataEntryMgr::GetFirstTarget(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 grade)
{
	// TeamCopyTargetCfg 按目标id自动排序
	const std::vector<TeamCopyTargetCfg*>* targetList = GetTargetList(copyId, teamGrade, stage, grade);
	if (NULL == targetList || targetList->empty())
	{
		return NULL;
	}
			
	return (*targetList)[0];
}

const std::vector<TeamCopyTargetCfg*>* TeamCopyTargetDataEntryMgr::GetTargetList(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 grade)
{
	TCTargetStageGrade stageGrade(copyId, teamGrade, stage, grade);
	auto iter = m_TargetStageGradeMap.find(stageGrade);
	if (iter == m_TargetStageGradeMap.end())
	{
		return NULL;
	}

	return &(iter->second);
}

bool TeamCopyTargetDataEntryMgr::IsHasFieldByStage(UInt32 teamGrade, UInt32 stage, UInt32 fieldId)
{
	TeamCopyStageField stageField(teamGrade, stage);
	auto iter = m_GradeStageFieldMap.find(stageField);
	if (iter == m_GradeStageFieldMap.end())
	{
		return false;
	}
	
	auto it = iter->second.find(fieldId);
	if (it == iter->second.end())
	{
		return false;
	}

	return true;
}

bool TeamCopyTargetDataEntryMgr::IsHasFieldByStage(UInt32 teamGrade, UInt32 fieldId)
{
	auto iter = m_GradeFieldMap.find(teamGrade);
	if (iter == m_GradeFieldMap.end())
	{
		return false;
	}

	auto it = iter->second.find(fieldId);
	if (it == iter->second.end())
	{
		return false;
	}

	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

