#ifndef __5V5_MATCH_SERVICE_H__
#define __5V5_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "Pk5V5MatchRace.h"


class Pk5V5MatchService :
	public PvPMatchService
{
public:
	Pk5V5MatchService();
	~Pk5V5MatchService();

	virtual bool Init();
	virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

    /**
    *@brief RelayServer开始战斗返回
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
	virtual UInt32 GetWaitMatchUntilRobotSec(SeasonMainLevel level = SML_BRONZE) const { return 15; };

protected:
	virtual MatchRace* CreateRace() { return new Pk5V5MatchRace(PK_5V5_MATCH); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_PK_5V5; }
};

#endif