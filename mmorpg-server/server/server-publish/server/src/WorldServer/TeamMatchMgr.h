#ifndef __TEAM_MATCH_MGR_H__
#define __TEAM_MATCH_MGR_H__

#include "TeamMatchDefine.h"
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <AvalonProtocol.h>

// ���³�ƥ���
struct TeamDungeonMatchPool
{
	TeamDungeonMatchPool()
	{
		matchType = TEAM_MATCH_TYPE_TEAM;
		fullPlayerNum = 3;
	}

	UInt32							targetId;
	TeamMatchType					matchType;
	UInt32							fullPlayerNum;
	std::vector<TeamMatchEntry*>	entrys;
};

// ƥ����
struct TeamMatchResult
{
	TeamMatchResult()
	{
		target = 0;
		dungeonId = 0;
		matchType = TEAM_MATCH_TYPE_TEAM;
		doResultTime = 0;
	}

	UInt32							target;
	UInt32							dungeonId;
	TeamMatchType					matchType;
	std::vector<TeamMatchEntry*>	entrys;
	// ����ƥ����ʱ��(ms)
	UInt64							doResultTime;
};

class TeamMatchMgr : public Avalon::Singleton<TeamMatchMgr>
{
public:
	TeamMatchMgr();
	~TeamMatchMgr();

	/**
	*@brief ��ʼ��
	*/
	bool OnInit();

	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

public:
	// ����¼�

	/**
	*@brief �������ʼƥ��
	*/
	UInt32 OnPlayerJoinMatch(WSPlayer* player, UInt32 dungeonId);

	/**
	*@brief ���ȡ��ƥ��
	*/
	UInt32 OnPlayerCancelMatch(WSPlayer* player);

	/**
	*@brief ����ȡ��ƥ��
	*/
	UInt32 OnTeamCancelMatch(WSTeam* team);

private:
	// ƥ���㷨

	/**
	*@brief ����ƥ����
	*/
	void _DoMatchResult();

	/**
	*@brief ��ƥ�����ƥ��
	*/
	void _DoMatch();

	/**
	*@brief ���Լ����Ķ���
	*/
	void _DoJoinTeamMatch();

	/**
	*@brief ���Լ����Ķ��飬���سɹ���ʧ��
	*/
	bool _TryJoinOtherTeam(UInt32 target, UInt32 fullPlayerNum, TeamMatchEntry* entry);

	/**
	*@brief ��ʼͶƱ����ս��
	*/
	void _OnStartRaceVote();

	/**
	*@brief ƥ��ɹ�
	*/
	bool _OnMatchSuccess(TeamMatchResult result);

	/**
	*@brief ��ƥ��ذ��������������
	*/
	std::vector<std::vector<TeamMatchEntry*>> _GenMatchEntryBySize(TeamDungeonMatchPool& pool);

private:
	// �ڲ�����

	/**
	*@brief ����ƥ�䵥Ԫ
	*/
	TeamMatchEntry* _FindEntryByPlayer(WSPlayer* player);

	/**
	*@brief ����ƥ�䵥Ԫ
	*/
	TeamMatchEntry* _FindEntryByTeam(WSTeam* team);

	/**
	*@brief ɾ��ƥ�䵥Ԫ
	*/
	void _DelEntry(TeamMatchEntry* entry, bool destoryObject);

	/**
	*@brief �㲥ͶƱ���������Ϣ
	*/
	void _BroadcastNotifyRaceVote(TeamMatchResult& result);
	
	/**
	*@brief �Ƿ���ͬ
	*/
	bool _IsSame(TeamMatchEntry* entry, WSPlayer* player);

	/**
	*@brief �Ƿ���ͬ
	*/
	bool _IsSame(TeamMatchEntry* entry, WSTeam* team);

	/**
	*@brief �㲥ȡ��ƥ��
	*/
	void _BroadMatchCancel(TeamMatchEntry* entry);

	/**
	*@brief �㲥��Ϣ
	*/
	void _Broadcast(TeamMatchEntry* entry, Avalon::Protocol& protocol);

private:
	// ƥ���
	HashMap<UInt32, TeamDungeonMatchPool>			m_MatchPool;
	// ƥ����ɿ�ʼս��ͶƱ
	std::vector<TeamMatchResult>					m_Results;
	// ƥ�䶨ʱ��
	Avalon::SimpleTimer								m_MatchTimer;
};

#endif