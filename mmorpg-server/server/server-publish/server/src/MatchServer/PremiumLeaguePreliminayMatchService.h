#ifndef __PREMIUM_LEAGUE_PRELIMINAY_MATCH_SERVICE_H__
#define __PREMIUM_LEAGUE_PRELIMINAY_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "PremiumLeagueMatchRace.h"

class PremiumLeaguePreliminayMatchService : public PvPMatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
	PremiumLeaguePreliminayMatchService();

	virtual bool Init();

	virtual bool OnTick(const Avalon::Time& now);

	virtual bool OnJoinMatchPool(MatchTeam* team);

protected:
	virtual MatchRace* CreateRace() { return new PremiumLeagueMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO; }
	virtual RaceType GetRaceType() { return RT_PREMIUM_LEAGUE_PRELIMINAY; }
};

#endif