#ifndef __MATCH_SERVICE_H__
#define __MATCH_SERVICE_H__

#include <CLMatchProtocol.h>
#include "MatchTeam.h"
#include "MatchRace.h"
#include "MatchAlgorithmBase.h"

enum MatchServiceType
{
	// PK���
	MST_RACE_BEGIN,

    // PK
    MST_PVP = MST_RACE_BEGIN,
    // ����
    MST_SEASON_PVP,
    // ��ϰ
    MST_PRACTICE,
	// ����ս
	MST_GUILD_BATTLE,
	// �������ս
	MST_CROSS_GUILD_BATTLE,
	// �ͽ�����Ԥѡ��
	MST_PREMIUM_LEAGUE_PRELIMINAY,
	// �ͽ�������̭��
	MST_PREMIUM_LEAGUE_BATTLE,
	// PK����
	MST_PK_ROOM,
	// 3v3ƥ��
	MST_PK_3V3_MATCH,
	// 3v3�
	MST_PK_3V3_ACTIVITY,
	// 3v3�Ҷ�
	MST_PK_3V3_TUMBLE,
	//��ƽ������
	MST_EQUAL_PK,
	//��ƽ�����������д�
	MST_EQUAL_PK_PRACTICE,
	// 2V2�Ҷ��
	MST_PK_2V2_ACTIVITY,
	// �ھ���
	MST_CHAMPION_BATTLE,
	// 5v5ƥ��
	MST_PK_5V5_MATCH,
	MST_RACE_END = MST_PK_5V5_MATCH,


	// ���³����
	MST_DUNGEON_BEGIN,

	// ��ӵ��³�
	MST_DUNGEON_TEAM = MST_DUNGEON_BEGIN,
	// ���³�ƥ��
	MST_DUNGEON_MATCH,

	MST_DUNGEON_END = MST_DUNGEON_MATCH,

    MST_NUM,
};

//template<class MatchAlgorithm>
class MatchService
{
public:
	// �ȴ�relayserver�����������ʱ��
	const UInt32 WAIT_RELAY_SERVER_MAX_TIME = 50;
	// �ȴ���ѯ�����Ϣ��ʱʱ��
	const UInt32 WAIT_PLAYER_INFO_MAX_TIME = 50;

    MatchService(MatchServiceType type);
    virtual ~MatchService();

public:
    virtual bool Init() = 0;
    virtual bool OnTick(const Avalon::Time& now);
    virtual bool OnTeamStartMatch(MatchTeam* team) = 0;
    virtual bool OnJoinMatchPool(MatchTeam* team) = 0;
    virtual UInt32 OnTeamCancelMatch(ObjID_t teamId) = 0;
    virtual bool OnMatchSuccess(VMatchTeam allTeams) = 0;

	/**
	*@brief �㱨���
	*/
	void Report();

	/**
	*@brief ��ѯս����Ϣ����
	*/
	virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info) {}

	/**
	*@brief ��ѯ���³�ս����Ϣ����
	*/
	virtual void OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info) {}

    /**
    *@brief ��ɢƥ�䵽����ң�����ƥ��(��rematch����)
    */
    virtual void DismissMatchRace(const MatchRace* race, bool rematch);

    /**
    *@brief RelayServer��ʼս������
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address) = 0;

    /**
    *@brief ����
    */
    virtual void OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const void* raceEnd);

public:
    // ��������ƥ����֣�Ĭ�Ͼ���ȡƽ��ֵ
    virtual UInt32 CountTeamMatchScore(MatchTeam* team);

	/**
	*@brief �ǲ���pk��صķ���
	*/
	bool IsPkService();

	/**
	*@brief �ǲ��ǵ��³���صķ���
	*/
	bool IsDungeonService();

public:
    inline MatchServiceType GetType() const { return m_type; }
    inline void SetType(MatchServiceType type) { m_type = type; }

	inline UInt32 GetNodeID() const { return m_nodeid; }
	inline void SetNodeID(UInt32 nodeid) { m_nodeid = nodeid; }

    bool AddTeam(MatchTeam* team);
    MatchTeam* FindTeam(ObjID_t id);
    void DelTeam(ObjID_t id);

    bool AddRace(MatchRace* race);
    MatchRace* FindRace(ObjID_t id);
    void DelRace(ObjID_t id);

protected:
    inline void SetAlgorithm(Match::MatchAlgorithm* algorithm) { m_matchAlgorithm = algorithm; }
    virtual MatchRace* CreateRace() = 0;
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }

protected:
    MatchServiceType                m_type;
	UInt32							m_nodeid;
    std::map<ObjID_t, MatchTeam*>   m_mapTeams;
	std::map<ObjID_t, MatchRace*>   m_mapRaces;
    Match::MatchAlgorithm*          m_matchAlgorithm;
};

#endif