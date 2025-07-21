#ifndef __PREMIUM_LEAGUE_BATTLE_H__
#define __PREMIUM_LEAGUE_BATTLE_H__

#include <CLPremiumLeagueDefine.h>

struct PremiumLeagueBattle : public CLPremiumLeagueBattle
{
	PremiumLeagueBattle();
	~PremiumLeagueBattle();

	void InsertToDB();
	void UpdateToDB();
	void SyncToPlayer();

	ObjID_t							guid;
};

#endif