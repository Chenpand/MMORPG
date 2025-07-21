#ifndef __2V2_MATCH_SERVICE_H__
#define __2V2_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "Pk2V2MatchRace.h"


class Pk2V2ActivityMatchService :
	public PvPMatchService
{
public:
	Pk2V2ActivityMatchService();
	~Pk2V2ActivityMatchService();

	virtual bool Init();
	virtual UInt32 CountTeamMatchScore(MatchTeam* team);
	virtual bool OnMatchSuccess(VMatchTeam allTeams);
	virtual bool OnJoinMatchPool(MatchTeam* team);

protected:
	virtual UInt32 AdjuestMatchScore(MatchTeamMember* member);

protected:
	virtual MatchRace* CreateRace() { return new Pk2V2MatchRace(PK_2V2_ACTIVITY); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_2V2_TUMNLE; }
};

#endif