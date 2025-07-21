#include "SceneHeadFrameHandler.h"
#include "Player.h"

void HandleProtocol(Player* player, CLProtocol::SceneHeadFrameReq& protocol)
{
	if (NULL == player) return;

	player->GetHeadFrameMgr().HandleHeadFrameReq();
}

void HandleProtocol(Player* player, CLProtocol::SceneHeadFrameUseReq& protocol)
{
	if (NULL == player) return;

	player->GetHeadFrameMgr().HandleUseHeadFrameReq(protocol.headFrameId);
}