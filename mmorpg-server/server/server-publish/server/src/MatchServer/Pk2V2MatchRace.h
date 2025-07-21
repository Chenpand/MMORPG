#ifndef __PK_2V2_TUMBLE_MATCH_RACE_H__
#define __PK_2V2_TUMBLE_MATCH_RACE_H__

#include "PvPMatchRace.h"

class Pk2V2MatchRace : public PvPMatchRace
{
public:
	Pk2V2MatchRace(PkType pkType);
	virtual ~Pk2V2MatchRace();
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);
	/**
	*@brief �������ս������µĻ��ֱ仯
	*/
	virtual void CountMatchScoreChanges();
};

#endif