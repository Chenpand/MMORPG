#ifndef _SCENE_ROOM_HANDLER_H_
#define _SCENE_ROOM_HANDLER_H_

#include <CLRoomProtocol.h>
#include <CLGameSessionProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SceneRoomStartMatch& protocol);

void HandleProtocol(CLProtocol::CrossRoomMatchInfoReq& req);

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol);

void HandleProtocol(CLProtocol::SceneRoomSyncInfo& protocol);

#endif