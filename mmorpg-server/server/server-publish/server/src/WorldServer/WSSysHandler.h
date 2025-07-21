#ifndef _WS_SYS_HANDLER_H_
#define _WS_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLChatProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol);

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol);

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol);

void HandleProtocol(CLProtocol::SysTransferProtocol& protocol);

#endif
