#ifndef __GUILD_BATTLE_MATCH_RACE_H__
#define __GUILD_BATTLE_MATCH_RACE_H__

#include "PvPMatchRace.h"

class GuildBattleMatchRace : public PvPMatchRace
{
public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
	/**
	*@brief 计算各种战斗结果下的积分变化
	*/
	virtual void CountMatchScoreChanges();

	/**
	*@brief 根据不同连胜情况胜利的积分变化
	*/
	int CountWinScoreAddition(UInt32 score, UInt32 opponeScore);
};

#endif
