#ifndef _GS_SECURITY_LOCK_HANDLER_H_
#define _GS_SECURITY_LOCK_HANDLER_H_

#include <CLSecurityLockProtocol.h>

class GSPlayer;

void HandleProtocol(GSPlayer* player, CLProtocol::GateSecurityLockRemoveReq& protocol);

void HandleProtocol(CLProtocol::GateSecurityLockCheckRes& protocol);

void HandleProtocol(CLProtocol::GateToWorldSecurityLockRemoveRes& protocol);



#endif 
