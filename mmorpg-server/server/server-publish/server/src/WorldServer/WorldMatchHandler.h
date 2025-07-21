#ifndef _WORLD_MATCH_HANDLER_H_
#define _WORLD_MATCH_HANDLER_H_

#include <CLMatchProtocol.h>
#include <CLSysProtocol.h>
#include <CLGameSessionProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol);

void HandleProtocol(CLProtocol::MatchServerMatchStartReq &req);

void HandleProtocol(CLProtocol::MatchServerMatchStartRes &res);

void HandleProtocol(CLProtocol::MatchServerMatchCancelReq &req);

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoReq& req);

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& res);

void HandleProtocol(CLProtocol::WorldNotifyRaceStart& res);

void HandleProtocol(CLProtocol::MatchServerRaceEnd& res);

void HandleProtocol(CLProtocol::MatchServerDungeonRaceEnd& res);

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceReq& req);

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceRes& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMatchQueryFriendStatusReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMatchCancelRes& res);

void HandleProtocol(CLProtocol::MatchServerMatchDungeonRes& res);

void HandleProtocol(CLProtocol::MatchServerGuildBattleRaceEnd& res);

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueRaceEnd& res);

void HandleProtocol(CLProtocol::RelayServerNotifyRaceStart& res);

void HandleProtocol(CLProtocol::MatchServerPlayerMatchSuccessNotify& notify);

void HandleProtocol(CLProtocol::MatchServerPlayerMatchCancelNotify& notify);

void HandleProtocol(CLProtocol::MatchServerPkRoomReq& protocol);

void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoReq& protocol);


#endif