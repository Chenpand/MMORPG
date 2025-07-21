#include "GSRelationHandler.h"
#include "GSPlayerMgr.h"

void HandleProtocol(CLProtocol::GateSyncBlackList& req)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(req.id);
	if(player == NULL) return;

	player->SetBlackList(req.blacklist);
}

void HandleProtocol(CLProtocol::GateNotifyNewBlackList& req)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(req.id);
	if(player == NULL) return;

	player->AddBlackList(req.target);
}

void HandleProtocol(CLProtocol::GateNotifyDelBlackList& req)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(req.id);
	if(player == NULL) return;

	player->RemoveBlackList(req.target);
}
