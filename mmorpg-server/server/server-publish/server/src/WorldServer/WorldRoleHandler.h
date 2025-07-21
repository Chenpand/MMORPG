#ifndef _WORLD_ROLE_HANDLER_H_
#define _WORLD_ROLE_HANDLER_H_

#include <CLRoleProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldExtensibleRoleFieldUnlockReq& protocol);

#endif