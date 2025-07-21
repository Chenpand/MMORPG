#ifndef _BS_SYS_HANDLER_H_
#define _BS_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLBattleProtocol.h>
#include <CLGateProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol);

void HandleProtocol(CLProtocol::BattleExitSceneRes& protocol);

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol);

void HandleProtocol(CLProtocol::GateTransmitPacket& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol);

#endif
