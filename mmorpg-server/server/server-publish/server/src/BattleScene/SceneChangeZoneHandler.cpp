#include "SceneChangeZoneHandler.h"

#include <CLErrorCode.h>
#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::GateChangeZoneCheckRet& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	if(protocol.result != ErrorCode::SUCCESS)
	{
		player->SendNotify(227);
		return;
	}

	player->LeaveGameForTour(protocol.zoneId, protocol.mapId, protocol.gateAddr, protocol.key);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneChangeZoneReturn &protocol)
{
	pPlayer->ReturnZone();
}
