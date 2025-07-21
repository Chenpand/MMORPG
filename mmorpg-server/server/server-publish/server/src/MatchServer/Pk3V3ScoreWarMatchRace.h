#ifndef __PK_3V3_SCORE_WAR_MATCH_RACE_H__
#define __PK_3V3_SCORE_WAR_MATCH_RACE_H__

#include "PvPMatchRace.h"

/*
	3v3积分赛
*/

class Pk3V3ScoreWarMatchRace : public PvPMatchRace
{
public:
	Pk3V3ScoreWarMatchRace(PkType pkType);
	virtual ~Pk3V3ScoreWarMatchRace();

public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
	/**
	*@brief 获取基础分
	*/
	UInt32 _GetBaseScore(MatchRaceResult result);

	/**
	*@brief 获取贡献分
	*/
	UInt32 _GetContriScore(MatchRaceResult result, ObjID_t roleid, UInt32 damagePercent);

protected:
	PkType				m_PkType;
};


#endif