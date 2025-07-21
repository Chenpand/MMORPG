#ifndef _SCENE_TEAM_COPY_HANDLER_H_
#define _SCENE_TEAM_COPY_HANDLER_H_

#include <CLTeamCopyProtocol.h>

void HandleProtocol(CLProtocol::TeamCopyQueryPlayerInfoReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerCheckReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyPlayerInfoSync& protocol);

void HandleProtocol(CLProtocol::TeamCopyCfgValRes& protocol);

void HandleProtocol(CLProtocol::TeamCopyGetItem& protocol);

#endif