#ifndef _US_GOLD_CONSIGNMENT_HANDLER_H_
#define _US_GOLD_CONSIGNMENT_HANDLER_H_

#include <CLGoldConsignmentProtocol.h>

struct RoleIDConnID;
class USPlayer;

void HandleProtocol(CLProtocol::SceneGoldConsignmentSubmitOrderReq& req, USPlayer* player);

void HandleProtocol(CLProtocol::UnionGoldConsignmentOrderPageReq& req, RoleIDConnID& info);

void HandleProtocol(CLProtocol::UnionGoldConsignmentDealRecordReq& req, USPlayer* player);

void HandleProtocol(CLProtocol::UnionGoldConsignmentOwnerOrderReq& req, USPlayer* player);

void HandleProtocol(CLProtocol::UnionGoldConsignmentCancelOrderReq& req, USPlayer* player);

void HandleProtocol(CLProtocol::UnionGoldConsignmentActivityClose& req);

#endif // !_US_GOLD_CONSIGNMENT_HANDLER_H_

