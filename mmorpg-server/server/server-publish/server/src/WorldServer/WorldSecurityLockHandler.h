#ifndef _WORLD_SECURITY_LOCK_HANDLER_H_
#define _WORLD_SECURITY_LOCK_HANDLER_H_

#include "CLSecurityLockProtocol.h"

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSecurityLockDataReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSecurityLockOpReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeSecurityPasswdReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBindDeviceReq& protocol);

void HandleProtocol(CLProtocol::GateSecurityLockCheckReq& protocol);

void HandleProtocol(CLProtocol::GateToWorldSecurityLockRemoveReq& protocol);

#endif