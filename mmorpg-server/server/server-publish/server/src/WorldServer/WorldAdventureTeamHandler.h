#ifndef _WORLD_ADVENTURE_TEAM_HANDLER_H_
#define _WORLD_ADVENTURE_TEAM_HANDLER_H_

#include <CLAdventureTeamProtocol.h>

class WSPlayer;


void HandleProtocol(WSPlayer* player, CLProtocol::WorldAdventureTeamExtraInfoReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAdventureTeamRenameReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlessCrystalInfoReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInheritBlessInfoReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInheritExpReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryExpeditionMapReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryOptionalExpeditionRolesReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDispatchExpeditionTeamReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldCancelExpeditionReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetExpeditionRewardsReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryAllExpeditionMapsReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryOwnOccupationsReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRemoveUnlockedNewOccupationsReq& req);

void HandleProtocol(CLProtocol::GateNotifyReCalculateTotalRoleValueScore& protocol);

#endif