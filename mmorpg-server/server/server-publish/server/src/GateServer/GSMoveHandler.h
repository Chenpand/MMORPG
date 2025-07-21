#ifndef _GS_MOVE_HANDLER_H_
#define _GS_MOVE_HANDLER_H_

#include <CLMoveProtocol.h>

void HandleProtocol(CLProtocol::GateNotifyEnterScene& protocol);

void HandleProtocol(CLProtocol::GateNotifyLeaveScene& protocol);

void HandleProtocol(CLProtocol::GateSyncGridIndex& protocol);

#endif
