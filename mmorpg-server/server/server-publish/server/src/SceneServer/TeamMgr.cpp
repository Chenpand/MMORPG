#include "TeamMgr.h"

TeamMgr::TeamMgr()
{
}

TeamMgr::~TeamMgr()
{
	for(TeamMap::iterator iter = m_Teams.begin();
		iter != m_Teams.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_Teams.clear();
}

bool TeamMgr::AddTeam(Team* team)
{
	if(team == NULL) return false;

	TeamMap::iterator iter = m_Teams.find(team->GetId());
	if(iter != m_Teams.end())
	{
		if(iter->second->GetMemberNumInScene() == 0)	//没有成员了
		{
			CL_SAFE_DELETE(iter->second);
			m_Teams.erase(iter);
		}
		else return false;
	}
	return m_Teams.insert(std::make_pair(team->GetId(), team)).second;
}

void TeamMgr::RemoveTeam(Team* team)
{
	if(team == NULL) return;

	m_Teams.erase(team->GetId());
}

Team* TeamMgr::FindTeam(UInt32 id)
{
	TeamMap::iterator iter = m_Teams.find(id);
	if(iter != m_Teams.end()) return iter->second;
	return NULL;
}

Team* TeamMgr::GetRandTeam()
{
    if (m_Teams.empty())
    {
        return NULL;
    }

    return m_Teams.begin()->second;
}