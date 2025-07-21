
#include "SceneWarpStoneHandler.h"
#include "Player.h"


void HandleProtocol(Player* player, CLProtocol::SceneWarpStoneUnlockReq& req)
{
	CLProtocol::SceneWarpStoneUnlockRes res;
	res.result = player->WarpStoneUnlock(req.id);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneWarpStoneChargeReq& req)
{
	CLProtocol::SceneWarpStoneChargeRes res;
	res.result = player->WarpStoneCharge(req.id, req.info);
	player->SendProtocol(res);
}
