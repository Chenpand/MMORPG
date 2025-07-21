#ifndef _TEAM_MGR_H_
#define _TEAM_MGR_H_

#include <AvalonSingleton.h>
#include <map>
#include "Team.h"

/**
 *@brief ���������
 */
class TeamMgr : public Avalon::Singleton<TeamMgr>
{
	typedef std::map<UInt32, Team*> TeamMap;

public:
	TeamMgr();
	~TeamMgr();

public:
	/**
	 *@brief ���ɾ�����Ҷ���
	 */
	bool AddTeam(Team* team);
	void RemoveTeam(Team* team);
	Team* FindTeam(UInt32 id);

    Team* GetRandTeam();

private:
	//�����б�
	TeamMap	m_Teams;
};

#endif
