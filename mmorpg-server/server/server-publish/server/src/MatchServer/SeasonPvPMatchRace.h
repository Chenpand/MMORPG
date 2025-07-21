#ifndef __SEASON_PVP_MATCH_RACE_H__
#define __SEASON_PVP_MATCH_RACE_H__

#include "PvPMatchRace.h"

/*
    赛季PK
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
    *@brief 计算各种战斗结果下的积分变化
    */
    virtual void CountMatchScoreChanges();

private:
    /**
    *@brief 根据战斗结果计算积分变化
    */
    Int32 _CountMatchScoreChange(UInt32 score, UInt32 opponeScore, MatchRaceResult result);
};

#endif
