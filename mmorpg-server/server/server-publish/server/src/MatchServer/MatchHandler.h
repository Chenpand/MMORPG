#ifndef _WORLD_MATCH_HANDLER_H_
#define _WORLD_MATCH_HANDLER_H_

#include <CLMatchProtocol.h>
#include <CLSysProtocol.h>
#include <CLGameSessionProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol);

void HandleProtocol(CLProtocol::MatchServerMatchStartReq &req);

void HandleProtocol(CLProtocol::MatchServerMatchDungeonReq& req);

void HandleProtocol(CLProtocol::MatchServerGuildBattleMatchReq &req);

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueMatchReq &req);

void HandleProtocol(CLProtocol::MatchServerPkRoomReq &req);

void HandleProtocol(CLProtocol::MatchServerMatchCancelReq &req);

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& res);

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg);

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& res);

void HandleProtocol(CLProtocol::MatchSvrDungeonRaceEnd& req);

void HandleProtocol(CLProtocol::MatchSvrEndGameReq& res);

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceReq& req);

void HandleProtocol(CLProtocol::WorldMatchPracticeReq& req);

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueBattleReq& req);

void HandleProtocol(CLProtocol::SceneServerGiveupReconn& protocol);

void HandleProtocol(CLProtocol::MatchServerSpecifyPkRobot& req);

void HandleProtocol(CLProtocol::MatchServerChampionBattleReq& req);

void HandleProtocol(CLProtocol::RelayAbalitySync& sync);

#endif