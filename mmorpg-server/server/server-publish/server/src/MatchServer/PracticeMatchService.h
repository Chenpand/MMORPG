#ifndef __PRACTICE_MATCH_SERVICE_H__
#define __PRACTICE_MATCH_SERVICE_H__

#include "MatchService.h"
#include "PracticeMatchRace.h"

class PracticeMatchService : public MatchService
{
public:
    PracticeMatchService();

    virtual bool Init();
    virtual bool OnTeamStartMatch(MatchTeam* team);
    virtual bool OnJoinMatchPool(MatchTeam* team);
    virtual UInt32 OnTeamCancelMatch(ObjID_t teamId);
    virtual bool OnMatchSuccess(VMatchTeam allTeams);
    virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

    /**
    *@brief RelayServer开始战斗返回
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
    virtual MatchRace* CreateRace() { return new PracticeMatchRace(); }
	virtual RaceType GetRaceType() { return RT_PK; }
	/**
	*@brief 随机一个PK场景
	*/
	virtual UInt32 GenPkDungeon();

protected:
	// pk地下城列表
	std::vector<UInt32>				m_pkDungeonIdList;
};

#endif