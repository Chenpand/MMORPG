#ifndef __CHAMPION_MATCH_SERVICE_H__
#define __CHAMPION_MATCH_SERVICE_H__

#include "PracticeMatchService.h"
#include "ChampionMatchRace.h"

class ChampionMatchService : public PracticeMatchService
{
public:

	ChampionMatchService();

public:
	virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);
	virtual bool OnMatchSuccess(VMatchTeam allTeams);
	virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);
	virtual void DismissMatchRace(const MatchRace* race, bool notice);
protected:
	virtual MatchRace* CreateRace() { return new ChampionMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_CHAMPION; }
};


#endif


