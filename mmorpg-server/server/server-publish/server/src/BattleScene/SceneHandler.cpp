#include "SceneHandler.h"

#include <CLErrorCode.h>
#include <CLSceneDataEntry.h>
#include <AsyncMsgCallbackMgr.h>
#include "Scene.h"
#include "SceneMgr.h"
#include "SSRouter.h"
#include "PlayerMgr.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"

void HandleProtocol(CLProtocol::SceneNotifyLoadmap& protocol)
{
	SceneObject::SetIDSeed(protocol.idSeed + 2000);
	for(std::vector<UInt32>::iterator iter = protocol.maps.begin()
		;iter != protocol.maps.end();++iter)
	{
		if(!SceneMgr::Instance()->LoadScene(*iter))
		{
			ErrorLogStream << "load scene(" << *iter << ") failed!" << LogStream::eos;
			continue;
		}
	}
}

void HandleProtocol(CLProtocol::SceneNotifyLoadDungeonEntryMap& protocol)
{
    SceneObject::SetIDSeed(protocol.idSeed + 3000);
    for (auto iter = protocol.maps.begin(); iter != protocol.maps.end(); ++iter)
    {
        if (!SceneMgr::Instance()->LoadDungeonEntryScene(*iter))
        {
            ErrorLogStream << "load dungeon entry scene(" << *iter << ") failed!" << LogStream::eos;
            continue;
        }
    }
}

void HandleProtocol(CLProtocol::SceneNotifyLoadDynMap& protocol)
{
}

void HandleProtocol(CLProtocol::SceneNotifyLoadBattleScene& protocol)
{
}

void HandleProtocol(CLProtocol::WorldRegisterSceneRet& protocol)
{
	Scene* scene = SceneMgr::Instance()->FindScene(protocol.sceneid);
	if(scene == NULL || scene->GetStatus() != SCENE_INIT) return;

	if(protocol.result != ErrorCode::SUCCESS)
	{
		SceneMgr::Instance()->RemoveScene(scene);
		SceneMgr::Instance()->FreeScene(scene);
		return;
	}

	scene->OnSceneInit();
	scene->SetStatus(SCENE_NORMAL);

	InfoLogStream << "scene(" << scene->GetID() << ")(" << scene->GetName()
		<< ") is loaded." << LogStream::eos;

	return;
}

void HandleProtocol(CLProtocol::WorldUnregisterSceneRet& protocol)
{
	Scene* scene = SceneMgr::Instance()->FindNormalScene(protocol.sceneid);
	if(scene == NULL) return;

	InfoLogStream << "scene(" << scene->GetID() << ")(" << scene->GetName()
		<< ") is unloaded." << LogStream::eos;

	scene->SetStatus(SCENE_QUIT);
}

void HandleProtocol(CLProtocol::SceneSetParam& protocol)
{
}

void HandleProtocol(Player* player, CLProtocol::SceneTestNetDelay& protocol)
{
	player->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysAsyncWaitMsgRet& protocol)
{
    AsyncMsgCallbackMgr::Instance()->OnRecvReturnMsg(protocol.id, protocol.packet);
}

void HandleProtocol(CLProtocol::SceneSummonNpc& protocol)
{
	SceneMgr::Instance()->GenActivityCityMonster();
}