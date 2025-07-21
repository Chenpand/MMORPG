#include "WSBlackMarketHandler.h"
#include "BlackMarketMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionListReq &req)
{
	BlackMarketMgr::Instance()->OnPlayerQueryAuctionList(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionReq &req)
{
	BlackMarketMgr::Instance()->OnPlayerAuctionReq(player, req.auction_guid, req.item_guid, req.auction_price);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionCancelReq &req)
{
	BlackMarketMgr::Instance()->OnPlayerCancelAuctionReq(player, req.auction_guid);
}