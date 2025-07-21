#include "GASGambingHandler.h"

#include <CLGambingProtocol.h>
#include "GambingMgr.h"
#include "GASGameZoneMgr.h"
#include "GASPlayerMgr.h"


void HandleProtocol(GASPlayer* player, CLProtocol::PayingGambleReq &req)
{
	GambingMgr::Instance()->PayingGamble(player, req);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GambingItemQueryReq &req)
{
	GambingMgr::Instance()->QueryGambingItems(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GambingMineQueryReq &req)
{
	GambingMgr::Instance()->QueryMineGambingItems(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::GambingRecordQueryReq &req)
{
	GambingMgr::Instance()->QueryGambingRecordData(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::SceneCheckItemNumRet& protocol)
{
	player->OnCheckItemRet(protocol.serial, protocol.result, protocol.counterTimes);
}