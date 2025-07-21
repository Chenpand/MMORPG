#include "LogicFactory.h"
#include "PVPLogic.h"
#include "DungeonLogic.h"
#include "TestLogic.h"
#include "VerifyMgr.h"

ILogic* LogicFactory::CreateLogic(RaceType type)
{
	VerifyMgr::Instance()->GetStatistic().create++;
	switch (type)
	{
	case RT_PK:
	case RT_GUILD_BATTLE:
	case RT_PREMIUM_LEAGUE_PRELIMINAY:
	case RT_PREMIUM_LEAGUE_BATTLE:
	case RT_PK_3V3:
	case RT_SCORE_WAR:
	case RT_3V3_TUMBLE:
	case RT_CHAMPION:
		return new PVPLogic();
	case RT_DUNGEON:
	case RT_DUNGEON_CROSS:
		return new DungeonLogic();
	default:
		return NULL;
	}
}