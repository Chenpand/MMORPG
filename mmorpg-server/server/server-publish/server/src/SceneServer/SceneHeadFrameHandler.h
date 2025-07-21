#ifndef _SCENE_HEAD_FRAME_H_
#define _SCENE_HEAD_FRAME_H_

#include <CLHeadFrameProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneHeadFrameReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneHeadFrameUseReq& protocol);


#endif // !_SCENE_HEAD_FRAME_H_
