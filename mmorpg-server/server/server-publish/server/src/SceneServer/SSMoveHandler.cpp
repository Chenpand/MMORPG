#include "SSMoveHandler.h"

#include <AvalonCompress.h>
#include <CLLeaveGameProtocol.h>
#include <CLTaskDataEntry.h>
#include <CLSceneDataEntry.h>
#include <CLSysNotify.h>
#include <CLRecordClient.h>
#include <CLSyncObjectProtocol.h>
#include <CLCrossProtocol.h>

#include "Player.h"
#include "PlayerMgr.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "SSRouter.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "DungeonEntryScene.h"
#include "SSApplication.h"
#include "SceneChampionMgr.h"

#define WORLD_UNREGISTER_PLAYER(Id) \
	CLProtocol::WorldUnregisterPlayerReq req; \
	req.id = Id; \
	Router::SendToWorld(req);

void HandleProtocol(Player* player,CLProtocol::SceneMoveRequire& protocol)
{	
// 	if (SERVER_TYPE == ST_WSCENE)
// 	{
// 		DebugLogStream << "player(" << player->GetID() << ") require move pos(" << protocol.pos.GetX() << ", " << protocol.pos.GetY()
// 			<< ") dir(" << protocol.dir.GetX() << ", " << protocol.dir.GetY() << ")" << LogStream::eos;
// 	}
   
	// 玩家移动了，肯定不在loading中了
	player->SetClientLoading(false);

    Scene* scene = player->GetScene();
    if (!scene)
    {
        ErrorLogStream << "player(" << player->GetID() << ") not in scene." << LogStream::eos;
        return;
    }

	if (SSApplication::Instance()->IsOpenMoveCheck() && scene->GetMapID() != protocol.clientSceneId)
	{
		InfoLogStream << "player(" << player->GetID() << ") invalid move(" << protocol.pos.GetX() << ", " << protocol.pos.GetY() 
						<< ") clientScene(" << protocol.clientSceneId << ") serverScene(" << scene->GetMapID() << ")." << LogStream::eos;
		return;
	}

    if (protocol.dir.GetX() == 0 && protocol.dir.GetY() == 0)
    {
        /*InfoLogStream << "player(" << player->GetID() << ") request to stop moving cur pos(" << protocol.pos.GetX()
                    << ", " << protocol.pos.GetY() << ")." << LogStream::eos;*/

        ScenePos pos = protocol.pos;
        if (!player->CanMoveToTarget(protocol.pos))
        {
            InfoLogStream << "player(" << player->GetID() << ") request to stop at invalid pos(" << protocol.pos.GetX()
                << ", " << protocol.pos.GetY() << ")." << LogStream::eos;

            pos = player->GetScenePos();
        }

        player->RequestStopMoving(pos);
        return;
    }

    if (!player->CanMoveToTarget(protocol.pos))
    {
        return;
    }

    if (!protocol.dir.IsValid())
    {
        DebugLogStream << "player(" << player->GetID() << ") require invalid dir(" << protocol.dir.GetX() << ", " 
                        << protocol.dir.GetY() << ")" << LogStream::eos;
        return;
    }

    player->SetStatus(CREATURE_STATUS_WALK);
    player->SetRuning(true);
    player->SetSceneDir(protocol.dir);
    player->SetScenePos(protocol.pos.GetX(), protocol.pos.GetY());
    player->SetPos(protocol.pos.GetGridePos(scene->GetGridWidth(), scene->GetGridHeight()));

	//同步移动
	/*CLProtocol::SceneSyncPlayerMove syncMove;
	syncMove.id = player->GetID();
    syncMove.pos = player->GetScenePos();
    syncMove.dir = protocol.dir;

	player->SendProtocol(syncMove);*/
}

void HandleProtocol(CLProtocol::SceneBeginChangeSceneReq& protocol)
{
	if(PlayerMgr::Instance()->FindPlayer(protocol.info.id) != NULL) return;

	Scene* scene = SceneMgr::Instance()->FindNormalScene(protocol.info.destscene);
	if(scene == NULL) return;

	if(protocol.objtype == SOT_PLAYER)
	{
		if(!scene->CheckPlayerEnter(protocol.info.id, protocol.level)) return;
	}

	SceneObject* obj = SceneObject::CreateObject(protocol.objtype);
	if(obj == NULL) return;

	obj->SetID(protocol.info.id);
	obj->SetAccID(protocol.info.accid);

	CLPosition pos(protocol.pos);
	if(pos.x == 0 && pos.y == 0)
	{
		if(!scene->HasThisDoor(protocol.dstDoorId) && protocol.dstDoorId > 0)
		{
            ErrorLogStream << "player(" << protocol.info.id << ") want to change to scene(" << protocol.info.destscene 
                            << ")'s non-existent door(" << protocol.dstDoorId << ")." << LogStream::eos;

			SceneObject::FreeObject(obj);
			return;
		}
        pos = scene->GetDoorBirthPos(protocol.dstDoorId);
	}
	else
	{
		if(!scene->FindPosNearby(protocol.pos, pos, SCENE_BLOCK_PHYSICS|SCENE_BLOCK_TRANSPOINT))
		{
			SceneObject::FreeObject(obj);
			return;
		}
	}

	obj->SetPos(pos);
	obj->SetDir(protocol.dir);
	if(!obj->SetGateID(protocol.info.gateid))
	{
		SceneObject::FreeObject(obj);
		return;
	}

	//切换场景时先占个位置
	if(!scene->AddWaitObject(obj))
	{
		SceneObject::FreeObject(obj);
		return;
	}

	CLProtocol::SceneBeginChangeSceneRet ret;
	ret.info = protocol.info;
	ret.info.destnode = NODE_ID;
	ret.pos = pos;
	ret.bTransZone = protocol.bTransZone;
	Router::SendToScene(protocol.info.srcnode,ret);
}

void HandleProtocol(CLProtocol::SceneBeginChangeSceneRet& protocol)
{
	Scene* scene = SceneMgr::Instance()->FindScene(protocol.info.srcscene);
	if(scene == NULL) return;

	//查找源obj
	SceneObject* obj = scene->FindObject(protocol.info.id);
	if(obj == NULL) return;

	if(obj->GetType() == SOT_PLAYER)
	{
		Player* player = (Player*)obj;

		//从场景移除
		scene->RemoveObject(player);

		//先发送数据
		Avalon::Packet* packet = player->MakeChangeSceneDataPacket(protocol.info);
		if(packet == NULL)
		{
			player->KickOff();
			player->LeaveGame();
			return;
		}
				
		Router::SendToScene(protocol.info.destnode, packet);
		Avalon::Packet::Destroy(packet);
		
		//通知切场景
		CLProtocol::SceneChangeScene req;
		req.info = protocol.info;
		Router::SendToWorld(req);

		//从玩家管理器中移除
		PlayerMgr::Instance()->RemovePlayer(player, true);
		SceneObject::FreeObject(player);
	}
}

void TransObjectDataFunction::operator()(Avalon::Packet* packet) const
{
	size_t dataLen = 0;
	if(!Avalon::snappy_uncompressedlen((char*)packet->GetBuffer(), packet->GetSize(), &dataLen) || dataLen > Avalon::Packet::MAX_BODY_SIZE * 2)
	{
		ErrorLogStream << "decode changescene data failed:uncompressed failed!" << LogStream::eos;
		return;
	}

	char databuf[Avalon::Packet::MAX_BODY_SIZE * 2];
	if(!Avalon::snappy_uncompress(databuf, (char*)packet->GetBuffer(), packet->GetSize()))
	{
		ErrorLogStream << "decode changescene data failed:uncompressed failed!" << LogStream::eos;
		return;
	}

	Avalon::NetInputStream stream((UInt8*)databuf, dataLen);

	ChangeSceneInfo info;
	try
	{
		stream & info;
	}
	catch(const Avalon::NetException& exception)
	{
		ErrorLogStream << "decode changescene data failed:" << exception.what() << "!" << LogStream::eos;
		return;
	}

	Scene* scene = SceneMgr::Instance()->FindScene(info.destscene);
	if(scene == NULL)
	{
		if(info.accid != 0) CLFlushPlayerTables();
		//一般没这个问题
		ErrorLogStream << "can not find scene(" << info.destscene
			<< ") when object(" << info.id << ") change scene." << LogStream::eos;
 		return;
	}

	SceneObject* obj = scene->FindWaitObject(info.id);
	if(obj == NULL)
	{
		if(info.accid != 0) CLFlushPlayerTables();
		//一般没这个问题
		ErrorLogStream << "can not find object(" << info.id << ") when change scene." << LogStream::eos;
		return;
	}

	bool bSucc = false;
	try
	{
		obj->Decode(stream);
		obj->Input(stream);
		if(stream.GetPos() == dataLen)
		{
			bSucc = true;
		}
	}
	catch(const Avalon::NetException& exception)
	{
		if(obj->GetType() == SOT_PLAYER) CLFlushPlayerTables();
		//一般没这个问题
		ErrorLogStream << "decode changescene data failed:" << exception.what() << "!" << LogStream::eos;
		return;
	}
	if(!bSucc)
	{
		if(obj->GetType() == SOT_PLAYER) CLFlushPlayerTables();
		ErrorLogStream << "decode changescene data failed!" << LogStream::eos;
		return;
	}
}

void HandleProtocol(CLProtocol::SceneChangeScene& protocol)
{
	Scene* scene = SceneMgr::Instance()->FindScene(protocol.info.destscene);
	if(scene == NULL)
	{
		if(protocol.info.accid != 0)
		{
			WORLD_UNREGISTER_PLAYER(protocol.info.id);
			CLFlushPlayerTables();
		}

		ErrorLogStream << "can not find scene " << protocol.info.destscene
			<< " when object(" << protocol.info.id << ") change scene." << LogStream::eos;
		return;
	}

	SceneObject* obj = scene->FindWaitObject(protocol.info.id);
	if(obj == NULL)
	{
		if(protocol.info.accid != 0)
		{
			WORLD_UNREGISTER_PLAYER(protocol.info.id);
			CLFlushPlayerTables();
		}

		ErrorLogStream << "can not find object(" << protocol.info.id << ") when change scene." << LogStream::eos;
		return;
	}

	if(!obj->SetupBase())
	{
		if(obj->GetType() == SOT_PLAYER)
		{
			WORLD_UNREGISTER_PLAYER(protocol.info.id);
			CLFlushPlayerTables();
		}

		ErrorLogStream << "obj(" << obj->GetID() << ") setupbase failed!" << LogStream::eos;

		scene->RemoveObject(obj);
		SceneObject::FreeObject(obj);
		return;
	}

	scene->MoveOutWaitList(obj);

	if(obj->GetType() == SOT_PLAYER)
	{
		Player* player = (Player*)obj;
		
		if(!PlayerMgr::Instance()->AddPlayer(player))
		{
			GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," 
				<< player->GetName() << ") can not add to mgr when change scene!" << LogStream::eos;

			scene->RemoveObject(player);
			WORLD_UNREGISTER_PLAYER(player->GetID());
			SceneObject::FreeObject(player);
			CLFlushPlayerTables();
			return;
		}
		player->SetGameStatus(PS_NORMAL);
		
		GameInfoLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName()
			<< ") add to mgr for changing scene." << LogStream::eos;
	}

	obj->OnSceneChanged();
}

void HandleProtocol(CLProtocol::SceneEnterScenePullPlayer& protocol)
{
}

void HandleProtocol(Player* player, CLProtocol::SceneLeaveDynScene& req)
{
	player->GetScene()->KickPlayerOut(player);
}

void HandleProtocol(CLProtocol::ScenePlayerPosReq &req)
{
	Player	*pPlayer = PlayerMgr::Instance()->FindPlayer(req.uPlayerId);
	if(NULL == pPlayer) return;

	if(pPlayer->GetScene() == NULL) return;
	
	CLProtocol::ScenePlayerPosRet	ret;

	// 部落召集只能召集到静态地图
	if(1 == req.uType)
	{
		if(!IsStaticMapId(pPlayer->GetSceneID()))
		{
			pPlayer->SendNotify(895);
			return;
		}
		
		SceneDataEntry	*sceneDataEntry = SceneDataEntryMgr::Instance()->FindEntry(pPlayer->GetScene()->GetMapID());
		if(sceneDataEntry == NULL) return;
		if(IsDynMap(sceneDataEntry->type) || ((sceneDataEntry->option & SCENE_OPTION_FORBIDFLY) && sceneDataEntry->id != 112))
		{
			pPlayer->SendNotify(895);
			return;
		}

		ret.uMapId = pPlayer->GetScene()->GetMapID();
	}
	else
	{
		return;
	}
	
	ret.uPlayerId = req.uPlayerId;
	ret.pos = pPlayer->GetPos();
	ret.uType = req.uType;
	ret.uParam = req.uParam;

	req.SendBack(ret);
}

void HandleProtocol(CLProtocol::WorldMatchRaceStart& req)
{
    Player	*player = PlayerMgr::Instance()->FindPlayer(req.roleId);
    if (!player)
    {
        return;
    }

    Scene* scene = player->GetScene();
    if (!scene)
    {
        ErrorLogStream << "player(" << player->GetID() << ") not in scene." << LogStream::eos;
        return;
    }

	if (SERVER_TYPE == ST_SCENE && player->GetCrossSceneNodeID() > 0)
	{
		Router::SendToWScene(player->GetCrossSceneNodeID(), req);
		return;
	}

    CLPosition pos;
	if (SERVER_TYPE == ST_WSCENE)
	{
		player->ChangeScene(10000, pos, 0);
	}
	else
	{
		player->ChangeScene(9999, pos, 0);
	}
    
    //player->EnterCopyScene(2223, 0, 1);
}

void HandleProtocol(Player* player, CLProtocol::SceneReturnToTown& req)
{
    if (!player)
    {
        return;
    }

	// 玩家在跨服场景想回到城战，就代表要回到原城镇
	if (player->GetScene() && player->GetScene()->GetDataEntry() && SERVER_TYPE == ST_WSCENE &&
		(player->GetScene()->GetDataEntry()->subType == SCENE_SUB_CROSS_GUILD_BATTLE 
			|| player->GetScene()->GetDataEntry()->subType == SCENE_SUB_SCORE_WAR 
			|| player->GetScene()->GetDataEntry()->subType == SCENE_SUB_ROOM))
	{
		CLProtocol::CrossExitSceneReq exit;
		exit.roleId = player->GetID();
		Router::SendToSelfScene(player->GetSrcSceneNodeID(), exit);

		InfoLogStream << "player(" << player->GetID() << ") request exit cross scene." << LogStream::eos;
		return;
	}

    //DebugLogStream << "player(" << player->GetID() << ") request return to town(" << player->GetEnterSceneID() << ")." << LogStream::eos;

	UInt32 sceneId = 0;
	CLPosition pos;
	UInt32 doorId = 0;

	auto scene = player->GetScene();
	auto subType = scene && scene->GetDataEntry() ? scene->GetDataEntry()->subType : SCENE_SUB_NULL;
	if (scene == NULL || scene->GetDataEntry() == NULL)
	{
		sceneId = player->GetMajorCityID();
		auto marjorScene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
		if (marjorScene)
		{
			pos = scene->GetBirthPoint();
		}
	}
	else
	{
		if (subType == SCENE_SUB_WUDAO || subType == SCENE_SUB_ROOM)
		{
			sceneId = player->GetWudaoPrevSceneID();
			auto scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
			if (!scene || sceneId == player->GetID())
			{
				sceneId = player->GetBirthCity();
				scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
				if (scene)
				{
					pos = scene->GetBirthPoint();
				}
			}
			else
			{
				pos = player->GetWudaoPrevPos();
			}
			player->SetWudaoPrevSceneID(0);
		}
		else if (player->GetEnterSceneID() == 0 || player->GetEnterSceneID() == player->GetSceneID())
		{
			sceneId = player->GetBirthCity();
			auto scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
			if (scene)
			{
				pos = scene->GetBirthPoint();
			}
		}
		else if (scene->GetType() == SCENE_TYPE_PK_PREPARE)
		{
			// 回到主城的赛丽亚房间
			sceneId = player->GetMajorCityID();
			/*auto marjorScene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
			if (marjorScene)
			{
				pos = scene->GetBirthPoint();
			}
			else
			{
				sceneId = player->GetBirthCity();
			}*/
		}
		else if (player->GetScene() && (player->GetScene()->GetType() == SCENE_TYPE_DUNGEON_ENTRY))
		{
			DungeonEntryScene* dungeonEntryScene = dynamic_cast<DungeonEntryScene*>(player->GetScene());
			if (!dungeonEntryScene)
			{
				sceneId = player->GetEnterSceneID();
				pos = player->GetEnterPos();
			}

			sceneId = dungeonEntryScene->GetMajorCity();
			doorId = dungeonEntryScene->GetMajorDoor();
		}
		else
		{
			sceneId = player->GetEnterSceneID();
			auto scene = SceneMgr::Instance()->FindNormalScene(GetRealSceneId(sceneId));
			if (scene && scene->GetType() == SCENE_TYPE_TEAM_COPY)
			{
				// 团本直接回出生点
				pos = scene->GetBirthPoint();
			}
			else
			{
				pos = player->GetEnterPos();
			}
			if (scene->GetDataEntry()->IsChampionScene())
			{
				if (!SceneChampionMgr::Instance()->IsPlayerInChampion(player->GetID()))
				{
					InfoLogStream << "player:" << player->GetID() << "enter ChampionUnion" << LogStream::eos;
					SceneChampionMgr::Instance()->OnPlayerEnterChampionScene(player, sceneId, 0);
					return;
				}
			}
		}
	}

	DebugLogStream << "player(" << player->GetID() << ") in scene(" << player->GetSceneID() << ") request return to town(" << sceneId << ") pos(" << pos.GetX() << ", " << pos.GetY() << ")." << LogStream::eos;

	if (!player->ChangeScene(sceneId, pos, 0, true, doorId))
	{
		DebugLogStream << "player(" << player->GetID() << ") in scene(" << player->GetSceneID() << ") change to scene(" << sceneId << ") failed." << LogStream::eos;

		CLProtocol::SceneNotifyEnterScene notify;
		notify.result = ErrorCode::SCENE_CHANGE_ERROR;
		player->SendProtocol(notify);

		// 客户端一定要等这个消息，发个空的下去
		CLProtocol::SceneSyncSceneObject a;
		player->SendProtocol(a);
	}

	player->NotifyLeaveDungeon(false);
}

void HandleProtocol(Player* player, CLProtocol::ScenePlayerChangeSceneReq& req)
{
    if (!player)
    {
        return;
    }

    player->OnSceneChangeReq(req.curDoorId, req.dstSceneId, req.dstDoorId);
}