#ifndef _CHAT_MONITOR_HANDLER_H_
#define _CHAT_MONITOR_HANDLER_H_

#include <CLCenterProtocol.h>

void HandleProtocol(CLProtocol::CenterSyncChatInfo& protocol);

void HandleProtocol(CLProtocol::CenterChatMonitorSetServers& protocol);

#endif
