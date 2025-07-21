#ifndef __PREMIUM_LEAGUE_MATCH_RACE_H__
#define __PREMIUM_LEAGUE_MATCH_RACE_H__

#include "PvPMatchRace.h"

class PremiumLeagueMatchRace : public PvPMatchRace
{
public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
};

#endif
