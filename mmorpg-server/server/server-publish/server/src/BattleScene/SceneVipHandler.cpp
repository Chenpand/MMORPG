#include "SceneVipHandler.h"

#include <CLVipDefine.h>
#include "Player.h"
#include "SSServiceSwitchMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneVipBuyItemReq& req)
{
	CLProtocol::SceneVipBuyItemRes res;
	res.result = player->BuyGiftItem(req.vipLevel);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneGiveMoneyManageRewardReq& req)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MONEY_MANAGE))
	{
		return;
	}

	CLProtocol::SceneGiveMoneyManageRewardRes res;
	res.result = player->GiveMoneyManageReward((UInt32)req.rewardId);
	player->SendProtocol(res);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(player) << "SceneGiveMoneyManageRewardReq CheckMask reward(" << req.rewardId << ") resut(" << res.result << ")." << LogStream::eos;
	}
}

