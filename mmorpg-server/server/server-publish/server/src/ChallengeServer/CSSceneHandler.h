#ifndef _CS_SCENE_HANDLER_H_
#define _CS_SCENE_HANDLER_H_

#include <CLSceneProtocol.h>

void HandleProtocol(Protocol::WorldRegisterSceneReq& protocol);

void HandleProtocol(Protocol::WorldUnregisterSceneReq& protocol);

#endif
