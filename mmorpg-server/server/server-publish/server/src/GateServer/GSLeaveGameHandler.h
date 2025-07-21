#ifndef _GS_LEAVEGAME_HANDLER_H_
#define _GS_LEAVEGAME_HANDLER_H_

#include <CLLeaveGameProtocol.h>
#include <CLCenterProtocol.h>

class GSPlayer;

void HandleProtocol(GSPlayer* player,CLProtocol::GateLeaveGameReq& protocol);

void HandleProtocol(CLProtocol::GateKickOffPlayer& protocol);

void HandleProtocol(CLProtocol::CenterPlayerAddictTimeSync& protocol);

#endif
