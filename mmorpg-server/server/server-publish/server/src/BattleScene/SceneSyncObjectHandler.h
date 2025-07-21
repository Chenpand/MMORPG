#ifndef _SCENE_SYNC_OBJECT_HANDLER_H_
#define _SCENE_SYNC_OBJECT_HANDLER_H_

#include <CLSyncObjectProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SceneSyncSocialData& sync);

void HandleProtocol(CLProtocol::WorldAddExpReq& req);

void HandleProtocol(CLProtocol::SceneWatchPlayerReq& req);

void HandleProtocol(CLProtocol::SceneSyncPunishment& req);

void HandleProtocol(Player* player, CLProtocol::SceneObjectDataReq& req); 

void HandleProtocol(Player* player, CLProtocol::SceneSetCustomData& req);

void HandleProtocol(CLProtocol::ScenePlayerPowerReq& req);

void HandleProtocol(CLProtocol::SceneSyncEvent& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneClearRedPoint& req);

void HandleProtocol(CLProtocol::WorldAccountCounterSync& protocol);

void HandleProtocol(CLProtocol::WorldAccountDataSync& protocol);

void HandleProtocol(CLProtocol::SceneGameParamSync& protocol); 

void HandleProtocol(Player* player, CLProtocol::SceneSelectObject& protocol);

#endif
