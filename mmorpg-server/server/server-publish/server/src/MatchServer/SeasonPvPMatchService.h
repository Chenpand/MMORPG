#ifndef __SEASON_PVP_MATCH_SERVICE_H__
#define __SEASON_PVP_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "SeasonPvPMatchRace.h"

class SeasonPvPMatchService : public PvPMatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
    SeasonPvPMatchService();

    virtual bool Init();

	virtual bool OnTick(const Avalon::Time& now);

	virtual bool OnJoinMatchPool(MatchTeam* team);

	virtual void OnRaceStarted(ObjID_t sessionId);

protected:
	UInt32 AdjuestMatchScore(MatchTeamMember* member);

protected:
    virtual MatchRace* CreateRace() { return new SeasonPvPMatchRace(); }

};

#endif