#ifndef __GUILD_BATTLE_MATCH_RACE_H__
#define __GUILD_BATTLE_MATCH_RACE_H__

#include "PvPMatchRace.h"

class GuildBattleMatchRace : public PvPMatchRace
{
public:
	virtual bool OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd);

protected:
	/**
	*@brief �������ս������µĻ��ֱ仯
	*/
	virtual void CountMatchScoreChanges();

	/**
	*@brief ���ݲ�ͬ��ʤ���ʤ���Ļ��ֱ仯
	*/
	int CountWinScoreAddition(UInt32 score, UInt32 opponeScore);
};

#endif
