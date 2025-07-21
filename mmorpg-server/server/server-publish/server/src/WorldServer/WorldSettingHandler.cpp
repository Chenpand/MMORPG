#include "WorldSettingHandler.h"

#include "WSPlayerMgr.h"
#include "WSSortListMgr.h"
#include "WSRelationMgr.h"
#include "GuildMgr.h"
#include "HireMgr.h"

void HandleProtocol(CLProtocol::SceneNotifyChangeNameToWorld& protocol)
{
	WSPlayerMgr::Instance()->OnNameChanged(protocol.playerId, protocol.newName);
	WSSortListMgr::Instance()->OnNameChanged(protocol.playerId, protocol.newName);
	WSRelationMgr::OnNameChanged(protocol.playerId, protocol.newName);
	GuildMgr::Instance()->OnMemberNameChanged(protocol.playerId, protocol.newName);
	HireMgr::Instance()->OnPlayerNameChange(protocol.playerId, protocol.newName);
}

void HandleProtocol(CLProtocol::SceneNotifyChangeSexToWorld& protocol)
{
	WSPlayerMgr::Instance()->OnSexChanged(protocol.playerId, protocol.sex);
	WSRelationMgr::OnSexChanged(protocol.playerId, protocol.sex);
}

void HandleProtocol(CLProtocol::SceneGameSetSync& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (NULL == player)
		return;

	player->ParseGameSet(protocol.gameSet);
}