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
	*@brief 心跳事件
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 匹配算法，为每个玩家寻找对手
	*/
	void OnMatching();

	/**
	*@brief 取消匹配
	*/
	UInt32 OnCancelMatch(ObjID_t id);

	/**
	*@brief 匹配成功，开始比赛
	*/
	bool OnMatchSuccess(const VMatchPlayer& allPlayers);

	/**
	*@brief RelayServer开始战斗返回
	*/
	void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

	/**
	*@brief 战斗过长，超时
	*/
	void OnRaceTimeOut(MatchResult* matchResult);

    /**
    *@brief world server 返回玩家信息
    */
    void OnQueryPlayerInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

    /**
    *@brief 解散匹配到的玩家，重新匹配(由playerRematch决定)
    */
    void DismissMatchResult(const MatchResult* matchResult, bool playerRematch = true);

    /**
    *@brief 结算
    */
    void OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const std::vector<FightergResult>& results);

    /**
    *@brief 结算
    */
    void OnRelayServerShutDown(UInt32 serverID);

	/**
	*@brief 计算Session
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

    // 匹配算法
    TraditionMatchAlgorithm     m_tradAlgorithm;
};

#endif