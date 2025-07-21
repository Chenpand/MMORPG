#include "WSLeaveGameHandler.h"
#include <AvalonLogger.h>
#include "WSPlayerMgr.h"
#include "WSPlayerShareDataMgr.h"

void HandleProtocol(CLProtocol::WorldLeaveGameReq& protocol)
{
	WSPlayerMgr::Instance()->DelFromWaitList(protocol.accid);

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.id);
	if (player == NULL || player->GetGameStatus() == PS_FINAL)
	{
		InfoLogStream << "player(" << protocol.id << ") is already offline." << LogStream::eos;
		return;
	}

	if (player->GetGateID() != 0 && player->GetGateID() != protocol.GetConnID())
	{
		ErrorLogStream << "player(" << protocol.id << ") recv invalid logout request from gate(" << protocol.GetConnID() << ")." << LogStream::eos;
		return;
	}

	player->NotifyLeaveGame(protocol.isOfflineTimeOut);
}

void HandleProtocol(CLProtocol::WorldUnregisterPlayerReq& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.id);
	if(player == NULL || player->GetGameStatus() == PS_FINAL) return;

	if(protocol.sceneId != 0)
	{
		player->SetOfflineScene(protocol.sceneId);
		player->SetOfflinePos(protocol.pos);
	}

	player->LeaveGame();
}

void HandleProtocol(CLProtocol::SceneSyncShareDataToWorld& protocol)
{
	WSPlayerShareDataMgr::Instance()->HandleScenePlayerSyncShareData(protocol.m_Guid, protocol.sharedata);
}