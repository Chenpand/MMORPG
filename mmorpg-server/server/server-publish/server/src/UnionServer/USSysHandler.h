#ifndef _US_SYS_HANDLER_H_
#define _US_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLChatProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLRelationProtocol.h>
#include <CLUnionServerProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerToUnion& protocol);

void HandleProtocol(CLProtocol::GateNotifyLeaveConnect& protocol);

void HandleProtocol(CLProtocol::UnionSceneIsReady& protocol);

void HandleProtocol(CLProtocol::UnionWorldChgZoneID& protocol);

void HandleProtocol(CLProtocol::UnionNotifyActivityInfo& protocol);

#endif
