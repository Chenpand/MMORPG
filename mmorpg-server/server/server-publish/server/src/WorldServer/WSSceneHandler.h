#ifndef _WS_SCENE_HANDLER_H_
#define _WS_SCENE_HANDLER_H_

#include <CLSceneProtocol.h>

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& req);

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& req);

void HandleProtocol(CLProtocol::WorldEnterDynSceneReq& req);

void HandleProtocol(CLProtocol::WorldRegisterNpcPos& req);

void HandleProtocol(CLProtocol::SceneSyncIdSeed& req);

void HandleProtocol(CLProtocol::SceneSyncPlayerNum& req);

void HandleProtocol(CLProtocol::SceneSyncPlayerNum& req);

void HandleProtocol(CLProtocol::SceneNotifyWorldEnterScene& protocol);

void HandleProtocol(CLProtocol::SceneNotifyWorldLoadStart& protocol);

void HandleProtocol(CLProtocol::SceneNotifyWorldLoadFinish& protocol);

void HandleProtocol(CLProtocol::SceneNotifyWorldLeave& protocol);

void HandleProtocol(CLProtocol::ScenePushNotifyToWorld& protocol);

#endif
