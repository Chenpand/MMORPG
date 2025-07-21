#include "WorldRoleHandler.h"

#include "WSPlayer.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldExtensibleRoleFieldUnlockReq& protocol)
{
	if (!player) return;

	player->UnlockExtensibleRoleField(protocol.costItemUId, protocol.costItemDataId);
}