#ifndef _SCENE_SYS_HANDLER_H_
#define _SCENE_SYS_HANDLER_H_

#include <CLChatProtocol.h>
#include <CLSysProtocol.h>
#include <CLWorldLevelProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol);

void HandleProtocol(CLProtocol::WorldNotifyWorldLevelToS &protocol);

void HandleProtocol(CLProtocol::SysServiceSwitchChangeStatus& protocol);

void HandleProtocol(CLProtocol::SysDungeonConfigUpdate& protocol);

void HandleProtocol(CLProtocol::SysDungeonOpenStatusChange& protocol);

void HandleProtocol(CLProtocol::SysPlayerForeverDeleted& protocol);

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol);

#endif
