#ifndef __SCENE_MATCH_HANDLER_H__
#define __SCENE_MATCH_HANDLER_H__

#include <CLMatchProtocol.h>
#include <CLGameSessionProtocol.h>
#include "Player.h"

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneMatchStartReq &req);

void HandleProtocol(Player* player, CLProtocol::SceneMatchCancelReq &req);

void HandleProtocol(Player* player, CLProtocol::SceneWudaoJoinReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneWudaoRewardReq& req);

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoReq& req);

void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoReq& req);



void HandleProtocol(CLProtocol::MatchServerRaceEnd& req);

void HandleProtocol(CLProtocol::MatchServerDungeonRaceEnd& req);

void HandleProtocol(CLProtocol::WorldChangeMatchScore& req);

void HandleProtocol(CLProtocol::SceneNotifySeason& req);

void HandleProtocol(Player* player, CLProtocol::SceneSeasonPlayStatus &protocol);

#endif