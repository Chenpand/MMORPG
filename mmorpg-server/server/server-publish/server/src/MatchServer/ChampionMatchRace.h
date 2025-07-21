#ifndef __CHAMPION_MATCH_RACE_H__
#define __CHAMPION_MATCH_RACE_H__

#include "PvPMatchRace.h"


class ChampionMatchRace : public PvPMatchRace
{
public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
	/**
	*@brief 计算各种战斗结果下的积分变化
	*/
protected:
	virtual void CountMatchScoreChanges();
};

#endif