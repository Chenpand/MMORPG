#include "SceneQuickBuyHandler.h"
#include <CLErrorCode.h>
#include "QuickBuyTransactionInstances.h"
#include "Player.h"
#include "PlayerMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneQuickBuyReq& req)
{
	if (!player)
	{
		return;
	}

	QuickBuyTransaction* trans = NULL;
	if (req.type == QUICK_BUY_REVIVE)
	{
		trans = new ReviveQuickBuyTransaction(req.param1, (UInt32)req.param2);
	}
	else if (req.type == QUICK_BUY_ITEM)
	{
		trans = new CommonQuickBuyTransaction((UInt32)req.param1, (UInt32)req.param2);
	}
	else
	{
		CLProtocol::SceneQuickBuyRes res;
		res.result = ErrorCode::QUICK_BUY_INVALID_TYPE;
		player->SendProtocol(res);
		return; 
	}

	if (UInt32 result = player->BeginQuickBuyTransaction(trans))
	{
		CLProtocol::SceneQuickBuyRes res;
		res.result = result;
		player->SendProtocol(res);
	}
}

void HandleProtocol(CLProtocol::SceneQuickBuyTransactionRes& res)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(res.roleId);
	if (!player)
	{
		return;
	}

	player->OnQuickBuyTransactionRes(res.transactionId, res.result);
}