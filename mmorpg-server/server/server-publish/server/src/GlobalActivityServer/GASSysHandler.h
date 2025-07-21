#ifndef _GAS_SYS_HANDLER_H_
#define _GAS_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLGlobalActivityProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol);

void HandleProtocol(CLProtocol::GASGMCommand& protocol);

#endif