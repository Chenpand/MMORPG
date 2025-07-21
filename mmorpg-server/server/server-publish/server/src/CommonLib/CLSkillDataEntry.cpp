#include "CLSkillDataEntry.h"
#include "CLSkillDefine.h"

SkillDataEntry::SkillDataEntry()
{
}

SkillDataEntry::~SkillDataEntry()
{
}

bool SkillDataEntry::CheckOccu(UInt8 occu)
{
	OccuVec::iterator occuIter = occus.begin();
	while (occuIter != occus.end())
	{
		if (*occuIter == occu)
		{
			return true;
		}
		occuIter++;
	}
	return false;
}

bool SkillDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt16();
	name = row.ReadString();

	std::string split = "|";
	std::string strOccu = row.ReadString();
	std::vector<std::string> vecOccu;
	Avalon::StringUtil::Split(strOccu, vecOccu, split);

	for (UInt32 i = 0;i < vecOccu.size();i++)
	{
		UInt8 val = Avalon::StringUtil::ConvertToValue<UInt8>(vecOccu[i].c_str());
		occus.push_back(val);
	}
	useType = row.ReadUInt8();
	type = row.ReadUInt8();
	UInt8 tmpVal = row.ReadUInt8();
	isBuff = tmpVal == 1 ? true : false;
	tmpVal = row.ReadUInt8();
	isQTE = tmpVal == 1 ? true : false;
	canUseInPVP = row.ReadUInt8();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "|");
		for (auto& param : strs)
		{
			UInt32 bufferId = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			buffInfoList.push_back(bufferId);
		}
	}

	tmpVal = row.ReadUInt8();
	isPreJob = tmpVal == 1 ? true : false;

	tmpVal = row.ReadUInt8();
	isStudy = tmpVal == 1 ? true : false;

	playerLevel = row.ReadUInt16();
	upLevelInterval = row.ReadUInt8();
	maxLevel = row.ReadUInt8();
	maxLevel = maxLevel == 0 ? 1 : maxLevel;

	std::string strNeedSkills = row.ReadString();

	//针对技能表的格式修改代码
	std::string strNeedSkillLevels = row.ReadString();

	std::vector<std::string> vecSkills;
	std::vector<std::string> vecSkillLevels;

	Avalon::StringUtil::Split(strNeedSkills, vecSkills, split);
	Avalon::StringUtil::Split(strNeedSkillLevels, vecSkillLevels, split);

	if (vecSkills.size() !=0 && vecSkills.size() == vecSkillLevels.size())
	{
		SkillDataLevel skillLevel;
		for (UInt32 i = 0;i < vecSkills.size(); i++)
		{
			skillLevel.id = Avalon::StringUtil::ConvertToValue<UInt16>(vecSkills[0]);
			skillLevel.level = Avalon::StringUtil::ConvertToValue<UInt8>(vecSkillLevels[0]);
			if(skillLevel.id != 0 && skillLevel.level != 0)
			{
				needSkills.push_back(skillLevel);
			}
		}
	}

	/*
	//这里以后可能还会用.
	std::vector<std::string> vecSkills;
	std::string splitd = ";";
	Avalon::StringUtil::Split(strNeedSkills, vecSkills, splitd);
	SkillDataLevel item;
	for (UInt32 i = 0;i < vecSkills.size();i++)
	{
		std::vector<std::string> vecSkill;
		Avalon::StringUtil::Split(vecSkills[i], vecSkill, split);
		if (vecSkill.size() < 2)
		{
			continue;
		}
		item.id = Avalon::StringUtil::ConvertToValue<UInt16>(vecSkill[0]);
		item.level = Avalon::StringUtil::ConvertToValue<UInt8>(vecSkill[1]);
		needSkills.push_back(item);
	}
	*/

	needSP = row.ReadUInt32();

	tmpVal = row.ReadUInt8();
	isForceSync = tmpVal == 1 ? true : false;

	return true;
}

/*
bool SkillDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt16();
	name = row.ReadString();
	occu = row.ReadUInt8();
	usetype = row.ReadUInt8();
	disablegroup = row.ReadUInt8();
	type = row.ReadUInt8();
	needbuff = row.ReadUInt16();
	weaponflag = row.ReadUInt16();
	cdgroup = row.ReadUInt8();
	groupcd = row.ReadUInt32();
	blockflag = row.ReadUInt8();
	scope = row.ReadUInt8();
	scopeshapeparam1 = row.ReadUInt8();
	scopeshapeparam2 = row.ReadUInt8();
	targetflag = row.ReadUInt16();
	distancefloor = row.ReadUInt8();
	distancetop = row.ReadUInt8();
	pkattr = row.ReadUInt8();
	autohook = row.ReadUInt8();

	return true;
}
*/

SkillDataEntryMgr::SkillDataEntryMgr()
{
}

SkillDataEntryMgr::~SkillDataEntryMgr()
{
}

bool SkillDataEntryMgr::AddEntry(SkillDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SkillDataEntry>::AddEntry(entry)) return false;

	if (entry->needSkills.size() != 0)
	{
		for (UInt32 i = 0; i < entry->needSkills.size(); ++i)
		{
			m_PostSkills[entry->needSkills[i].id].push_back(entry->id);
		}
	}

	if (IsAwaken(entry->type))
	{
		for (UInt32 i = 0; i < entry->occus.size(); ++i)
		{
			m_AwakenSkills[entry->occus[i]].push_back(entry);
		}
	}

	if (entry->isStudy)
	{
		for (UInt32 i = 0; i < entry->occus.size(); ++i)
		{
			m_AutoStudySkills[(UInt8)entry->playerLevel][entry->occus[i]].push_back(entry);
		}
	}

	return true;
}

const static SkillDataEntryMgr::SkillIdVec NullSkillIds;
const static SkillDataEntryMgr::SkillVec NullSkills;

const SkillDataEntryMgr::SkillIdVec& SkillDataEntryMgr::GetPostSkills(UInt16 skillId) const
{
	SkillIdVecMap::const_iterator iter = m_PostSkills.find(skillId);
	if (iter != m_PostSkills.end())
	{
		return iter->second;
	}
	return NullSkillIds;
}

const SkillDataEntryMgr::SkillVec& SkillDataEntryMgr::GetAwakenSkills(UInt8 occu) const
{
	AwakenSkillMap::const_iterator iter = m_AwakenSkills.find(occu);
	if (iter != m_AwakenSkills.end())
	{
		return iter->second;
	}
	return NullSkills;
}

const SkillDataEntryMgr::SkillVec& SkillDataEntryMgr::GetAutoStudySkills(UInt8 level, UInt8 occu) const
{
	AutoStudySkillMap::const_iterator iter = m_AutoStudySkills.find(level);
	if (iter != m_AutoStudySkills.end())
	{
		LevelSkillMap::const_iterator levelIter = iter->second.find(occu);
		if (levelIter != iter->second.end())
		{
			return levelIter->second;
		}
	}
	return NullSkills;
}
