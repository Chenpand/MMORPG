#include "CLJobDataEntry.h"

JobDataEntry::JobDataEntry()
{

}

JobDataEntry::~JobDataEntry()
{

}

bool JobDataEntry::CheckTargetOccu(UInt8 occu)
{
	OccuVec::iterator occuIter = targetOccus.begin();
	while (occuIter != targetOccus.end())
	{
		if (*occuIter == occu)
		{
			return true;
		}
		occuIter++;
	}
	return false;
}

UInt32 JobDataEntry::GetInitLevel(UInt16 id)
{
	SkillVec::iterator skillIter = find(skills.begin(), skills.end(), id);
	if (skillIter != skills.end())
	{
		return 1;
	}
	return 0;
}

UInt32 JobDataEntry::GetPreSkillLevel(UInt16 id)
{
	SkillVec::iterator skillIter = find(preJobSkills.begin(), preJobSkills.end(), id);
	if (skillIter != skills.end())
	{
		return 1;
	}
	return 0;
}

bool JobDataEntry::Read(Avalon::DataRow& row)
{
    occu = (Occupation)row.ReadUInt32();
	name = row.ReadString();
	isOpen = (bool)(row.ReadUInt32() != 0);

	std::string split = "|";
	std::string strSkills = row.ReadString();
	std::vector<std::string> vecSkills;
	Avalon::StringUtil::Split(strSkills, vecSkills, split);

	for (UInt32 i = 0; i < vecSkills.size(); i++)
	{
		UInt16 skillId = Avalon::StringUtil::ConvertToValue<UInt16>(vecSkills[i].c_str());
		if (skillId == 0) continue;
		skills.emplace_back(skillId);
	}

	
	std::string strAwakenSkills = row.ReadString();
	vecSkills.clear();
	Avalon::StringUtil::Split(strAwakenSkills, vecSkills, split);
	for (UInt32 i = 0; i < vecSkills.size(); ++i)
	{
		UInt16 skillId = Avalon::StringUtil::ConvertToValue<UInt16>(vecSkills[i].c_str());
		if (skillId == 0) continue;
		awakenSkills.emplace_back(skillId);
	}

	std::string strPreJobSkills = row.ReadString();
	vecSkills.clear();
	Avalon::StringUtil::Split(strPreJobSkills, vecSkills, split);
	for (UInt32 i = 0; i < vecSkills.size(); ++i)
	{
		UInt16 skillId = Avalon::StringUtil::ConvertToValue<UInt16>(vecSkills[i].c_str());
		if (skillId == 0) continue;
		preJobSkills.emplace_back(skillId);
	}

	std::string strOccu = row.ReadString();
	std::vector<std::string> vecOccu;
	Avalon::StringUtil::Split(strOccu, vecOccu, split);

	for (UInt32 i = 0;i < vecOccu.size();i++)
	{
		UInt8 val = Avalon::StringUtil::ConvertToValue<UInt8>(vecOccu[i].c_str());
		targetOccus.emplace_back(val);
	}

	canCreateRole = (bool)(row.ReadUInt32() != 0);

	itemId = row.ReadUInt32();

	EqualPvPSP = row.ReadInt32();

	chiJiOccu = row.ReadInt32();

    return true;
}

bool JobDataEntryMgr::AddEntry(JobDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<JobDataEntry>::AddEntry(entry))
	{
		return false;
	}

	if (JobDataEntry::GetBaseOccu(entry->occu) != entry->occu && entry->isOpen)
	{
		m_OpenChangeOccuVec.push_back(entry->occu);
	}

	if (entry->chiJiOccu != 0)
	{
		m_ChiJiOccuVec.push_back(entry->occu);
	}

	return true;
}