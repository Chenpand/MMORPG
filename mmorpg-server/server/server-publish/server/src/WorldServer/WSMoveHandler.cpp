#include "WSMoveHandler.h"

#include <AvalonLogger.h>
#include <CLErrorCode.h>
#include <CLRecordClient.h>

#include "WSPlayerMgr.h"
#include "WSSceneMgr.h"
#include "WSTeam.h"

void HandleProtocol(CLProtocol::SceneBeginChangeSceneReq& protocol)
{
	WSScene* scene = WSSceneMgr::Instance()->FindScene(protocol.info.destscene);
	if(scene != NULL)
	{
		scene->SendToScene(protocol);
	}
}

void HandleProtocol(CLProtocol::SceneChangeScene& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.info.id);
	if(player == NULL)
	{
		GameErrorLogStream("Player") << "can not find player(," << protocol.info.id << ",) when change scene!" << LogStream::eos;
		
		CLFlushPlayerTables();
		return;
	}

	UInt32 oldNodeId = player->GetSceneNode();
	if(!player->SetSceneNode(protocol.info.destnode))
	{
		GameErrorLogStream("Player") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") set scenenode failed when change scene!" << LogStream::eos;

		player->KickOff(ErrorCode::SCENE_DUPLICATE);
		player->LeaveGame();

		CLFlushPlayerTables();
		return;
	}

	player->SendToScene(protocol);

	player->OnServerChanged(oldNodeId);

	GameInfoLogStream("Player") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") changed to sceneserver " << protocol.info.destnode
		<< LogStream::eos;
}

