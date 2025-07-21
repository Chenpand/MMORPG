#ifndef _WORLD_SYNC_OBJECT_HANDLER_H_
#define _WORLD_SYNC_OBJECT_HANDLER_H_

#include <CLSyncObjectProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::WorldSyncPlayerBaseInfo& sync);

void HandleProtocol(CLProtocol::WorldNotifyKillPlayer& notify);

void HandleProtocol(CLProtocol::SceneSyncEvent& sync);

void HandleProtocol(CLProtocol::SceneSyncOfflineEvent& sync);

void HandleProtocol(CLProtocol::SceneSyncPlayerAvatar& sync);

void HandleProtocol(CLProtocol::SceneSyncAccountInfo& sync);

void HandleProtocol(CLProtocol::WorldAccountCounterSync& sync);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWatchPlayerReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPlayerPowerReq& req);

void HandleProtocol(CLProtocol::SceneSyncRoleValueScore& req);

void HandleProtocol(CLProtocol::SceneGameParamSync& sync);

#endif
