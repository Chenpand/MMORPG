#ifndef _SCENE_VIP_HANDLER_H_
#define _SCENE_VIP_HANDLER_H_

#include <CLVipProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneVipBuyItemReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneGiveMoneyManageRewardReq& req);

#endif
