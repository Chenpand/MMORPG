#include "TCSPlayerSyncHandler.h"
#include "TCSPlayerMgr.h"
#include "TCSTeamMgr.h"

void HandleProtocol(CLProtocol::TeamCopyPlayerInfoSync& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (NULL == player)
		return;

	player->SetPlayerData(protocol);
	if (player->GetSceneSubType() != 0 && !TCSPlayerMgr::Instance()->PlayerIsExpire(protocol.roleId))
	{
		player->PlayerInfoToClient(player->GetTeamType());
	}
}

void HandleProtocol(CLProtocol::TeamCopyPlayerTicketSync& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (NULL == player)
		return;

	for (auto& iter : protocol.ticketMap)
	{
		player->SetTicketIsEnough(iter.first, iter.second);
	}

	player->PlayerInfoToClient(player->GetTeamType());

	TCSTeamMgr::Instance()->TicketUpdate(protocol.roleId, player->GetTicketIsEnough(player->GetTeamType()));
}

void HandleProtocol(CLProtocol::TeamCopyPlayerEquipScoreSync& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (NULL == player)
		return;

	player->SetEquipScore(protocol.equipScore);
}

void HandleProtocol(CLProtocol::TeamCopyPlayerLoadingSync& protocol)
{
	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.roleId);
	if (NULL == player)
		return;

	player->SetIsLoading(protocol.isLoading ? true:false);
}