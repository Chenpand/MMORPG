/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLChiJiSkillDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

ChiJiSkillDataEntry::ChiJiSkillDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

ChiJiSkillDataEntry::~ChiJiSkillDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 ChiJiSkillDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool ChiJiSkillDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	maxLvl = row.ReadUInt32();
	job = row.ReadString();

// __CUSTOM_READ_BEGIN__

	std::vector<std::string> param;
	Avalon::StringUtil::Split(job, param, "|");
	for (auto& iter : param)
	{
		UInt32 jobId = Avalon::StringUtil::ConvertToValue<UInt32>(iter);
		jobVec.push_back(jobId);
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool ChiJiSkillDataEntryMgr::AddEntry(ChiJiSkillDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ChiJiSkillDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	for (auto& iter : entry->jobVec)
	{
		m_JobSkillMap[iter].push_back(entry->id);
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

void ChiJiSkillDataEntryMgr::GetSkillVec(UInt32 jobId, UInt32 skillNum, std::set<UInt32>& exceptSet, std::vector<UInt32>& skillVec)
{
	auto iter = m_JobSkillMap.find(jobId);
	if (iter == m_JobSkillMap.end())
	{
		return;
	}

	std::vector<UInt32> tempVec = iter->second;
	for (auto& tt : exceptSet)
	{
		std::vector<UInt32>::iterator tt1 = tempVec.begin();
		while (tt1 != tempVec.end())
		{
			if (tt == *tt1)
			{
				tempVec.erase(tt1);
				break;
			}
			++ tt1;
		}
	}

	skillVec.clear();
	std::random_shuffle(tempVec.begin(), tempVec.end());
	for (auto& it : tempVec)
	{
		if (skillNum == 0)
			return;

		skillVec.push_back(it);
		--skillNum;
	}
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

