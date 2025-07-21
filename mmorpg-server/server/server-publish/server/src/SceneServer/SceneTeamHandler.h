#ifndef _SCENE_TEAM_HANDLER_H_
#define _SCENE_TEAM_HANDLER_H_

#include <CLTeamProtocol.h>
#include <CLGameSessionProtocol.h>

class Player;

void HandleProtocol(CLProtocol::WorldSyncTeamInfoToScene& req);

void HandleProtocol(CLProtocol::WorldSyncTeamMemberStatusToScene& req);

void HandleProtocol(CLProtocol::WorldNotifyDelTeamToScene& req);

void HandleProtocol(CLProtocol::WorldSyncTeamBaseToScene& req);

void HandleProtocol(CLProtocol::WorldNotifyMemberInToScene& req);

void HandleProtocol(CLProtocol::WorldNotifyMemberOutToScene& req);

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceRes& res);

void HandleProtocol(CLProtocol::WorldServerTeamStartDungeonRes& res);

void HandleProtocol(Player* player, CLProtocol::SceneTeamMatchStartReq& req);

void HandleProtocol(CLProtocol::WorldTeamQueryTargetCondReq& req);

void HandleProtocol(CLProtocol::WorldTeamDungeonRaceStart& protocol);

void HandleProtocol(CLProtocol::WorldTeamDungeonRaceOver& protocol);

#endif
