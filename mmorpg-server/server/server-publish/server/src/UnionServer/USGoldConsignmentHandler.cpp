#include "USGoldConsignmentHandler.h"

#include <CLErrorCode.h>
#include "USPlayer.h"
#include "UnionServiceMgr.h"
#include "GoldConsignmentService.h"

void HandleProtocol(CLProtocol::SceneGoldConsignmentSubmitOrderReq& req, USPlayer* player)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;

	service->HandlerSumbitOrderReq(player, req.tradeType, req.orderType, req.unitPrice, req.tradeNum, req.param, req.submitId);
}

void HandleProtocol(CLProtocol::UnionGoldConsignmentOrderPageReq& req, RoleIDConnID& info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;
	
	service->HandlerOrderPageReq(info);
}

void HandleProtocol(CLProtocol::UnionGoldConsignmentDealRecordReq& req, USPlayer* player)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;

	service->HandlerDealRecordReq(player);
}

void HandleProtocol(CLProtocol::UnionGoldConsignmentOwnerOrderReq& req, USPlayer* player)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;

	service->HandlerOwnerOrderReq(player);
}

void HandleProtocol(CLProtocol::UnionGoldConsignmentCancelOrderReq& req, USPlayer* player)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;

	service->HandlerCancelOrderReq(player, req.orderType, req.orderId);
}

void HandleProtocol(CLProtocol::UnionGoldConsignmentActivityClose& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<GoldConsignmentService>();
	if (service == nullptr)
		return;

	service->ZoneActivityEnd(req.zoneId, req.endTime);
}