#ifndef __SCENE_REPLAY_HANDLER_H__
#define __SCENE_REPLAY_HANDLER_H__

#include <CLReplayProtocol.h>

class Player;
void HandleProtocol(Player* player, CLProtocol::SceneReplayListReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneReplayView& protocol);

void HandleProtocol(CLProtocol::ReplayListRes& res);

#endif