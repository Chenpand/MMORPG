/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLSkillRecommendDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

SkillRecommendDataEntry::SkillRecommendDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

SkillRecommendDataEntry::~SkillRecommendDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 SkillRecommendDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__

	return id;
// __CUSTOM_GETKEY_END__
}

bool SkillRecommendDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	jobID = row.ReadUInt32();
	skillMode = row.ReadUInt32();
	maxLv = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-" && str != "")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				skillIDVec.push_back(value);
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
				priorityVec.push_back(value);
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
				posIDVec.push_back(value);
			}
		}
	}

// __CUSTOM_READ_BEGIN__
// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool SkillRecommendDataEntryMgr::AddEntry(SkillRecommendDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SkillRecommendDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	UInt32 skillNum = entry->skillIDVec.size();
	if (skillNum != entry->priorityVec.size() || skillNum != entry->posIDVec.size())
	{
		ErrorLogStream << "SkillRecommendDataEntry Table Error id:" << entry->id << " skill id num:" << skillNum
			<< " skill priority num:" << UInt32(entry->priorityVec.size())
			<< " skill pos num:" << UInt32(entry->posIDVec.size())
			<< LogStream::eos;
		return false;
	}

	UInt32 max_priority = 0;
	for (auto& iter :entry->priorityVec)
	{
		if (iter > max_priority)
			max_priority = iter;
	}

	// 检查优先级的Vec，优先级ID必须连续，否则出错
	if (max_priority != entry->priorityVec.size())
	{
		ErrorLogStream << "SkillRecommendDataEntry Table Error id:" << entry->id << " priorityVec num error." << LogStream::eos;
		return false;
	}

	std::vector<UInt32>::iterator iterId = entry->skillIDVec.begin();
	std::vector<UInt32>::iterator iterPriority = entry->priorityVec.begin();
	std::vector<UInt32>::iterator iterPos = entry->posIDVec.begin();

	JobSkillInfo JobInfo;
	JobInfo.jobID = entry->jobID;
	JobInfo.skillConfigMode = entry->skillMode;

	for (UInt32 k = 0;k < skillNum;++k)
	{
		SkillDetails* skill_details = new SkillDetails();
		skill_details->skillID = *iterId;
		skill_details->Priority = *iterPriority;
		skill_details->skillPos = *iterPos;

		_addSkillRecommendInfo(entry->maxLv, JobInfo, skill_details, skillNum);

		iterId++, iterPriority++, iterPos++;
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

SkillRecommendDataEntryMgr::SkillRecommendDataEntryMgr()
{

}

void SkillRecommendDataEntryMgr::_addSkillRecommendInfo(UInt32 _maxLv, JobSkillInfo& _jobInfo, const SkillDetails* _details, UInt32 _skillNum)
{
	auto iter = m_JobSkillRecommondInfoMap.find(_jobInfo);
	if (iter == m_JobSkillRecommondInfoMap.end())
	{
		SkillRecommendVec skillVec;
		skillVec.resize(_skillNum);
		skillVec[_details->Priority - 1] = _details;

		RecommendInfoMap recommend_info_map;
		recommend_info_map[_maxLv] = skillVec;
		m_JobSkillRecommondInfoMap[_jobInfo] = recommend_info_map;
  	}
	else
	{
		RecommendInfoMap& recommend_info_map = iter->second;
		auto it = recommend_info_map.find(_maxLv);
		if (it == recommend_info_map.end())
		{
			SkillRecommendVec skillVec;
			skillVec.resize(_skillNum);
			skillVec[_details->Priority - 1] = (_details);
			recommend_info_map[_maxLv] = skillVec;
		}
		else
			it->second[_details->Priority - 1] = _details;
	}
}

const SkillRecommendVec* SkillRecommendDataEntryMgr::GetRecommendSkillVec(UInt32 _jobID, UInt32 _skillMode, UInt32 _playerLv)
{
	JobSkillInfo jobInfo;
	jobInfo.jobID = _jobID;
	jobInfo.skillConfigMode = _skillMode;
	auto iter = m_JobSkillRecommondInfoMap.find(jobInfo);
	if (iter == m_JobSkillRecommondInfoMap.end())
		return NULL;

	RecommendInfoMap& recommendInfoMap = iter->second;
	auto it = recommendInfoMap.lower_bound(_playerLv);
 	if (it == recommendInfoMap.end())
		return &((--recommendInfoMap.end())->second);

	return &(it->second);
}
// __CUSTOM_MGRFUNCTIONDEFINE_END__

