#ifndef __PK_3V3_MATCH_SERVICE_H__
#define __PK_3V3_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "PkRoomMatchRace.h"

class Pk3V3MatchService : public PvPMatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
	Pk3V3MatchService();

	virtual bool Init();

	virtual bool OnTick(const Avalon::Time& now);

	virtual bool OnJoinMatchPool(MatchTeam* team);

protected:
	virtual UInt32 AdjuestMatchScore(MatchTeamMember* member);

protected:
	virtual MatchRace* CreateRace() { return new PkRoomMatchRace(PK_3V3_MATCH); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_PK_3V3; }
};

#endif