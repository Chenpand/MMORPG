#include "SceneHonorHandler.h"

#include "PlayerMgr.h"
#include "HonorMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneHonorReq& req)
{
	if (NULL == player) return;
	
	player->GetHistoryHonor().HandlerHonor();
}
