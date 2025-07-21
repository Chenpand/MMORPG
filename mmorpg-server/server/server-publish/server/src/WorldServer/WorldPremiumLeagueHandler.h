#ifndef __WORLD_PREMIUM_LEAGUE_HANDLER_H__
#define __WORLD_PREMIUM_LEAGUE_HANDLER_H__

#include <CLPremiumLeagueProtocol.h>
#include <CLMatchProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueRewardPoolReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueEnrollReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueBattleRecordReq &req);

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueBattleRes& res);

#endif