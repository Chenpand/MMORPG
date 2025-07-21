#ifndef _WORLD_SHOP_HANDLER_H_
#define _WORLD_SHOP_HANDLER_H_

#include <CLShopProtocol.h>

class WSPlayer;


void HandleProtocol(WSPlayer* player, CLProtocol::WorldAccountShopItemQueryReq& protocol);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAccountShopItemBuyReq& protocol);
	
#endif