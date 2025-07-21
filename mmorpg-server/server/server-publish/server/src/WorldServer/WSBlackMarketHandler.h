#ifndef _WORLD_BLACK_MARKET_HANDLER_H_
#define _WORLD_BLACK_MARKET_HANDLER_H_

#include <CLBlackMarketProtocol.h>
class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionListReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionReq &req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBlackMarketAuctionCancelReq &req);
#endif