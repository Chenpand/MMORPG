#ifndef _SS_LEAVEGAME_HANDLER_H_
#define _SS_LEAVEGAME_HANDLER_H_

#include <CLLeaveGameProtocol.h>

void HandleProtocol(CLProtocol::SceneLeaveGameReq& protocol);
void HandleProtocol(CLProtocol::SceneLogicLeaveGameReq& protocol);
#endif
