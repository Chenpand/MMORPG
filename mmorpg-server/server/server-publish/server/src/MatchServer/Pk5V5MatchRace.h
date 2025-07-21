#ifndef __PK_5V5_MATCH_RACE_H__
#define __PK_5V5_MATCH_RACE_H__

#include "PvPMatchRace.h"

class Pk5V5MatchRace : public PvPMatchRace
{
public:
	Pk5V5MatchRace(PkType pkType);
	virtual ~Pk5V5MatchRace();
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
};

#endif