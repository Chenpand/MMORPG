#include "SceneSysRecordHandler.h"

#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::SceneSysRecordDataRes& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.ownerId);
	if (!player) return;

	player->OnSysRecordDataRes(protocol);
}