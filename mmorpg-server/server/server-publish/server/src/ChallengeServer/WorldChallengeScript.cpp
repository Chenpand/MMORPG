#include "WorldChallengeScript.h"

#include <sstream>

WorldChallengeScript::WorldChallengeScript(Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	m_BeginTime = m_EndTime = 0;
}

WorldChallengeScript::~WorldChallengeScript()
{

}

void WorldChallengeScript::Init()
{
	Call<void>("Init", this);
}

UInt32 WorldChallengeScript::GetGroupByLevel(UInt16 level) const
{
	for(ChallengeGroupVec::const_iterator iter = m_Groups.begin();
		iter != m_Groups.end(); ++iter)
	{
		if(level >= iter->minLevel && level <= iter->maxLevel) return iter->id;
	}
	return 0;
}

UInt32 WorldChallengeScript::GetPreGroupByGroup(UInt32 id) const
{
	UInt16 minLevel = 0;
	for(ChallengeGroupVec::const_iterator iter = m_Groups.begin();
		iter != m_Groups.end(); ++iter)
	{
		if(id == iter->id)
		{
			minLevel = iter->minLevel;
			break;
		}
	}
	if(minLevel == 0) return 0;
	return GetGroupByLevel(minLevel - 1);
}

const ChallengeGroup* WorldChallengeScript::GetGroup(UInt32 group) const
{
	for(ChallengeGroupVec::const_iterator iter = m_Groups.begin();
		iter != m_Groups.end(); ++iter)
	{
		if(iter->id == group) return &(*iter);
	}
	return NULL;
}

void WorldChallengeScript::SetActivityTime(const char* begintime, const char* endtime)
{
	m_BeginTime = StrToTime(begintime);
	m_EndTime = StrToTime(endtime);
}

void WorldChallengeScript::SetGroupInfo(UInt32 id, UInt16 minLevel, UInt16 maxLevel)
{
	ChallengeGroup group;
	group.id = id;
	group.minLevel = minLevel;
	group.maxLevel = maxLevel;
	m_Groups.push_back(group);
}


UInt32 WorldChallengeScript::StrToTime(const std::string& str)
{
	std::istringstream oss(str);

	UInt32 hour = 0;
	UInt32 min = 0;
	UInt32 sec = 0;
	char split = 0;
	
	oss >> hour >> split >> min >> split >> sec;
	return hour * HOUR_TO_SEC + min * MIN_TO_SEC + sec;
}
