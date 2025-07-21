#ifndef __PRACTICE_MATCH_RACE_H__
#define __PRACTICE_MATCH_RACE_H__

#include "MatchRace.h"

class PracticeMatchRace : public MatchRace
{
public:
    PracticeMatchRace();
    virtual ~PracticeMatchRace();

public:
    virtual bool OnCreate(VMatchTeam teams);
    virtual bool OnRelayServerCreateRaceRet();
    virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
};

#endif
