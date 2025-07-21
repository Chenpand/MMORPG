#ifndef _TCS_SYS_HANDLER_H_
#define _TCS_SYS_HANDLER_H_

#include <CLSysProtocol.h>
#include <CLChatProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLRelationProtocol.h>

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol);

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol);

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol);

void HandleProtocol(CLProtocol::SysTransmitPlayerToTeamCopy& protocol);

void HandleProtocol(CLProtocol::GateNotifyLeaveConnect& protocol);

void HandleProtocol(CLProtocol::SceneSyncChat& protocol);

#endif
