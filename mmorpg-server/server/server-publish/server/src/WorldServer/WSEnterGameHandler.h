#ifndef _WS_ENTERGAME_HANDLER_H_
#define _WS_ENTERGAME_HANDLER_H_

#include <CLEnterGameProtocol.h>

void HandleProtocol(CLProtocol::WorldVerifyUniqueReq& protocol);

void HandleProtocol(CLProtocol::SceneEnterGameReq& protocol);

void HandleProtocol(CLProtocol::WorldPlayerLeaveLoginQueue& protocol);

void HandleProtocol(CLProtocol::WorldRegisterPlayerReq& protocol);

void HandleProtocol(CLProtocol::SceneGameReconnectRet& protocol);

void HandleProtocol(ObjID_t id, CLProtocol::SceneQueryAccountReq& req);

void HandleProtocol(CLProtocol::WorldGetLoginPushInfoReq& protocol);

#endif
