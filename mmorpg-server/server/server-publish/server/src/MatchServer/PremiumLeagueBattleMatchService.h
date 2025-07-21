#ifndef __PREMIUM_LEAGUE_BATTLE_MATCH_SERVICE_H__
#define __PREMIUM_LEAGUE_BATTLE_MATCH_SERVICE_H__

#include "PracticeMatchService.h"
#include "PremiumLeagueMatchRace.h"

class PremiumLeagueBattleMatchService : public PracticeMatchService
{
public:
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;

	PremiumLeagueBattleMatchService();

public:
	virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
	virtual MatchRace* CreateRace() { return new PremiumLeagueMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_PREMIUM_LEAGUE_BATTLE; }
};

#endif