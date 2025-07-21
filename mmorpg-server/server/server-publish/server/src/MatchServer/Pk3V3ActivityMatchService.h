#ifndef __PK_3V3_ACTIVITY_MATCH_SERVICE_H__
#define __PK_3V3_ACTIVITY_MATCH_SERVICE_H__

#include "PK3V3MatchService.h"
#include "Pk3V3ScoreWarMatchRace.h"

class Pk3V3ActivityMatchService : public Pk3V3MatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
	Pk3V3ActivityMatchService() {
		SetType(MST_PK_3V3_ACTIVITY);
	}

protected:
	virtual UInt32 AdjuestMatchScore(MatchTeamMember* member);

protected:
	virtual MatchRace* CreateRace() { return new Pk3V3ScoreWarMatchRace(PK_3V3_ACTIVITY); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_SCORE_WAR; }
};

#endif