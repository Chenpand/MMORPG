#include "WorldNewTitleHandler.h"
#include "NewTitle.h"
#include "WSPlayerMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeUpReq &req)
{
	PlayerTitleMgr::OnPlayerNewTitleTakeUpReq(player, req.titleGuid, req.titleId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeOffReq &req)
{
	PlayerTitleMgr::OnPlayerNewTitleTakeOffReq(player, req.titleGuid, req.titleId);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldNewTitleTakeUpGuildPostReq &req)
{
	if (NULL == player) return;
	player->GetTitleMgr().OnPlayerNewTitleTakeUpGuildPostReq(player);
}

void HandleProtocol(CLProtocol::SceneNewTitleModifyWorld& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
	{
		return;
	}

	player->SetCurNewTitleGuid(protocol.titleGuid);

	if (protocol.opType == NTOPP_OFF)
	{
		player->SetCurNewTitleGuid(0);
		CLProtocol::WorldNewTitleTakeOffRes pro;
		pro.res = ErrorCode::SUCCESS;
		pro.titleGuid = protocol.titleGuid;
		player->SendProtocol(pro);
	}
	else if (protocol.opType == NTOPP_UP)
	{
		player->SetCurNewTitleGuid(protocol.titleGuid);
		CLProtocol::WorldNewTitleTakeUpRes pro;
		pro.res = ErrorCode::SUCCESS;
		pro.titleGuid = protocol.titleGuid;
		player->SendProtocol(pro);
	}
}

void HandleProtocol(CLProtocol::SceneNewTitleAddWorld& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (player != NULL)
	{
		PlayerTitleMgr::OnPlayerAddNewTitle(player, protocol.titleId, protocol.dueTime);
	}
	else
	{
		// 玩家不在线走离线事件
		OfflineEvent* event = new TitleEvent(protocol.titleId, protocol.dueTime);
		GlobalOfflineEventMgr::Instance()->AddOfflineEvent(protocol.roleId, event);
		event->InsertToDB(protocol.roleId);
	}
}