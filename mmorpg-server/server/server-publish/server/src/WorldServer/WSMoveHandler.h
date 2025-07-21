#ifndef _WS_MOVE_HANDLER_H_
#define _WS_MOVE_HANDLER_H_

#include <CLNetwork.h>
#include <CLMoveProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::SceneBeginChangeSceneReq& protocol);

void HandleProtocol(CLProtocol::SceneChangeScene& protocol);

#endif
