#include "SceneSecurityLockHandler.h"
#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::WorldSecurityStatusSync& sync)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(sync.playerId);

	if (player != NULL)
		player->SetSecurityStatus(sync.state);
}