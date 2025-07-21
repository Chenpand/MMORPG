#ifndef __SCENE_QUICK_BUY_HANDLER_H__
#define __SCENE_QUICK_BUY_HANDLER_H__

#include <CLQuickBuyProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneQuickBuyReq& req);

void HandleProtocol(CLProtocol::SceneQuickBuyTransactionRes& res);

#endif