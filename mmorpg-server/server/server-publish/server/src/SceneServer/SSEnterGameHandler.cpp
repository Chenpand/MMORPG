#include "SSEnterGameHandler.h"
#include <CLErrorCode.h>
#include <CLRecordClient.h>
#include <CLLeaveGameProtocol.h>

#include "SSRouter.h"
#include "PlayerMgr.h"
#include "SceneMgr.h"
#include "Shop.h"
#include "DungeonEntryScene.h"
#include "JarItem.h"
#include "SSApplication.h"
#include "PlayerLoadCallback.h"
#include "SceneChampionMgr.h"

class LoginCallback : public PlayerLoadingCallback
{
    /**
    *@brief 加载成功
    */
    virtual void OnFinish(Player* player)
	{
		//InfoLogStream << "player(" << player->GetID() << ") loading finished, process loading." << LogStream::eos;
        player->OnDataLoaded();
    }

    /**
    *@brief 加载失败
    */
    virtual void OnFailure(Player* player) 
	{
		if (!player)
		{
			return;
		}

		ErrorLogStream << "player(" << player->GetID() << ") loading failed." << LogStream::eos;

		//进入失败
		CLProtocol::SceneEnterGameRet ret;
		ret.id = player->GetID();
		ret.result = ErrorCode::ENTERGAME_INIT_FAILED;
		player->SendToGate(ret);

		player->LeaveGame();
	}

    /**
    *@brief 加载超时
    */
    virtual void OnTimeout(Player* player) 
	{
		if (!player)
		{
			return;
		}

		ErrorLogStream << "player(" << player->GetID() << ") loading timeout." << LogStream::eos;
		OnFailure(player);
	}
};

void HandleProtocol(CLProtocol::SceneEnterGameReq& protocol)
{
    DebugLogStream << "account(" << protocol.accid << ") player(" << protocol.id << ") recv enter game request." << LogStream::eos;

	Scene *scene = SceneMgr::Instance()->FindNormalScene(protocol.sceneid);
	if (scene == NULL)
	{
		ErrorLogStream << "player(" << protocol.id << ") want enter unexist scene(" << protocol.sceneid << ")." << LogStream::eos;
		return;
	}

	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	//等待网关再次发来请求
	if (player != NULL && player->GetGameStatus() != PS_FINAL)
	{
		ErrorLogStream << "player(" << protocol.id << ") can't enter game, invalid status(" << player->GetGameStatus() << LogStream::eos;
		return;
	}

	UInt32 tourLoginInterval = SSApplication::Instance()->GetTouLoginInterval();
	UInt32 tourOnlineTime = SSApplication::Instance()->GetTourOnlineTime();

	if(player != NULL)
	{
        if (!player->SetGateID(protocol.gatenode))
        {
            ErrorLogStream << "player(" << protocol.id << ") set gate id(" << protocol.gatenode << ") failed." << LogStream::eos;
            return;
        }

		player->SetAccID(protocol.accid);
		player->SetName(protocol.name);
		player->SetGmAuthority(protocol.gmauthority);
		player->SetClientOption(protocol.clientOption);
		player->SetSceneID(protocol.sceneid);
		player->SetPos(protocol.pos);
		player->SetCity(protocol.city);
		player->SetIp(protocol.ip);
		player->SetQQVipInfo(protocol.qqVipInfo);
		player->SetSource1(protocol.source1);
		player->setSource2(protocol.source2);
		player->SetOpenId(protocol.openid);
		player->SetInviter(protocol.inviter);
		player->SetPf(protocol.pf);
		player->SetDeviceId(protocol.did);
		player->SetModel(protocol.model);
		player->SetDeviceVersion(protocol.deviceVersion);
		player->SetSrcZoneId(protocol.srcZoneId);
		player->SetPhoneBind(protocol.isPhoneBind != 0);
		player->SetBornZoneId(protocol.bornZoneId);
		player->GetAntiAddictMgr().OnInit(protocol.antiAddictInfo, tourLoginInterval, tourOnlineTime);
		player->SetGameStatus(PS_INIT);
		player->SetRoleReturnTime(protocol.roleReturnTime);
		player->SetRoleReturnLevel(protocol.roleReturnLevel);
		player->SetAgainReturn(protocol.isAgainReturn);
		player->OnCheckVeteranReturn();

		// 重新获取一下账号信息
		player->LoadAccountInfo(new LoginCallback());

		player->SetStartLoginTime(CURRENT_TIME.MSec());
	}
	else
	{
		Player* player = new Player();
		player->SetID(protocol.id);
		player->SetAccID(protocol.accid);
		player->SetName(protocol.name);
		player->SetGmAuthority(protocol.gmauthority);
		player->SetClientOption(protocol.clientOption);
		player->SetPhoneBind(protocol.isPhoneBind != 0);
		player->SetGameStatus(PS_INIT);
		if(!player->SetGateID(protocol.gatenode))
		{
			ErrorLogStream << "player(" << protocol.id << ") set gateserver(" << protocol.gatenode << ") failed." << LogStream::eos;
			SceneObject::FreeObject(player);
			return;
		}
		player->SetSceneID(protocol.sceneid);
		player->SetPos(protocol.pos);
		player->SetCity(protocol.city);
		player->SetIp(protocol.ip);
		player->SetQQVipInfo(protocol.qqVipInfo);
		player->SetSource1(protocol.source1);
		player->setSource2(protocol.source2);
		player->SetOpenId(protocol.openid);
		player->SetInviter(protocol.inviter);
		player->SetPf(protocol.pf);
		player->SetDeviceId(protocol.did);
		player->SetModel(protocol.model);
		player->SetDeviceVersion(protocol.deviceVersion);
		player->SetSrcZoneId(protocol.srcZoneId);
		player->SetBornZoneId(protocol.bornZoneId);
		player->GetAntiAddictMgr().OnInit(protocol.antiAddictInfo, tourLoginInterval, tourOnlineTime);
		player->SetRoleReturnTime(protocol.roleReturnTime);
		player->SetRoleReturnLevel(protocol.roleReturnLevel);
		player->SetAgainReturn(protocol.isAgainReturn);
		player->OnCheckVeteranReturn();

		PlayerMgr::Instance()->AddPlayer(player);

        player->StartLoadData(new LoginCallback());

		player->SetStartLoginTime(CURRENT_TIME.MSec());
	}
}

void HandleProtocol(CLProtocol::SceneGameReconnectReq& protocol)
{
    InfoLogStream << "account(" << protocol.accid << ") player(" << protocol.roleId << ") recv reconnect game request." << LogStream::eos;

    CLProtocol::SceneGameReconnectRet ret;
    ret.roleId = protocol.roleId;

    Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
    if (!player)
    {
        InfoLogStream << "account(" << protocol.accid << ") player(" << protocol.roleId << ") can't find object." << LogStream::eos;

        ret.result = 1;
        Router::SendToWorld(ret);
        return;
    }

    if (player->GetStatus() != PS_LEAVEGAME && player->GetStatus() != PS_INIT)
    {
        InfoLogStream << "account(" << protocol.accid << ") player(" << protocol.roleId << ")'s state(" << player->GetStatus() << ") error." << LogStream::eos;

        ret.result = 1;
        Router::SendToWorld(ret);

        return;
    }
    

    if (!player->SetGateID(protocol.gatenode))
    {
        ErrorLogStream << "player(" << protocol.roleId << ") set gate id(" << protocol.gatenode << ") failed." << LogStream::eos;
        ret.result = 1;
        Router::SendToWorld(ret);
        return;
    }

    ret.result = 0;
    Router::SendToWorld(ret);
    player->SetGameStatus(PS_RECONNECT);
}

void HandleProtocol(CLProtocol::WorldRegisterPlayerRet& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.id);
	if (player == NULL || player->GetGameStatus() != PS_INIT)
	{
		if (!player)
		{
			ErrorLogStream << "player(" << protocol.id << ") is null." << LogStream::eos;
		}
		else
		{
			ErrorLogStream << "player" << protocol.id << ") recv register ret, invalid status(" << (UInt8)player->GetGameStatus() << ")." << LogStream::eos;
		}

		if (protocol.result == ErrorCode::SUCCESS)
		{
			ErrorLogStream << "player" << protocol.id << ") send unregister request to world." << LogStream::eos;
			CLProtocol::WorldUnregisterPlayerReq req;
			req.id = protocol.id;
			Router::SendToWorld(req);
		}
		return;
	}

	CLProtocol::SceneEnterGameRet ret;
	ret.id = player->GetID();

	if(protocol.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player" << protocol.id << ") register failed, reason(" << protocol.result << ")." << LogStream::eos;
		ret.result = protocol.result;
		player->SendToGate(ret);

		player->LeaveGame();
		return;
	}

	//DebugLogStream << "player" << protocol.id << ") register success." << LogStream::eos;

    UInt32 initScene = player->GetBirthCity();
	Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(initScene));
	if(scene == NULL) //场景未就绪
	{
		GameErrorLogStream("ERROR") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName()
			<< ") not find scene " << player->GetSceneID() << " when online!" << LogStream::eos;

		ret.result = ErrorCode::ENTERGAME_SCENE_UNREADY;
		player->SendToGate(ret);

		player->LeaveGame();
		return;
	}
	
	//上线处理
	UInt64 offlineTime = player->GetSavebackTime();
	player->OnOnline();

	// 重连团本
	player->GetTeamCopyMgr().ReconnectTeamCopy();
	player->GetTeamCopyMgr().SetTeamCopy(false);

	if(player->GetSavebackTime() == 0 && player->GetLevel() == 1)
	{
		player->OnBirth();
	}
	player->SyncToRecord();

	CLPosition pos = scene->GetBirthPoint();
	player->SetPos(pos);

	//此处会对player ClearDirty
	if(!scene->AddObject(player))
	{
		ret.result = ErrorCode::ENTERGAME_SCENE_UNREADY;
		player->SendToGate(ret);
		
		player->LeaveGame();
		return;
	}

	player->SetGameStatus(PS_NORMAL);

	ret.result = ErrorCode::SUCCESS;
	player->SendToGate(ret);

	GameInfoLogStream("Player") << "player(" << player->GetAccID() << ","  << player->GetID() 
		<< "," << player->GetName() << ") sceneserver online!" << LogStream::eos;

	//脚本处理上线
	player->ScriptOnOnline(offlineTime);

	UInt64 loginCostTime = CURRENT_TIME.MSec() - player->GetStartLoginTime();
	DebugLogStream << PLAYERINFO(player) << " login finish cost time:" << loginCostTime << LogStream::eos;
}

void HandleProtocol(CLProtocol::SceneGameReconnectRet& protocol)
{
    DebugLogStream << "player(" << protocol.roleId << ") start to enter scene." << LogStream::eos;

    Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
    if (player == NULL || player->GetGameStatus() != PS_RECONNECT)
    {
        if (protocol.result == ErrorCode::SUCCESS)
        {
            CLProtocol::WorldUnregisterPlayerReq req;
            req.id = protocol.roleId;
            Router::SendToWorld(req);
        }
        return;
    }

    CLProtocol::SceneEnterGameRet ret;
    ret.id = player->GetID();

    if (protocol.result != ErrorCode::SUCCESS)
    {
        ret.result = protocol.result;
        player->SendToGate(ret);

        player->LeaveGame();
        return;
    }

    //上线处理
    player->OnOnline();
    player->SyncToRecord();

    if (!player->GetScene())
    {
		UInt32 initScene = PlayerMgr::Instance()->GetInitSceneID();
        DebugLogStream << "player(" << player->GetID() << ") add to scene(" << initScene << ")." << LogStream::eos;
        Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(initScene));
        if (scene == NULL) //场景未就绪
        {
            GameErrorLogStream("ERROR") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName()
                << ") not find scene " << player->GetSceneID() << " when online!" << LogStream::eos;

            ret.result = ErrorCode::ENTERGAME_SCENE_UNREADY;
            player->SendToGate(ret);

            player->LeaveGame();
            return;
        }

        player->SetPos(scene->GetBirthPoint());
        if (!scene->AddObject(player))
        {
            DebugLogStream << "scene(" << initScene << " add player(" << player->GetID() << ") failed." << LogStream::eos;
            ret.result = ErrorCode::ENTERGAME_SCENE_UNREADY;
            player->SendToGate(ret);

            player->LeaveGame();
            return;
        }
    }
    else if (player->GetScene()->GetType() == SCENE_TYPE_DUNGEON_ENTRY)
    {
		CLPosition pos;
		DungeonEntryScene* dungeonEntryScene = dynamic_cast<DungeonEntryScene*>(player->GetScene());
		if (dungeonEntryScene)
		{
			player->ChangeScene(dungeonEntryScene->GetMajorCity(), pos, 0, true, dungeonEntryScene->GetMajorDoor());
		}
		else
		{
			pos = dungeonEntryScene->GetBirthPoint();
			UInt32 initScene = player->GetBirthCity();
			Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(initScene));
			if (!scene)
			{
				ErrorLogStream << "player(" << player->GetID() << ") reconnect, can't find init scene(" << initScene << ")." << LogStream::eos;
				return;
			}

			pos = scene->GetBirthPoint();
			player->ChangeScene(initScene, pos, 0);
		}
    }
	else if (player->GetTeamCopyMgr().IsTeamCopy())
	{
		// 团本场景，直接回赛丽亚房间
		UInt32 sceneId = player->GetBirthCity();
		Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
		if (!scene)
		{
			ErrorLogStream << "player(" << player->GetID() << ") team copy reconnect, can't find init scene(" << sceneId << ")." << LogStream::eos;
		}
		else
		{
			CLPosition pos = scene->GetBirthPoint();
			player->ChangeScene(sceneId, pos, 0);

			// 重连团本
			player->GetTeamCopyMgr().ReconnectTeamCopy();
			player->GetTeamCopyMgr().SetTeamCopy(false);
			InfoLogStream << "player:" << player->GetID() << "reconnect team copy set false!" << LogStream::eos;
		}
	}
	else if (player->GetScene()->GetDataEntry()->IsChampionScene())
	{
		// 冠军赛场景，直接回赛丽亚房间
		UInt32 sceneId = player->GetBirthCity();
		Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
		if (!scene)
		{
			ErrorLogStream << "player(" << player->GetID() << ") union champion reconnect, can't find init scene(" << sceneId << ")." << LogStream::eos;
		}
		else
		{
			CLPosition pos = scene->GetBirthPoint();
			//UInt32 championSceneID = player->GetScene()->GetID();
			player->ChangeScene(sceneId, pos, 0);

			// 重连冠军赛
			//SceneChampionMgr::Instance()->OnPlayerEnterChampionScene(player, player->GetScene()->GetID(), 0);
			//InfoLogStream << "player:" << player->GetID() << "reconnect union champion" << LogStream::eos;
		}
		
	}
	else if (player->GetScene()->GetType() == SCENE_TYPE_PK ||
		player->GetScene()->GetType() == SCENE_TYPE_ACTIVITY ||
		(player->GetScene()->GetDataEntry() && player->GetScene()->GetDataEntry()->subType == SCENE_SUB_ROOM))
	{
		UInt32 sceneId = player->GetEnterSceneID();
		CLPosition pos = player->GetEnterPos();
		if (sceneId == 0)
		{
			sceneId = PlayerMgr::Instance()->GetInitSceneID();
			pos = CLPosition();
		}

		// 如果上一个场景是房间场景，直接退回到赛丽亚房间
		auto lastScene = SceneMgr::Instance()->FindNormalScene(sceneId);
		if ((lastScene && lastScene->GetDataEntry() && lastScene->GetDataEntry()->subType == SCENE_SUB_ROOM) || lastScene->GetType() == SCENE_TYPE_CHAMPION)
		{
			sceneId = player->GetBirthCity();
			Scene *scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
			if (!scene)
			{
				ErrorLogStream << "player(" << player->GetID() << ") reconnect, can't find init scene(" << sceneId << ")." << LogStream::eos;
			}
			else
			{
				pos = scene->GetBirthPoint();
			}
		}

		player->ChangeScene(sceneId, pos, 0);
	}
    else
    {
        player->OnEnterScene();
    }
    
    DebugLogStream << "player(" << player->GetID() << ") reconnect to scene(" << player->GetSceneID() << ") grid(" << player->GetGridIndex() << ")." << LogStream::eos;

    player->SetGameStatus(PS_NORMAL);

    ret.result = ErrorCode::SUCCESS;
    player->SendToGate(ret);
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneUpdateBulletinRecord& protocol)
{
	pPlayer->SetUpdateBulletin(protocol.updateVersion);
}