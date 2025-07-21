#ifndef _BS_TEAM_MGR_H_
#define _BS_TEAM_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include "BSTeam.h"


class BSTeamMgr : public Avalon::Singleton<BSTeamMgr>
{
public:
	typedef std::map<UInt64, UInt32> BSPlayerMap;
	typedef std::map<UInt32, BSTeam*> BSTeamMap;

public:
	BSTeamMgr();
	~BSTeamMgr();

	void OnTick(const Avalon::Time& now);

public:
	UInt32 CreateTeam(const BSTeamMember& master);
	UInt32 JoinTeam(UInt32 teamId, const BSTeamMember& member);
	UInt32 LeaveTeam(UInt64 playerId, UInt32 teamId);
	UInt32 DismissTeam(UInt64 playerId, UInt32 teamId);
	UInt32 GetTeamID(UInt64 playerId);

public:
	void AddPlayerMap(UInt64 playerId, UInt32 teamId);
	void DelPlayerMap(UInt64 playerId);

	void AddRobot();

private:


private:
	//��ǰ���Է����ID
	UInt32 m_CurrentID;
	//��ҵĶ���IDӳ��
	BSPlayerMap m_PlayerMap;
	//���еĶ���
	BSTeamMap m_Teams;
	// Tick��ʱ����2000ms
	Avalon::SimpleTimer				m_TickTimer2000ms;
	UInt64 m_RobotPlayerID;
};


#endif //_BS_TEAM_MGR_H_
