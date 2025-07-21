#ifndef _WORLD_AUCTION_HANDLER_H_
#define _WORLD_AUCTION_HANDLER_H_

#include <CLAuctionProtocol.h>
#include <CLPacketExecutor.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionListReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionItemNumReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionSelfListReq &req);

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldAuctionRecommendPriceReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionBuy &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionRequest &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionCancel &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionFindByID &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemPricesReq &req);

class AuctionCheckRetFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::SceneAbnormalTransRemoveMailItemReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionGetTreasTransactionReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionRusyBuy &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionAttentReq &req);

void HandleProtocol(CLProtocol::SceneAuctionCheckPackageRet& ret);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemPriceListReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryMagicOnsalesReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAuctionQueryItemTransListReq& req);
#endif

