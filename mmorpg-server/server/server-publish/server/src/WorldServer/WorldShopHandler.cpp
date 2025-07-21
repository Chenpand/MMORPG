#include "WorldShopHandler.h"

#include "WSPlayer.h"
#include "WSShopMgr.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldAccountShopItemQueryReq& protocol)
{
	if (!player) return;
	
	WSShopMgr::Instance()->OnPlayerQueryShopItem(player, protocol);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAccountShopItemBuyReq& protocol)
{
	if (!player) return;

	WSShopMgr::Instance()->OnPlayerBuyShopItem(player, protocol);
}