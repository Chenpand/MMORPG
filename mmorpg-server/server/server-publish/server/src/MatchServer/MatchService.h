#ifndef __MATCH_SERVICE_H__
#define __MATCH_SERVICE_H__

#include <CLMatchProtocol.h>
#include "MatchTeam.h"
#include "MatchRace.h"
#include "MatchAlgorithmBase.h"

enum MatchServiceType
{
	// PK相关
	MST_RACE_BEGIN,

    // PK
    MST_PVP = MST_RACE_BEGIN,
    // 赛季
    MST_SEASON_PVP,
    // 练习
    MST_PRACTICE,
	// 公会战
	MST_GUILD_BATTLE,
	// 跨服公会战
	MST_CROSS_GUILD_BATTLE,
	// 赏金联赛预选赛
	MST_PREMIUM_LEAGUE_PRELIMINAY,
	// 赏金联赛淘汰赛
	MST_PREMIUM_LEAGUE_BATTLE,
	// PK房间
	MST_PK_ROOM,
	// 3v3匹配
	MST_PK_3V3_MATCH,
	// 3v3活动
	MST_PK_3V3_ACTIVITY,
	// 3v3乱斗
	MST_PK_3V3_TUMBLE,
	//公平竞技场
	MST_EQUAL_PK,
	//公平竞技场好友切磋
	MST_EQUAL_PK_PRACTICE,
	// 2V2乱斗活动
	MST_PK_2V2_ACTIVITY,
	// 冠军赛
	MST_CHAMPION_BATTLE,
	// 5v5匹配
	MST_PK_5V5_MATCH,
	MST_RACE_END = MST_PK_5V5_MATCH,


	// 地下城相关
	MST_DUNGEON_BEGIN,

	// 组队地下城
	MST_DUNGEON_TEAM = MST_DUNGEON_BEGIN,
	// 地下城匹配
	MST_DUNGEON_MATCH,

	MST_DUNGEON_END = MST_DUNGEON_MATCH,

    MST_NUM,
};

//template<class MatchAlgorithm>
class MatchService
{
public:
	// 等待relayserver创建比赛最大时长
	const UInt32 WAIT_RELAY_SERVER_MAX_TIME = 50;
	// 等待查询玩家信息超时时间
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
	*@brief 汇报情况
	*/
	void Report();

	/**
	*@brief 查询战斗信息返回
	*/
	virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info) {}

	/**
	*@brief 查询地下城战斗信息返回
	*/
	virtual void OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info) {}

    /**
    *@brief 解散匹配到的玩家，重新匹配(由rematch决定)
    */
    virtual void DismissMatchRace(const MatchRace* race, bool rematch);

    /**
    *@brief RelayServer开始战斗返回
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address) = 0;

    /**
    *@brief 结算
    */
    virtual void OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const void* raceEnd);

public:
    // 计算队伍的匹配积分，默认就是取平均值
    virtual UInt32 CountTeamMatchScore(MatchTeam* team);

	/**
	*@brief 是不是pk相关的服务
	*/
	bool IsPkService();

	/**
	*@brief 是不是地下城相关的服务
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