#ifndef __PVP_MATCH_SERVICE_H__
#define __PVP_MATCH_SERVICE_H__

#include "MatchService.h"
#include "PvPMatchRace.h"
#include <CLSeasonDefine.h>

class PvPMatchService : public MatchService
{
	// 最多保存的历史对手记录
	const UInt32 MAX_PVP_HISTORY_OPPONENT = 3;
public:
    PvPMatchService(bool useRobot = false);
	virtual ~PvPMatchService();

	virtual bool OnTick(const Avalon::Time& now);
    virtual bool Init();
    virtual bool OnTeamStartMatch(MatchTeam* team);
    virtual bool OnJoinMatchPool(MatchTeam* team);
    virtual UInt32 OnTeamCancelMatch(ObjID_t teamId);
    virtual bool OnMatchSuccess(VMatchTeam allTeams);
    virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

	virtual void OnRaceStarted(ObjID_t sessionId) {}

    /**
    *@brief RelayServer开始战斗返回
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
	bool OnStartRaceWithRobot(MatchTeam* team);
	RobotAIHardType GenRobotAIHard(MatchTeamMember* member, UInt16 level);
	RobotAttrHardType GenRobotAttrHard(MatchTeamMember* member, UInt16 level);
	UInt32 GenRobotMatchScore(UInt32 score);

protected:
	/**
	*@brief 添加匹配历史
	*/
	void AddHistory(ObjID_t player1, ObjID_t player2);

	/**
	*@brief 获取上一个对手
	*/
	std::vector<ObjID_t> GetLastOpponent(ObjID_t id);

	/**
	*@brief 随机一个PK场景
	*/
	virtual UInt32 GenPkDungeon();

	/**
	*@brief 随机计算PK段位
	*/
	UInt32 _GenRobotPkValue(UInt32 pkValue);

	/**
	*@brief 随机计算赛季段位
	*/
	UInt32 _GenRobotSeasonLevel(UInt32 seasonLevel);

protected:
    virtual MatchRace* CreateRace() { return new PvPMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_PK; }

protected:
	virtual UInt32 GetWaitMatchUntilRobotSec(SeasonMainLevel level = SML_BRONZE) const;

protected:
	bool		m_useRobot;

	// 匹配历史
	HashMap<ObjID_t, std::list<ObjID_t>>		m_mathchHistory;

	// pk地下城列表
	std::vector<UInt32>							m_pkDungeonIdList;
};

#endif