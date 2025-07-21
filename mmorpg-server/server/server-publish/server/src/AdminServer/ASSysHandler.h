#ifndef _AS_SYS_HANDLER_H_
#define _AS_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLUnionServerProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitFromGlobalToNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerToTeamCopy& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerToUnion& protocol);

void HandleProtocol(CLProtocol::UnionNotifyOpenFunction& protocol);

void HandleProtocol(CLProtocol::SysTransmitToUnion& protocol);

void HandleProtocol(CLProtocol::SysTransferProtocol& protocol);

#endif
