#ifndef _WS_GATE_HANDLER_H_
#define _WS_GATE_HANDLER_H_

#include <CLGateProtocol.h>
#include <CLLeaveGameProtocol.h>

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol);

void HandleProtocol(CLProtocol::GateNotifyDisconnect& protocol);

void HandleProtocol(CLProtocol::GateNotifyReconnectSuccess& protocol);

#endif
