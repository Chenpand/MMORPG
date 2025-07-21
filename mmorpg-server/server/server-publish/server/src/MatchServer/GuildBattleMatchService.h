#ifndef __GUILD_BATTLE_MATCH_SERVICE_H__
#define __GUILD_BATTLE_MATCH_SERVICE_H__

#include "PvPMatchService.h"
#include "GuildBattleMatchRace.h"

class GuildBattleMatchService : public PvPMatchService
{
	const UInt32 MATCH_MAX_WAIT_TIME = 2 * 60;
public:
	GuildBattleMatchService();

	virtual bool Init();

	virtual bool OnTick(const Avalon::Time& now);

	virtual bool OnJoinMatchPool(MatchTeam* team);

protected:
	virtual MatchRace* CreateRace() { return new GuildBattleMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_GUILD_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_GUILD_BATTLE; }

	// 匹配机器人等待时间
	virtual UInt32 GetWaitMatchUntilRobotSec(SeasonMainLevel level = SML_BRONZE) const { return 50; }
};

class CrossGuildBattleMatchService : public GuildBattleMatchService
{
public :
	CrossGuildBattleMatchService()
	{
		SetType(MST_CROSS_GUILD_BATTLE);
	}
};

#endif