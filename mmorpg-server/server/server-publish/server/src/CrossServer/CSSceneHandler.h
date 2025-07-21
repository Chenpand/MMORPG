#ifndef _CS_SCENE_HANDLER_H_
#define _CS_SCENE_HANDLER_H_

#include <CLSceneProtocol.h>
#include <CLCrossProtocol.h>

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& protocol);

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& protocol);

void HandleProtocol(CLProtocol::CrossExitSceneRes& protocol);

void HandleProtocol(CLProtocol::SceneNotifyWorldEnterScene& protocol);

#endif
