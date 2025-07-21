#include "SceneReplayHandler.h"
#include "PlayerMgr.h"
#include "SSRouter.h"

void HandleProtocol(Player* player, CLProtocol::SceneReplayListReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::ReplayListReq protocol;
	protocol.type = req.type;
	protocol.roleid = player->GetID();
	Router::SendToReplay(protocol);
}

void HandleProtocol(Player* player, CLProtocol::SceneReplayView& protocol)
{
	if (!player)
	{
		return;
	}

	CLProtocol::ReplayReportView report;
	report.roleid = player->GetID();
	report.raceid = protocol.raceid;
	Router::SendToReplay(report);
}

void HandleProtocol(CLProtocol::ReplayListRes& res)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(res.roleid);
	if (!player)
	{
		return;
	}

	CLProtocol::SceneReplayListRes playerRes;
	playerRes.type = res.type;
	playerRes.replays = res.replays;

	player->SendProtocol(playerRes);
}