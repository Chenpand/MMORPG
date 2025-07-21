#ifndef _SS_CONSOLE_HANDLER_H_
#define _SS_CONSOLE_HANDLER_H_

#include <CLConsoleProtocol.h>

void HandleProtocol(CLProtocol::SceneConsoleQueryPlayer& protocol);

void HandleProtocol(CLProtocol::SceneConsoleReloadScripts& protocol);

void HandleProtocol(CLProtocol::SceneConsoleControlPlayer& protocol);

void HandleProtocol(CLProtocol::SceneConsolePhoneBind& protocol);

void HandleProtocol(CLProtocol::AdminConsoleRequest& req);

void HandleProtocol(CLProtocol::WorldSyncPlayerGMAuthority& protocol);

#endif
