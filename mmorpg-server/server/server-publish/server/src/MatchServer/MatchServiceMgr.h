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
    *@brief 请求PVP匹配
    */
	UInt32 OnPlayerStartPvPMatch(UInt32 wsId, const CLProtocol::MatchServerMatchStartReq& req);

	/**
	*@brief 请求公会战匹配
	*/
	UInt32 OnPlayerStartGuildBattleMatch(UInt32 wsId, const CLProtocol::MatchServerGuildBattleMatchReq& req);

	/**
	*@brief 请求赏金联赛匹配
	*/
	UInt32 OnPlayerStartPremiumLeagueMatch(UInt32 wsId, const CLProtocol::MatchServerPremiumLeagueMatchReq& req);

    /**
    *@brief 请求地下城组队战斗
    */
    bool OnPlayerStartDungeonTeamRace(UInt32 wsId, UInt32 wsTeamId, const SceneNpc& cityMonster, UInt32 dungeonId, UInt8 isHell, const std::vector<CLProtocol::Fighter>& fighters);

	/**
	*@brief 请求地下城匹配
	*/
	bool OnPlayerStartDungeonMatch(UInt32 wsId, UInt32 wsTeamId, UInt32 dungeonId, UInt8 isHell, const std::vector<MatchPlayerBaseInfo>& fighters, UInt32 level);

	/**
	*@brief 请求PK房间战斗
	*/
	UInt32 OnPlayerStartPkRoom(UInt32 wsId, UInt32 wsRoomId, const std::vector<PkRoomMatchPlayerInfo>& fighters, UInt8 pkType);

    /**
    *@brief 请求练习赛
    */
    bool OnPlayerStartPracticeMatch(UInt32 wsId, std::vector<ObjID_t> roleIds,UInt8 type);

	/**
	*@brief 请求练习赛
	*/
	bool OnPlayerStartPremiumLeagueBattle(UInt32 wsId, ObjID_t battleId, std::vector<ObjID_t> roleIds);

	/**
	*@brief 请求冠军赛
	*/
	bool OnPlayerStartChampionBattle(UInt32 usId, UInt32 groupID, UInt32 order, std::vector<ObjID_t> roleIds);

    /**
    *@brief 取消匹配
    */
    UInt32 OnPlayerCancelMatch(ObjID_t id);

    /**
    *@brief world server 返回玩家战斗信息
    */
    void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

	/**
	*@brief world server 返回玩家地下城战斗信息
	*/
	void OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info);

    /**
    *@brief RelayServer开始战斗返回
    */
    void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

    /**
    *@brief PvP结算
    */
    void OnPvPRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo);

    /**
    *@brief 地下城结算
    */
    void OnDungeonRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const DungeonRaceEndInfo& raceEndInfo);

public:
    /**
    *@brief 查找玩家
    */
    MatchServicePlayer* FindPlayer(ObjID_t id);

    /**
    *@brief 删除玩家
    */
    void DelPlayer(ObjID_t id);

    /**
    *@brief 创建玩家
    */
    MatchServicePlayer* CreatePlayer(UInt32 wsId, ObjID_t id, UInt32 accid, std::string name);


public:
    /**
    *@brief 计算比赛ID
    */
    ObjID_t GenRaceID();

private:
    /**
    *@brief 获取匹配服务
    */
    MatchService* _GetMatchService(UInt32 nodeid, MatchServiceType type);

	/**
	*@brief 创建匹配服务
	*/
	MatchService* _CreateMatchService(UInt32 nodeid, MatchServiceType type);

	/**
	*@brief 创建单服的匹配服务
	*/
	void _CreateSingleZoneMatchServices(UInt32 nodeid);

	/**
	*@brief 创建跨服的匹配服务
	*/
	void _CreateCrossZoneMatchServices();

	/**
	*@brief 是否跨服匹配
	*/
	bool _IsCrossZoneMatch(MatchServiceType type);

private:
	std::map<MatchServiceKey, MatchService*>		m_services;

    // 维护玩家的简单信息，主要是玩家相关联的队伍和比赛ID
    HashMap<ObjID_t, MatchServicePlayer*>			m_players;

	// 汇报匹配信息定时器
	Avalon::SimpleTimer								m_reportTimer;
};

#endif