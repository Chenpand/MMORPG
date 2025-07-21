#ifndef _CENTER_SYS_HANDLER_H_
#define _CENTER_SYS_HANDLER_H_

#include <CLSysProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol);

void HandleProtocol(CLProtocol::SysTransmitFromGlobalToNode& protocol);

#endif