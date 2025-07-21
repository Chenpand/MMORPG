#include "WSSceneHandler.h"
#include <AvalonLogger.h>
#include <CLErrorCode.h>

#include <CLMoveProtocol.h>
#include <CLSysNotify.h>
#include <CLSceneDataEntry.h>

#include "WSSceneMgr.h"
#include "WSPlayerMgr.h"
#include "WSTeam.h"
#include "WSNetwork.h"
#include "WorldSysNotify.h"
#include "WSPushNotify.h"
#include "RoomMgr.h"

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& req)
{
	CLProtocol::WorldRegisterSceneRet ret;
	ret.sceneid = req.sceneid;

	WSScene* scene = new WSScene();
	if(!scene->Init(req.sceneid, req.mapid, req.name, req.GetConnID()))
	{
		CL_SAFE_DELETE(scene);
		return;
	}

	scene->SetType(req.type);
	scene->SetLevel(req.level);
	scene->SetRevivePos(req.pos);
	
	WSPlayer* creater = WSPlayerMgr::Instance()->FindNormalPlayer(req.creater);
	switch(scene->GetType())
	{
	case SCENE_TYPE_PRIVATESCENE:
		{
			scene->SetOwner(req.creater);
		}
		break;
	case SCENE_TYPE_TEAMCOPY:
		{
			if(creater == NULL || creater->GetTeam() == NULL || creater->GetTeam()->GetId() != req.owner)
			{
				ret.result = ErrorCode::SCENE_NOOWNER;
				ErrorLogStream << "scene(" << req.mapid << "," << req.name << ") register failed! errorcode=" << ret.result << LogStream::eos;
				req.SendBack(ret);
				CL_SAFE_DELETE(scene);
				return;
			}
			scene->SetOwner(req.owner);
		}
		break;
	}

	if(!WSSceneMgr::Instance()->AddScene(scene))
	{
		ret.result = ErrorCode::SCENE_DUPLICATE;
		ErrorLogStream << "scene(" << req.mapid << "," << req.name << ") register failed!" << LogStream::eos;
		req.SendBack(ret);
		CL_SAFE_DELETE(scene);
		return;
	}

	ret.result = ErrorCode::SUCCESS;
	req.SendBack(ret);

	scene->OnCreated(creater);
}

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& req)
{
	WSScene* scene = WSSceneMgr::Instance()->FindScene(req.sceneid);
	if(scene == NULL) return;

	CLProtocol::WorldUnregisterSceneRet ret;
	ret.sceneid = req.sceneid;
	scene->SendToScene(ret);

	WSSceneMgr::Instance()->RemoveScene(scene);
	CL_SAFE_DELETE(scene);
}

void HandleProtocol(CLProtocol::WorldEnterDynSceneReq& req)
{
	
}

void HandleProtocol(CLProtocol::WorldRegisterNpcPos& req)
{
	for(std::map<UInt32, CLPosition>::iterator iter = req.npcs.begin();
		iter != req.npcs.end(); ++iter)
	{
		WSSceneMgr::Instance()->RegisterNpc(iter->first, req.mapId, iter->second);
	}
}

void HandleProtocol(CLProtocol::SceneSyncIdSeed& req)
{
	WSSceneMgr::Instance()->SetIdSeed(req.GetConnID(), req.idSeed);
}

void HandleProtocol(CLProtocol::SceneSyncPlayerNum& req)
{
	WSScene* scene = WSSceneMgr::Instance()->FindScene(req.sceneId);
	if(scene != NULL)
	{
		scene->SetPlayerNum(req.playerNum);
	}
}

void HandleProtocol(CLProtocol::SceneNotifyWorldEnterScene& protocol)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (player)
	{
		if (player->GetRoomId() != 0)
		{
			RoomMgr::Instance()->OnChangeScene(player, protocol.sceneId);
		}
		player->SetSceneID(protocol.sceneId);
	}
}

void HandleProtocol(CLProtocol::SceneNotifyWorldLoadStart& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") start load on scene server(" << protocol.GetConnID() << ")." << LogStream::eos;

	WSPlayerMgr::Instance()->AddLoadingPlayer(protocol.roleId);
}

void HandleProtocol(CLProtocol::SceneNotifyWorldLoadFinish& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") load finish on scene server(" << protocol.GetConnID() << ")." << LogStream::eos;

	WSPlayerMgr::Instance()->OnPlayerLoadFinishOnSceneServer(protocol.roleId, protocol.GetConnID());
}

void HandleProtocol(CLProtocol::SceneNotifyWorldLeave& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") leave scene server(" << protocol.GetConnID() << ")." << LogStream::eos;

	WSPlayerMgr::Instance()->OnPlayerLeaveSceneServer(protocol.roleId);
}

void HandleProtocol(CLProtocol::ScenePushNotifyToWorld& protocol)
{
	if (protocol.accid > 0)
	{
		pushNotifyToCenter(protocol.accid, protocol.title, protocol.content);
	}
	pushNotifyToCenterByRoleID(protocol.roleid, protocol.title, protocol.content);
}