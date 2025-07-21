#ifndef __MATCH_MGR_H__
#define __MATCH_MGR_H__

#include <AvalonSocket.h>
#include <AvalonSingleton.h>
#include <map>

#include "CLMatchProtocol.h"
#include "MatchPlayer.h"
#include "MatchResult.h"
#include "TraditionMatchAlgorithm.h"

class MatchMgr : public Avalon::Singleton<MatchMgr>
{
public:
	typedef HashMap<ObjID_t, MatchPlayer*>	MapMatchPlayer;
	typedef HashMap<ObjID_t, MatchResult*>	MapMatchResult;
    typedef HashMap<UInt32, MapMatchResult> RelayServerMatchResultSet;

public:
	MatchMgr();
	~MatchMgr();

public:
	bool Init();
	void Final();

	/**
	*@brief �����¼�
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ƥ���㷨��Ϊÿ�����Ѱ�Ҷ���
	*/
	void OnMatching();

	/**
	*@brief ȡ��ƥ��
	*/
	UInt32 OnCancelMatch(ObjID_t id);

	/**
	*@brief ƥ��ɹ�����ʼ����
	*/
	bool OnMatchSuccess(const VMatchPlayer& allPlayers);

	/**
	*@brief RelayServer��ʼս������
	*/
	void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

	/**
	*@brief ս����������ʱ
	*/
	void OnRaceTimeOut(MatchResult* matchResult);

    /**
    *@brief world server ���������Ϣ
    */
    void OnQueryPlayerInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

    /**
    *@brief ��ɢƥ�䵽����ң�����ƥ��(��playerRematch����)
    */
    void DismissMatchResult(const MatchResult* matchResult, bool playerRematch = true);

    /**
    *@brief ����
    */
    void OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const std::vector<FightergResult>& results);

    /**
    *@brief ����
    */
    void OnRelayServerShutDown(UInt32 serverID);

	/**
	*@brief ����Session
	*/
	static UInt64 GenGameSession();

    static UInt8 GetCampByPos(UInt8 pos) { return pos < 5 ? 0 : 1; }

public:
	UInt32 AddMatchPlayer(MatchPlayer* player);
	bool DelMatchPlayerByID(ObjID_t id);
	MatchPlayer* FindMatchPlayerByID(ObjID_t id);

    bool PlayerStartMatching(MatchPlayer* player);
    void PlayerStopMatching(ObjID_t roldId);

    void AddMatchResult(MatchResult* result);
    void AddMatchResultToRelayServerSet(MatchResult* result);
	MatchResult* FindMatchResult(ObjID_t id);
    void DeleteMatchResult(ObjID_t id);

	bool IsPlayerCanStartMatch(ObjID_t id);

private:
	MapMatchPlayer			    m_MatchPlayers;
	MapMatchResult			    m_MatchResults;
    RelayServerMatchResultSet   m_RelayServerMatchResultSet;

    // ƥ���㷨
    TraditionMatchAlgorithm     m_tradAlgorithm;
};

#endif