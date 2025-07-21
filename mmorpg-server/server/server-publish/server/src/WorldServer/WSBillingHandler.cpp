#include "WSBillingHandler.h"
#include <CLErrorCode.h>

#include "ChargeManager.h"
#include "WSPlayer.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingGoodsReq& req)
{
	ChargeManager::Instance()->OnPlayerQueryChargeGoods(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingChargePacketReq& req)
{
	ChargeManager::Instance()->OnPlayerQueryChargePacketGoods(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBillingChargeReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldBillingChargeRes res;
	if (req.mallType == CHARGE_MALL_CHARGE || req.mallType == CHARGE_MALL_MONEY_MANAGE || req.mallType == CHARGE_DAY_CHARGE_WELFARE ||
		req.mallType == CHARGE_ADVENTURE_PASS_KING || req.mallType == CHARGE_GIGT_RIGHT_CARD || req.mallType == CHARGE_DUNGEON_RIGHT_CARD)
	{
		res.result = ChargeManager::Instance()->OnPlayerCanBuyChargeGoods(player, req.goodsId);
	}
	else if (req.mallType == CHARGE_MALL_PACKET)
	{
		res.result = ChargeManager::Instance()->OnPlayerCanBuyChargePacket(player, req.goodsId);
	}
	else
	{
		res.result = ErrorCode::BILLING_GOODS_UNEXIST;
	}

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneBillingSendGoodsRes& res)
{
	ChargeManager::Instance()->OnSceneSendGoodsRes(res.orderId, res.result);
}
