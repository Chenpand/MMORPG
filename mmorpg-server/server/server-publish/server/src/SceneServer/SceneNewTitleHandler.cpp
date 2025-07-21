#include "SceneNewTitleHandler.h"
#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::WorldNewTitleModifyScene &protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
	{
		return;
	}
	player->OnNewTitleModify(protocol.opType, protocol.ownerType, protocol.titleGuid, protocol.titleName, protocol.style, protocol.titleId);
}