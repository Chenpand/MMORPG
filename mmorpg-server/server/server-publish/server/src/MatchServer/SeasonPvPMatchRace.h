#ifndef __SEASON_PVP_MATCH_RACE_H__
#define __SEASON_PVP_MATCH_RACE_H__

#include "PvPMatchRace.h"

/*
    ����PK
*/

class SeasonPvPMatchRace : public PvPMatchRace
{
public:
    SeasonPvPMatchRace();
    virtual ~SeasonPvPMatchRace();

public:
    virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
    /**
    *@brief �������ս������µĻ��ֱ仯
    */
    virtual void CountMatchScoreChanges();

private:
    /**
    *@brief ����ս�����������ֱ仯
    */
    Int32 _CountMatchScoreChange(UInt32 score, UInt32 opponeScore, MatchRaceResult result);
};

#endif
