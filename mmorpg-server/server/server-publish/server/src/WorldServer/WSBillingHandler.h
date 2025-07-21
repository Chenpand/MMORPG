#ifndef _WS_BILLING_HANDLER_H_
#define _WS_BILLING_HANDLER_H_

#include <CLBillingProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingGoodsReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingChargePacketReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingChargeReq& req);

void HandleProtocol(CLProtocol::SceneBillingSendGoodsRes& res);


#endif
