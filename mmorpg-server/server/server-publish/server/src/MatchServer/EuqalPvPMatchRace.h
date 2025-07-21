#ifndef __EQUAL_PVP_MATCH_RACE_H__
#define __EQUAL_PVP_MATCH_RACE_H__


#include "PvPMatchRace.h"
#include "PracticeMatchRace.h"

class EuqalPvPMatchRace : public PvPMatchRace
{
public:
	EuqalPvPMatchRace();
	virtual ~EuqalPvPMatchRace();
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
};

class EuqalPracticeMatchRace : public PracticeMatchRace
{
public:
	EuqalPracticeMatchRace();
	virtual ~EuqalPracticeMatchRace();
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
};



#endif

