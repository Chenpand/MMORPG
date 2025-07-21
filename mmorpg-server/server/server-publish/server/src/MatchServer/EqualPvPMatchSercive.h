#ifndef __EQUAL_PVP_MATCH_SERVICE_H__
#define __EQUAL_PVP_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "EuqalPvPMatchRace.h"
#include "PracticeMatchService.h"

class EqualPvPMatchSercive : public PvPMatchService
{

public:
	EqualPvPMatchSercive(bool useRobot = false);
	virtual ~EqualPvPMatchSercive();
protected:
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_EQUAL_RACE_INFO; }
	virtual MatchRace* CreateRace() { return new EuqalPvPMatchRace(); }
	virtual RaceType GetRaceType() { return RT_EQULA_PK; }
};

class EqualPvPPracticeMatchService : public PracticeMatchService
{
public:
	EqualPvPPracticeMatchService();
	virtual ~EqualPvPPracticeMatchService();
protected:
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_EQUAL_RACE_INFO; }
	virtual MatchRace* CreateRace() { return new EuqalPracticeMatchRace(); }
	virtual RaceType GetRaceType() { return RT_EQULA_PK; }
};


#endif
