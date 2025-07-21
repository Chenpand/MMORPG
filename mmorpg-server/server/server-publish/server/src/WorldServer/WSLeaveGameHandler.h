#ifndef _WS_LEAVEGAME_HANDLER_H_
#define _WS_LEAVEGAME_HANDLER_H_

#include <CLLeaveGameProtocol.h>

void HandleProtocol(CLProtocol::WorldLeaveGameReq& protocol);

void HandleProtocol(CLProtocol::WorldUnregisterPlayerReq& protocol);

void HandleProtocol(CLProtocol::SceneSyncShareDataToWorld& protocol);
#endif
