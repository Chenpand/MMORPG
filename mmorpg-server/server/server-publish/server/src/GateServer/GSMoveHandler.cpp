#include "GSMoveHandler.h"

#include <CLLeaveGameProtocol.h>
#include "GSRouter.h"
#include "GSPlayerMgr.h"
#include "GSSceneMgr.h"

void HandleProtocol(CLProtocol::GateNotifyEnterScene& protocol)
{
	InfoLogStream << "recv player(" << protocol.id << ") enter scene(" << protocol.sceneid << ") from server(" << protocol.GetConnID() << ")." << LogStream::eos;

	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	if(player == NULL)
	{
		CLProtocol::WorldLeaveGameReq req;
		req.accid = 0;
		req.id = protocol.id;
		Router::SendToWorld(req);
		return;
	}

	GSScene* scene = GSSceneMgr::Instance()->FindScene(protocol.sceneid);
	if (scene == NULL)
	{
		ErrorLogStream << "recv player(" << protocol.id << ") enter scene(" << protocol.sceneid << ") from server(" << protocol.GetConnID() << "), invalild scene." << LogStream::eos;
		return;
	}

	if(player->GetScene() != NULL)
	{
		player->GetScene()->RemovePlayer(player);
	}
	scene->AddPlayer(protocol.gridindex,player);
}

void HandleProtocol(CLProtocol::GateNotifyLeaveScene& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	if(player == NULL || player->GetScene() == NULL || player->GetSceneID() != protocol.sceneId) return;

	player->GetScene()->RemovePlayer(player);
}

void HandleProtocol(CLProtocol::GateSyncGridIndex& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	if(player == NULL || player->GetScene() == NULL) return;

	player->GetScene()->UpdateGrid(player,protocol.gridIndex);
}
