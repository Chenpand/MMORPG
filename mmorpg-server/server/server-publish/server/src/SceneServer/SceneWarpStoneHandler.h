#ifndef _SCENE_WARP_STONE_HANDLER_H_
#define _SCENE_WARP_STONE_HANDLER_H_

#include <CLWarpStoneProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneWarpStoneUnlockReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneWarpStoneChargeReq& req);


#endif //_SCENE_WARP_STONE_HANDLER_H_