#ifndef _WS_TEAM_MGR_H_
#define _WS_TEAM_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <AvalonDailyTimer.h>
#include <map>
#include <set>
#include "WSTeam.h"

/**
 *@brief ���������
 */
class WSTeamMgr : public Avalon::Singleton<WSTeamMgr>
{
	typedef std::map<UInt32, WSTeam*>	TeamMap;

    // һ�β�ѯ����������
    static const UInt32 QUERY_MAX_TEAM_NUM_ONCE = 10;
	static const UInt32 MAX_TEAM_NUMBER = 9999;

public:
	WSTeamMgr();
	~WSTeamMgr();

public:
	/**
	 *@brief ���һ������
	 */
	bool AddTeam(WSTeam* team);
	void RemoveTeam(WSTeam* team);
	WSTeam* FindTeam(UInt32 id);

	/**
	 *@brief �������ж���
	 */
	void VisitTeams(WSTeamVisitor& visitor);

    /**
    *@brief ��ѯ�����б�
    */
    void OnQueryTeamList(WSPlayer* player, const CLProtocol::WorldQueryTeamList& req);

	/**
	*@brief ��ѯһ��ָ��Ŀ��������Ķ���
	*/
	WSTeam* OnQueryTeamByTargetAndNum(WSTeam* srcTeam, UInt32 target, UInt32 num);

	/**
	*@brief �ϲ���������
	*/
	WSTeam* CombineTeam(std::vector<WSTeam*> teams);

public:// �¼�
	/**
	 *@brief �������
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����һ��
	 */
	void OnDayChanged();

protected:
    /**
    *@brief �Ƿ�ƥ���ѯ����
    */
	bool IsMatchQueryCond(WSPlayer* player, WSTeam* team, const CLProtocol::WorldQueryTeamList& req);

	/**
	*@brief ���ḱ����ӵ�ƥ��
	*/
	bool GuildDungeonCond(WSPlayer* player, WSTeam* team);

	/**
	*@brief ��ȡ������
	*/
	UInt16 GenTeamNumber();

private:
	//id������
	UInt32				m_TeamIdSerial;
	//�����б�
	TeamMap				m_Teams;
	//������(�����ſ��ظ�ʹ��)
	std::set<UInt16>	m_TeamNumbers;
	// ÿ�ն�ʱ��
	Avalon::DailyTimer	m_DailyTimer;
};

#endif
