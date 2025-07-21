#ifndef __MATCH_SERVICE_MGR_H__
#define __MATCH_SERVICE_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLMatchProtocol.h>
#include <CLDungeonDefine.h>
#include "MatchService.h"
#include "MatchServicePlayer.h"

struct MatchServiceKey
{
	MatchServiceKey(UInt32 zoneid_ = 0, MatchServiceType type_ = MST_PVP) : zoneid(zoneid_), type(type_) {}
	bool operator<(const MatchServiceKey& key) const
	{
		if (zoneid != key.zoneid)
		{
			return zoneid < key.zoneid;
		}
		return type < key.type;
	}

	UInt32				zoneid;
	MatchServiceType	type;
};

class MatchServiceMgr : public Avalon::Singleton<MatchServiceMgr>
{
public:
    MatchServiceMgr();
    ~MatchServiceMgr();

    bool Init();
    void OnTick(const Avalon::Time& now);

	void OnWorldServerConnected(UInt32 nodeid);

public:
    /**
    *@brief ����PVPƥ��
    */
	UInt32 OnPlayerStartPvPMatch(UInt32 wsId, const CLProtocol::MatchServerMatchStartReq& req);

	/**
	*@brief ���󹫻�սƥ��
	*/
	UInt32 OnPlayerStartGuildBattleMatch(UInt32 wsId, const CLProtocol::MatchServerGuildBattleMatchReq& req);

	/**
	*@brief �����ͽ�����ƥ��
	*/
	UInt32 OnPlayerStartPremiumLeagueMatch(UInt32 wsId, const CLProtocol::MatchServerPremiumLeagueMatchReq& req);

    /**
    *@brief ������³����ս��
    */
    bool OnPlayerStartDungeonTeamRace(UInt32 wsId, UInt32 wsTeamId, const SceneNpc& cityMonster, UInt32 dungeonId, UInt8 isHell, const std::vector<CLProtocol::Fighter>& fighters);

	/**
	*@brief ������³�ƥ��
	*/
	bool OnPlayerStartDungeonMatch(UInt32 wsId, UInt32 wsTeamId, UInt32 dungeonId, UInt8 isHell, const std::vector<MatchPlayerBaseInfo>& fighters, UInt32 level);

	/**
	*@brief ����PK����ս��
	*/
	UInt32 OnPlayerStartPkRoom(UInt32 wsId, UInt32 wsRoomId, const std::vector<PkRoomMatchPlayerInfo>& fighters, UInt8 pkType);

    /**
    *@brief ������ϰ��
    */
    bool OnPlayerStartPracticeMatch(UInt32 wsId, std::vector<ObjID_t> roleIds,UInt8 type);

	/**
	*@brief ������ϰ��
	*/
	bool OnPlayerStartPremiumLeagueBattle(UInt32 wsId, ObjID_t battleId, std::vector<ObjID_t> roleIds);

	/**
	*@brief ����ھ���
	*/
	bool OnPlayerStartChampionBattle(UInt32 usId, UInt32 groupID, UInt32 order, std::vector<ObjID_t> roleIds);

    /**
    *@brief ȡ��ƥ��
    */
    UInt32 OnPlayerCancelMatch(ObjID_t id);

    /**
    *@brief world server �������ս����Ϣ
    */
    void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

	/**
	*@brief world server ������ҵ��³�ս����Ϣ
	*/
	void OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info);

    /**
    *@brief RelayServer��ʼս������
    */
    void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

    /**
    *@brief PvP����
    */
    void OnPvPRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo);

    /**
    *@brief ���³ǽ���
    */
    void OnDungeonRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const DungeonRaceEndInfo& raceEndInfo);

public:
    /**
    *@brief �������
    */
    MatchServicePlayer* FindPlayer(ObjID_t id);

    /**
    *@brief ɾ�����
    */
    void DelPlayer(ObjID_t id);

    /**
    *@brief �������
    */
    MatchServicePlayer* CreatePlayer(UInt32 wsId, ObjID_t id, UInt32 accid, std::string name);


public:
    /**
    *@brief �������ID
    */
    ObjID_t GenRaceID();

private:
    /**
    *@brief ��ȡƥ�����
    */
    MatchService* _GetMatchService(UInt32 nodeid, MatchServiceType type);

	/**
	*@brief ����ƥ�����
	*/
	MatchService* _CreateMatchService(UInt32 nodeid, MatchServiceType type);

	/**
	*@brief ����������ƥ�����
	*/
	void _CreateSingleZoneMatchServices(UInt32 nodeid);

	/**
	*@brief ���������ƥ�����
	*/
	void _CreateCrossZoneMatchServices();

	/**
	*@brief �Ƿ���ƥ��
	*/
	bool _IsCrossZoneMatch(MatchServiceType type);

private:
	std::map<MatchServiceKey, MatchService*>		m_services;

    // ά����ҵļ���Ϣ����Ҫ�����������Ķ���ͱ���ID
    HashMap<ObjID_t, MatchServicePlayer*>			m_players;

	// �㱨ƥ����Ϣ��ʱ��
	Avalon::SimpleTimer								m_reportTimer;
};

#endif