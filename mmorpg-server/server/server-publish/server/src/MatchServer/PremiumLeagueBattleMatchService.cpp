#include "PremiumLeagueBattleMatchService.h"
#include "MSRouter.h"

PremiumLeagueBattleMatchService::PremiumLeagueBattleMatchService()
:PracticeMatchService()
{
	SetType(MST_PREMIUM_LEAGUE_BATTLE);
}

void PremiumLeagueBattleMatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	PracticeMatchService::OnRelayServerStartResponse(result, gamesessionID, abality, address);

	if (result != 0)
	{
		return;
	}

	auto race = FindRace(gamesessionID);
	if (!race || race->GetAllTeam().empty())
	{
		return;
	}

	auto team = race->GetAllTeam()[0];
	CLProtocol::MatchServerPremiumLeagueBattleRes res;
	res.battleId = team->GetPremiumLeagueBattleID();
	res.sessionId = gamesessionID;
	res.addr = address;
	MSRouter::Instance()->SendToWorld(team->GetWsID(), res);
}

