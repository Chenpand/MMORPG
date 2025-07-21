#ifndef _SCENE_TRADE_HANDLER_H_
#define _SCENE_TRADE_HANDLER_H_

#include <CLTradeProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneTradeUpdateItemsReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTradeUpdateSilverReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTradeSetStatus& req);

#endif
