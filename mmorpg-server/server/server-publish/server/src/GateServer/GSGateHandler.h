#ifndef _GS_GATE_HANDLER_H_
#define _GS_GATE_HANDLER_H_

#include <CLGateProtocol.h>
#include <CLSysProtocol.h>

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol);

void HandleProtocol(CLProtocol::GateBroadcastAround& protocol);

void HandleProtocol(CLProtocol::GateBroadcastExcept& protocol);

void HandleProtocol(CLProtocol::GateBroadcastAll& protocol);

void HandleProtocol(CLProtocol::GateSyncPlayerBaseInfo& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysServiceSwitchChangeStatus& protocol);


#endif
