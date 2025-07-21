#ifndef _TEAM_MGR_H_
#define _TEAM_MGR_H_

#include <AvalonSingleton.h>
#include <map>
#include "Team.h"

/**
 *@brief 队伍管理器
 */
class TeamMgr : public Avalon::Singleton<TeamMgr>
{
	typedef std::map<UInt32, Team*> TeamMap;

public:
	TeamMgr();
	~TeamMgr();

public:
	/**
	 *@brief 添加删除查找队伍
	 */
	bool AddTeam(Team* team);
	void RemoveTeam(Team* team);
	Team* FindTeam(UInt32 id);

    Team* GetRandTeam();

private:
	//队伍列表
	TeamMap	m_Teams;
};

#endif
