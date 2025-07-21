#include "GSSceneHandler.h"
#include <AvalonLogger.h>
#include <CLSysProtocol.h>

#include "GSRouter.h"
#include "GSSceneMgr.h"
#include "GSPlayerMgr.h"

void HandleProtocol(CLProtocol::GateRegisterScene& protocol)
{
	GSScene* scene = new GSScene();
	if (scene == NULL)
	{
		return;
	}

	if(!scene->Init(protocol.sceneid, protocol.name, protocol.row, protocol.col, 
		protocol.syncRange, protocol.GetConnID(), protocol.sceneDataId, protocol.sceneType))
	{
		CL_SAFE_DELETE(scene);
		return;
	}

	if(!GSSceneMgr::Instance()->AddScene(scene))
	{
		ErrorLogStream << "scene(" << scene->GetID() << "," 
			<< scene->GetName() << ") repeat registering!" << LogStream::eos;
		return;
	}

	InfoLogStream << "scene(" << scene->GetID() << "," << scene->GetName() << ") registered!" << LogStream::eos;
}

void HandleProtocol(CLProtocol::GateUnregisterScene& protocol)
{
	GSScene* scene = GSSceneMgr::Instance()->FindScene(protocol.sceneid);
	if(scene == NULL)
	{
		return;
	}

	InfoLogStream << "scene(" << scene->GetID() << "," << scene->GetName() << ") unregistered!" << LogStream::eos;

	GSSceneMgr::Instance()->RemoveScene(scene);
	CL_SAFE_DELETE(scene);
}

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.playerId);
	if (!player)
	{
		ErrorLogStream << "Can not find player(" << protocol.playerId << ")!" << LogStream::eos;
		return;
	}

	player->SendToGlobalActivity(protocol);
}

