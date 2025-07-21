#include "SSLeaveGameHandler.h"
#include "PlayerMgr.h"


void HandleProtocol(CLProtocol::SceneLeaveGameReq& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if(player == NULL) return;

	if (protocol.forceLeave)
	{
		player->OnLogicOffLine();
		player->OnLeaveTimeout();
	}
	else
	{
		player->LeaveGame();
	}
}

void HandleProtocol(CLProtocol::SceneLogicLeaveGameReq& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if (player == NULL) return;

	player->OnLogicOffLine();
}

