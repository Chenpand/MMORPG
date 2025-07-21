#include "SceneAdventureTeamHandler.h"

#include "PlayerMgr.h"


void HandleProtocol(CLProtocol::WorldAdventureTeamInfoSync& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player) return;
	player->SetAdventureTeamName(protocol.info.adventureTeamName);
	player->SetAdventureTeamLv(protocol.info.adventureTeamLevel);
	player->CalcRoleValueScore(false, protocol.isCreate);
}