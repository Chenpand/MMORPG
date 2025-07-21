#include "SceneBattleHandler.h"
#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "BattleScene.h"
#include "SceneSysNotify.h"
#include "LostBattleScene.h"
#include "SSApplication.h"
#include "LostDungeonMgr.h"

#include <CLChiJiPointDataEntry.h>
#include <CLChiJiRegionDataEntry.h>
#include <CLChiJiNpcRewardDataEntry.h>
#include <CLChiJiShopTimeDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLBattleDefine.h>
#include <CLMoveProtocol.h>
#include <CLLostDungeonDefine.h>
#include <CLLostDungeonProtocol.h>

void BattleEnterSceneFunction::operator()(Avalon::Packet* packet) const
{
	UInt32 accId = 0;
	ObjID_t roleId = 0;
	Player* player = NULL;
	UInt32 gateId = 0;
	UInt32 srcSceneNodeId = 0;
	UInt32 dstSceneId = 0;
	UInt32 chijiScore = 0;
	UInt32 dstSceneNodeId = 0;
	UInt32 battleId = 0;
	UInt8  battleType = 0;
	RacePlayerInfo racePlayerInfo;

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		stream & accId & roleId & gateId & srcSceneNodeId & dstSceneId & chijiScore & dstSceneNodeId & battleId & battleType & racePlayerInfo;
		player = PlayerMgr::Instance()->FindPlayer(roleId);
		if (player)
		{
			PlayerMgr::Instance()->DelPlayer(player);
			player = NULL;
		}

		player = new Player();
		player->Decode(stream);
		player->SetID(roleId);
		player->SetSrcSceneNodeID(srcSceneNodeId);
		player->SetChijiScore(chijiScore);
		player->SetBabttleID(battleId);

		if (battleType == BTY_DILAO)
		{
			player->SetLostBattleRacePlayerInfo(racePlayerInfo);
		}

		if (!player->SetupBase() || !player->SetGateID(gateId))
		{
			// 发送返回,ToDo...
			InfoLogStream << "player(" << player->GetID() << ") set base or gate id error." << LogStream::eos;
			return;
		}
		player->SetAccID(accId);

		Scene* battleScene = SceneMgr::Instance()->GetDynamicScene(battleId);
		if (!battleScene)
		{
			InfoLogStream << "player(" << player->GetID() << ") cant find battle scene(" << battleId << ")." << LogStream::eos;
			return;
		}
		
		LostBattleScene* lostBattleScene = NULL;
		if (battleType == BTY_DILAO)
		{
			lostBattleScene = dynamic_cast<LostBattleScene*>(battleScene);
			if (!lostBattleScene)
			{
				InfoLogStream << "player(" << player->GetID() << ") cant find LostBattleScene scene(" << battleId << ")." << LogStream::eos;
				return;
			}
		}

		CLPosition pos = battleScene->GetBirthPoint();
		player->SetPos(pos);

		if (battleType == BTY_DILAO)
		{
			ItemRewardVec	items;
			stream & items;
			RewardGroup group;
			std::string reason;
			group.AddRewards(items);
			player->AddRewardsNotNotify(reason.c_str(), &group, false,0, false, false);

			if (lostBattleScene)
			{
				lostBattleScene->OnPlayerBeforeEnterBattle(player);
			}
		}

		if (!battleScene->AddObject(player))
		{
			InfoLogStream << "player(" << player->GetID() << ") enter scene(" << battleId << ") failed." << LogStream::eos;
			PlayerMgr::Instance()->DelPlayer(player);
			return;
		}

		PlayerMgr::Instance()->AddPlayer(player);

		if (battleType == BTY_DILAO && lostBattleScene)
		{
			lostBattleScene->OnPlayerEnterBattle(player);
		}

		auto scenePos = player->GetScenePos();
		InfoLogStream << "player(" << player->GetID() << ") birth pos(" << scenePos.GetX() << ", " << scenePos.GetY() << ")." << LogStream::eos;

	}
	catch (Avalon::NetSerializeException* e)
	{
		// 发送返回,ToDo...
		InfoLogStream << "decode player msg error." << LogStream::eos;
	}

	player->SetGameStatus(PS_NORMAL);
	player->SetStatus(CREATURE_STATUS_NORMAL);

	{
		auto syncPacket = player->MakeSyncPropertyPacket(SST_AROUND);
		if (syncPacket != NULL)
		{
			player->SendPacket(syncPacket);
			Avalon::Packet::Destroy(syncPacket);
		}

		// 发送吃鸡相关数值
		CLProtocol::BattleNotifyChijiMisc notify;
		notify.moveIntervalMs = SSApplication::Instance()->GetChiJiMovePacketIntervalMs();
		player->SendProtocol(notify);
	}

	InfoLogStream << "player(" << roleId << ") enter scene(" << dstSceneId << ") from scene node(" << srcSceneNodeId << ")." << LogStream::eos;

	CLProtocol::BattleEnterSceneRes res;
	res.roleId = roleId;
	res.result = ErrorCode::SUCCESS;
	res.battleSceneNodeId = NODE_ID;
	res.battleId = battleId;
	res.battleType = battleType;
	Router::SendToSelfScene(srcSceneNodeId, res);
	Router::SendToBattle(res);
}

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// 发送返回,ToDo...
		return;
	}

	if (protocol.result != ErrorCode::SUCCESS)
	{
		// 发送返回,ToDo...
		return;
	}

	player->SetBattleSceneNodeID(protocol.battleSceneNodeId);

	/*
	进入跨服场景后，将玩家切到赛利亚房间，这种情况下不能通知gate和client,因为如果通知了gate，那么gate上的该玩家就会进入赛利亚房间了
	*/
	CLPosition pos;
	UInt32 majorCity = player->GetMajorCityID();
	player->SetNotifyChangeScene(false);
	player->ChangeScene(majorCity, pos, 0);
	player->SetNotifyChangeScene(true);

	InfoLogStream << "player(" << player->GetID() << ") enter bscene, nodeid(" << protocol.battleSceneNodeId << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::BattleExitSceneReq& protocol)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		// 发送返回,ToDo...
		return;
	}

	UInt32 nodeId = player->GetBattleSceneNodeID();
	player->SetBattleSceneNodeID(0);

	//通知网关进入场景
	player->NotifyGateEnterScene();

	//通知客户端加载场景了
	player->NotifyLoadScene();

	//同步场景对象
	player->SyncAroundToMe(true);

	CLProtocol::BattleExitSceneRes res;
	res.roleId = player->GetID();
	res.result = ErrorCode::SUCCESS;
	Router::SendToBScene(nodeId, res);

	//重新调用一下
	player->OnOnline();

	InfoLogStream << "player(" << player->GetID() << ") return to scene from bscene(" << nodeId << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::BattleExitSceneRes& protocol)
{
	//InfoLogStream << "player request exit battle scene." << LogStream::eos;
	
	auto player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
	{
		return;
	}

	InfoLogStream << "player:" << player->GetID() << " exit battle scene res, result : " << protocol.result
		<< ", battleType : " << (UInt32)protocol.battleType << ", battleResult : " << protocol.battleResult << LogStream::eos;

	if (protocol.result != ErrorCode::SUCCESS)
	{
		// 发送返回,ToDo...
		return;
	}

	//地牢
	if (protocol.battleType == BTY_DILAO)
	{
		if (protocol.battleResult == LDBR_OVER) //活动结束
		{
			player->NotifyLeaveDungeon(false);
			LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(player->GetBattleID()));
			if (battleScene)
			{
				battleScene->OnPlayerLeaveBattle(player, protocol.battleResult);
			}
			else
			{
				//通知BattleServer
				CLProtocol::BattleExitNotify notify;
				notify.roleId = player->GetID();
				notify.battleId = protocol.battleId;
				notify.battleType = BTY_DILAO;
				notify.battleResult = LDBR_OVER;
				notify.score = 0;

				Router::SendToBattle(notify);
			}
			//离开游戏
			player->LeaveGame();
			InfoLogStream << "player:" << player->GetID() << ", exit battle scene." << LogStream::eos;
			return;
		}

		if (protocol.battleResult == LDBR_ABN_EXIT) //异常退出
		{
			player->NotifyLeaveDungeon(false);
			LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(player->GetBattleID()));
			if (battleScene)
			{
				battleScene->OnPlayerLeaveBattle(player, protocol.battleResult);
				LostDungeonMgr::Instance()->OnPlayerAbnExit(player);
			}
			else
			{
				//通知BattleServer
				CLProtocol::BattleExitNotify notify;
				notify.roleId = player->GetID();
				notify.battleId = protocol.battleId;
				notify.battleType = BTY_DILAO;
				notify.battleResult = LDBR_ABN_EXIT;
				notify.score = 0;
				notify.btState = player->GetLostDungBatSt();

				Router::SendToBattle(notify);
			}
			//离开游戏
			player->LeaveGame();
			InfoLogStream << "player:" << player->GetID() << ", exit battle scene." << LogStream::eos;
			return;
		}

		Scene* scene = player->GetScene();
		if (!scene) return;
		LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(player->GetBattleID()));
		if (!battleScene)	return;

		if (protocol.revive == 0)
		{
			player->NotifyLeaveDungeon(false);
			battleScene->OnPlayerLeaveBattle(player, protocol.battleResult);
		}
		else if(protocol.revive == 1)//复活
		{
			// 复活后可重新继续之前失败的任务
			player->GetLostDungeonTaskMgr().RestoreFailTask();

			player->RemoveItem(GetReason(SOURCE_LOSTDUNGEON_FAIL, battleScene->GetFloor()).c_str(), protocol.reviveItem, 1);
			InfoLogStream << "player:" << player->GetID() << ", revive in battle scene. floor : " << battleScene->GetFloor() << LogStream::eos;
			if (scene->GetType() == SCENE_TYPE_LOST_DUNGEON)
			{
				player->OnEnterScene();
				player->NotifyLeaveDungeon(false);
			}
			else if (scene->GetType() == SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC)
			{
				UInt32 sceneId = 0;
				CLPosition pos;
				UInt64 dyncSceneId = 0;
				UInt32 doorId = 0;
				dyncSceneId = player->GetEnterDyncSceneID();
				sceneId = player->GetEnterSceneID();
				pos = player->GetEnterPos();

				if (!player->ChangeScene(sceneId, pos, 0, true, doorId, dyncSceneId))
				{
					DebugLogStream << "player(" << player->GetID() << ") in scene(" << player->GetSceneID() << ") change to scene(" << sceneId << ") failed." << LogStream::eos;

					CLProtocol::SceneNotifyEnterScene notify;
					notify.result = ErrorCode::SCENE_CHANGE_ERROR;
					player->SendProtocol(notify);

					// 客户端一定要等这个消息，发个空的下去
					CLProtocol::SceneSyncSceneObject a;
					player->SendProtocol(a);
				}
			}
			else
			{
			}
			player->SetLostDungeonHp(HP_MP_FULL_VALUE / 2);
			player->SetLostDungeonMp(HP_MP_FULL_VALUE / 2);
			return;
		}
		else
		{
		}
	}

	//离开游戏
	player->LeaveGame();

	InfoLogStream << "player:" << player->GetID() << ", exit battle scene." << LogStream::eos;
}

void HandleProtocol(CLProtocol::SceneBattleCreateBattleReq& protocol)
{
	CLProtocol::SceneBattleCreateBattleRes res;
	res.battleID = protocol.battleID;
	res.battleType = protocol.battleType;

	do 
	{
		if (SERVER_TYPE != ST_BSCENE)
		{
			res.result = ErrorCode::BATTLE_CREATE_SCENE_FAILED;
			return;
		}
		if (protocol.battleType == BTY_CHIJI)
		{
			BattleScene* battleScene = dynamic_cast<BattleScene*>(SceneMgr::Instance()->CreateDynamicScene(protocol.battleID, MAPID_CHIJI_BATTLE));
			if (!battleScene)
			{
				ErrorLogStream << "battle(" << protocol.battleID << ") create battle scene failed." << LogStream::eos;
				res.result = ErrorCode::BATTLE_CREATE_SCENE_FAILED;
				break;
			}

			InfoLogStream << "battle(" << protocol.battleID << ") create battle scene(" << battleScene->GetID() << ") success." << LogStream::eos;

			battleScene->InitBattle(protocol.battleID, protocol.playerIDs);
			res.result = ErrorCode::SUCCESS;
		}
		else if (protocol.battleType == BTY_DILAO)
		{
			LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->CreateDynamicScene(protocol.battleID, protocol.sceneId));
			if (!battleScene)
			{
				ErrorLogStream << "battle(" << protocol.battleID << ") create battle scene failed." << LogStream::eos;
				res.result = ErrorCode::BATTLE_CREATE_SCENE_FAILED;
				break;
			}
			
			res.result = ErrorCode::SUCCESS;
			res.sceneId = battleScene->GetDataEntry()->id;
			battleScene->SetBattleId(protocol.battleID);
			battleScene->SetBattleDataId(protocol.battleDataId);
			battleScene->SetFloor(protocol.lostDungeonFloor);
			battleScene->Init();

			InfoLogStream << "create lost dungeon battle  success. id : " << protocol.battleID << ", scene id : " << battleScene->GetID() << ", floor"
				<< ", floor : " << battleScene->GetFloor() << LogStream::eos;
		}

	} while (0);

	protocol.SendBack(res);
}

void HandleProtocol(CLProtocol::SceneBattleDestroyBattle& protocol)
{
	if (SERVER_TYPE != ST_BSCENE)
	{
		return;
	}
	if (protocol.battleType == BTY_CHIJI)
	{
		BattleScene* battleScene = dynamic_cast<BattleScene*>(SceneMgr::Instance()->GetDynamicScene(protocol.battleID));
		if (!battleScene)
		{
			return;
		}

		InfoLogStream << "destroy battle:" << protocol.battleID << LogStream::eos;

		battleScene->DestroyBattle(protocol.battleID);
		battleScene->SetStatus(SCENE_WAIT_KICK);
	}
	else if (protocol.battleType == BTY_DILAO)
	{
		InfoLogStream << "recv req destroy lost battle : " << protocol.battleID  << ", reason : " << (UInt32)protocol.reason << LogStream::eos;
		LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->GetDynamicScene(protocol.battleID));
		if (!battleScene)
		{
			ErrorLogStream << "battle(" << protocol.battleID << ")  not find." << LogStream::eos;
			return;
		}
		battleScene->Destroy(protocol.reason);
	}
}

void HandleProtocol(CLProtocol::SceneBattleKillSomeOne& protocol)
{
	if (SERVER_TYPE != ST_BSCENE)
	{
		return;
	}

	BattleScene* battleScene = dynamic_cast<BattleScene*>(SceneMgr::Instance()->GetDynamicScene(protocol.battleID));
	if (!battleScene)
	{
		return;
	}

	battleScene->KillSomeone(protocol.battleID, protocol.playerID, protocol.killerID, protocol.remainHp, protocol.remainMp, protocol.otherID);
}

void HandleProtocol(CLProtocol::SceneBattleNotifyWaveInfo& protocol)
{
	if (SERVER_TYPE != ST_BSCENE)
	{
		return;
	}

	BattleScene* battleScene = dynamic_cast<BattleScene*>(SceneMgr::Instance()->GetDynamicScene(protocol.battleID));
	if (!battleScene)
	{
		return;
	}

	battleScene->NotifyMsgToAllPlayerByBattleID(protocol.battleID, protocol);
	battleScene->OnWaveNotify(protocol.battleID, protocol.waveID);
}

void HandleProtocol(CLProtocol::SceneBattleBalanceLog& protocol)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (player)
	{
		player->SendChiJiBalanceUdpLog(protocol.rank, protocol.survivalTime, protocol.reason);
	}
}

void HandleProtocol(CLProtocol::BattleCheckNoWarReq& protocol)
{
	Player* targetPlayer = PlayerMgr::Instance()->FindPlayer(protocol.dstId);
	if (NULL == targetPlayer)
	{
		return;
	}

	// 检查免战牌的数量
	if (targetPlayer->GetItemNum(402000004) != 0)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
		if (NULL == player)
		{
			return;
		}
		CLProtocol::SceneBattleNoWarOption option;
		option.enemyRoleId = protocol.roleId;
		option.enemyName = player->GetName();
		targetPlayer->SendProtocol(option);

		targetPlayer->SetChijiNoWarInfo(protocol.roleId, protocol.battleID, protocol.dungeonID, (UInt32)CURRENT_TIME.Sec());

		CLProtocol::SceneBattleNoWarWait wait;
		player->SendProtocol(wait);
	}
	else
	{
		// 没有免战牌，直接开战
		CLProtocol::BattleCheckNoWarRes res;
		res.roleId = protocol.roleId;
		res.dstId = protocol.dstId;
		res.battleID = protocol.battleID;
		res.dungeonID = protocol.dungeonID;
		res.isNoWar = 0;
		protocol.SendBack(res);
	}
}

void HandleProtocol(CLProtocol::BattleShopNotify& protocol)
{
	if (SERVER_TYPE != ST_BSCENE)
	{
		return;
	}

	BattleScene* battleScene = dynamic_cast<BattleScene*>(SceneMgr::Instance()->GetDynamicScene(protocol.battleId));
	if (!battleScene)
	{
		return;
	}

	battleScene->OnShopWaveNotify(protocol.battleId, protocol.shopWave);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattlePickUpItemReq& protocol)
{
	{
		LostBattleScene* lostbattleScene = dynamic_cast<LostBattleScene*>(player->GetScene());
		if (lostbattleScene)
		{
			CLProtocol::SceneBattlePickUpItemRes ret;
			ret.errcode = lostbattleScene->OnPlayerPickUpItem(player, protocol.itemid);
			player->SendProtocol(ret);
			return;
		}
	}

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (!battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	CLProtocol::SceneBattlePickUpItemRes ret;
	ret.errcode = ErrorCode::ITEM_DATA_INVALID;

	SceneItem* tmpItem = battleScene->GetSceneItemByGuid(player->GetBattleID(), player->GetID(), protocol.itemid);
	if (NULL == tmpItem)
	{
		InfoLogStream << "battle:" << player->GetBattleID() << ", player:" << player->GetID() << ", item:" << protocol.itemid << ", not exist" << LogStream::eos;
		ret.errcode = ErrorCode::ITEM_BOX_NOT_EXIST;
		player->SendProtocol(ret);
		return;
	}

	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(tmpItem->dataId);
	if (NULL == itemEntry)
	{
		InfoLogStream << "player:" << player->GetID() << ", item:" << protocol.itemid << ", data id:" << tmpItem->dataId << ", not exist" << LogStream::eos;
		player->SendProtocol(ret);
		return;
	}

	if (itemEntry->subType == ST_CHIJI_BUFF)
	{
		if (itemEntry->onUseBuff != 0)
		{
			player->AddBuff(itemEntry->onUseBuff);
		}
	}
	else
	{
		player->SendProtocol(ret);
		return;

		//UInt16 count = player->AddItem("", tmpItem->dataId, 1);
		//if (count <= 0)
		//{
		//	InfoLogStream << "player:" << player->GetID() << ", item:" << protocol.itemid << ", data id:" << tmpItem->dataId << ", add item fail" << LogStream::eos;
		//	player->SendProtocol(ret);
		//	return;
		//}
	}

	ret.errcode = ErrorCode::SUCCESS;
	player->SendProtocol(ret);

	battleScene->DeleteSceneItemByGuid(player->GetBattleID(), protocol.itemid);
	InfoLogStream << "battle:" << player->GetBattleID() << ", player:" << player->GetID() << ", item:" << protocol.itemid << ", pick up" << LogStream::eos;
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleChangeSkillsReq& protocol)
{
	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (!battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	CLProtocol::SceneBattleChangeSkillsRes res;
	if (protocol.configType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		res.result = player->GetPvpSkillMgr().ChangeSkills(protocol.changeSkills);
	}
	else
	{
		res.result = player->GetSkillMgr().ChangeSkills(protocol.changeSkills);
	}

	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneItemSync& protocol)
{
	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (!battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	battleScene->SyncAllDataToPlayer(player->GetBattleID(), player->GetID());

	// 同步一次buff
	player->GetBuffMgr().SetDirty();
	player->SyncProperty();

	//CLProtocol::SceneItemList notify;
	//notify.battleID = protocol.battleID;
	//battleScene->GetSceneItemList(protocol.battleID, player->GetID(), notify);
	//player->SendProtocol(notify);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattlePickUpSpoilsReq& protocol)
{
	//检查自己
	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (!battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	//检查对方
	BattlePlayer* dstPlayer = battleScene->GetBattlePlayer(protocol.battleID, protocol.playerID);
	if (!dstPlayer)
	{
		InfoLogStream << "dst player:" << protocol.playerID << ", not in battle" << LogStream::eos;
		return;
	}

	CLProtocol::SceneBattlePickUpSpoilsRes res;
	res.retCode = ErrorCode::ITEM_DATA_INVALID;

	if (dstPlayer->m_killerID != player->GetID())
	{
		//InfoLogStream << "player2:" << player->GetID() << ", dst player:" << protocol.playerID << ", guid:" << protocol.itemGuid << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	for (std::vector<DetailItem>::iterator iter = dstPlayer->m_items.begin(); iter != dstPlayer->m_items.end(); ++iter)
	{
		if (protocol.itemGuid == iter->guid)
		{
			//InfoLogStream << "player3:" << player->GetID() << ", dst player:" << protocol.playerID << ", guid:" << protocol.itemGuid << LogStream::eos;
			player->AddItem("", iter->itemTypeId, iter->num);
			dstPlayer->m_items.erase(iter);
			break;
		}
	}

	//InfoLogStream << "player:" << player->GetID() << ", dst player:" << protocol.playerID << ", guid:" << protocol.itemGuid << LogStream::eos;
	res.itemGuid = protocol.itemGuid;
	res.retCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleSelectOccuReq& protocol)
{
	if (NULL == player) return;

	if (!player->IsHasChijiOccu(protocol.occu))
	{
		ErrorLogStream << "player:" << player->GetID() << ", occu error:" << protocol.occu << LogStream::eos;
		return;
	}

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		ErrorLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		ErrorLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	Sys::SendUdpLog("chiji_occu", LOG_CHIJI_OCCU, player->GetBattleID(), player->GetID(), protocol.occu);

	CLProtocol::SceneBattleSelectOccuRes res;
	res.retCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);
	
	player->OnOnlineBattle();
	player->ChangeOccu(protocol.occu);

	// 由于吃鸡重新开局，客户端商店货币没有清0. 
	// 但没有计数的counter不能设为0，所以先设成1，再设为0。帮助客户端重新开局把货币刷成0
	player->SetCounter(CHIJI_SHOP_COIN_NAME, 1);
	player->SetCounter(CHIJI_SHOP_COIN_NAME, 0);

	// 主动更新一下商店
	player->GetShopMgr().SelectOccuRefresh();
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleBirthTransfer& protocol)
{
	if (SERVER_TYPE == ST_BSCENE)
	{
		DebugLogStream << "player(" << player->GetID() << ") birth transfer region(" << protocol.regionID << ")" << LogStream::eos;
	}

	// 玩家移动了，肯定不在loading中了
	player->SetClientLoading(false);

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	if (!battleScene->BeInBattle(player->GetBattleID(), player->GetID()))
	{
		ErrorLogStream << "player(" << player->GetID() << ") not in battle." << LogStream::eos;
		return;
	}

	ChiJiRegionDataEntry* regionDataEntry = ChiJiRegionDataEntryMgr::Instance()->FindEntry(protocol.regionID);
	if (!regionDataEntry)
	{
		ErrorLogStream << "player(" << player->GetID() << ") , invalid region : " << protocol.regionID << LogStream::eos;
		return;
	}

	const ScenePos* tmpPos = battleScene->RandPositionByRegionID(protocol.regionID);
	if (!tmpPos)
	{
		ErrorLogStream << "player(" << player->GetID() << ") , can't rand a point by region : " << protocol.regionID << LogStream::eos;
		return;
	}

	player->SetScenePos(tmpPos->GetX(), tmpPos->GetY());
	player->SetPos(tmpPos->GetGridePos(player->GetScene()->GetGridWidth(), player->GetScene()->GetGridHeight()));

	CLProtocol::SceneBattleBirthTransferNotify notify;
	notify.battleID = player->GetBattleID();
	notify.playerID = player->GetID();
	notify.birthPosX = tmpPos->GetX();
	notify.birthPosY = tmpPos->GetY();

	battleScene->NotifyMsgToAllPlayerByBattleID(player->GetBattleID(), notify);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleThrowSomeoneItemReq& protocol)
{
	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	CLProtocol::SceneBattleThrowSomeoneItemRes res;
	res.targetID = protocol.targetID;
	res.itemGuid = protocol.itemGuid;
	res.attackID = player->GetID();

	do 
	{
		Item* item = player->FindItem(protocol.itemGuid);
		if (NULL == item)
		{
			res.retCode = ErrorCode::ITEM_USE_FAIL;
			break;
		}

		// 找不到目标
		auto targetPlayer = PlayerMgr::Instance()->FindNormalPlayer(protocol.targetID);
		if (NULL == targetPlayer)
		{
			res.retCode = ErrorCode::BATTLE_TARGET_FAILED;
			break;
		}

		res.targetName = targetPlayer->GetName();

		BattlePlayer* targetBatttlePlayer = battleScene->GetBattlePlayer(targetPlayer->GetBattleID(), targetPlayer->GetID());
		if (NULL == targetBatttlePlayer || targetBatttlePlayer->GetStatus() != BPS_ALIVE)
		{
			res.retCode = ErrorCode::BATTLE_TARGET_PKING;
			break;
		}

		if (CURRENT_TIME.Sec() < targetBatttlePlayer->m_protectTimestamp)
		{
			res.retCode = ErrorCode::BATTLE_TARGET_INVINCIBLE;
			break;
		}

		// 计算是否在投掷范围内
		//UInt32 distance = player->GetScenePos().GetDistance(targetPlayer->GetScenePos());
		//InfoLogStream << "distance:"<< distance << LogStream::eos;

		ItemDataEntry* itemEntry = item->GetDataEntry();
		if (NULL == itemEntry)
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		// 移除道具
		player->RemoveItem("", itemEntry->id, 1);
			
		if (itemEntry->cdGroup != 0)
		{
			if (CURRENT_TIME.MSec() < player->GetItemUseTime(itemEntry->cdGroup))
			{
				res.retCode = ErrorCode::ITEM_USE_CD;
				break;
			}

			player->SetItemUseTime(itemEntry->cdGroup, CURRENT_TIME.MSec() + itemEntry->cdTime * 1000, itemEntry->cdTime);
			player->SyncProperty();
		}

		// 给目标加buff
		if (itemEntry->onUseBuff != 0)
		{
			targetPlayer->GetBuffMgr().AddBuff(itemEntry->onUseBuff);
			BuffDataEntry* buffData = BuffDataEntryMgr::Instance()->FindEntry(itemEntry->onUseBuff);
			if (NULL != buffData)
				res.param = abs(buffData->param1)/10;
			else
				ErrorLogStream << "Not Find Buff Data BuffId:" << itemEntry->onUseBuff << LogStream::eos;
		}

		res.itemDataID = itemEntry->id;
		res.retCode = ErrorCode::SUCCESS;
		targetPlayer->SendProtocol(res);

		// 吃鸡开宝箱被打断
		if (battleScene)
		{
			battleScene->OnOpenBoxBeInterrupted(targetPlayer->GetBattleID(), targetPlayer->GetID());
		}

	} while (0);

	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleDelItemReq& protocol)
{
	if (NULL == player) return;

	CLProtocol::SceneBattleDelItemRes res;

	Item* item = player->GetItemMgr().FindItem(protocol.itemGuid);
	if (NULL == item)
	{
		ErrorLogStream << "not find item guid:"<< protocol.itemGuid << LogStream::eos;
		res.retCode = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(res);
		return;
	}

	player->ReduceItemNum("", item, item->GetNum());

	res.retCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleEnterBattleReq& protocol)
{
	//InfoLogStream << "player:" << player->GetID() << ", request enter battle:" << protocol.battleID << LogStream::eos;

	CLProtocol::SceneBattleEnterBattleRes res;
	res.battleID = protocol.battleID;

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(protocol.battleID, player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	battlePlayer->m_enterTimeoutTimestamp = 0;

	player->SetBabttleID(protocol.battleID);
	player->GetItemMgr().Clear();
	player->SetChijiHp(HP_MP_FULL_VALUE);
	player->SetChijiMp(HP_MP_FULL_VALUE);
	player->GetBuffMgr().ClearBuff();

	// 检查玩家的第一波商店是否刷新
	UInt32 firstShopWave = 1;
	ChiJiShopTimeDataEntry* shopData = ChiJiShopTimeDataEntryMgr::Instance()->FindEntry(firstShopWave);
	if (shopData != NULL)
	{
		if (player->GetShopMgr().GetCurChiJiShopID() != shopData->shopID)
		{
			if (player->GetShopMgr().GetCurChiJiShopID() != 0)
			{
				player->GetShopMgr().ResetChiJiData();
			}

			ChiJiShopTimeDataEntry* nextShopData = ChiJiShopTimeDataEntryMgr::Instance()->FindEntry(firstShopWave + 1);
			if (nextShopData != NULL)
			{
				player->GetShopMgr().OnRefreshChiJiShopID(shopData->shopID, nextShopData->startTime - shopData->startTime);
				InfoLogStream << PLAYERINFO(player) << "enter battle first refresh shop!" << LogStream::eos;
			}
		}
	}
	else
	{
		ErrorLogStream << PLAYERINFO(player) << "not find chi ji shop time config id:" << firstShopWave << LogStream::eos;
	}

	res.retCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleNpcTradeReq& protocol)
{
	if (NULL == player) return;

	CLProtocol::SceneBattleNpcTradeRes res;
	res.npcId = protocol.npcId;

	do
	{
		BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
		if (!battleScene)
		{
			ErrorLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
			return;
		}

		// 检查场景中是否有这个npc
		const BattleNpcInfo* sceneNpc = battleScene->GetBattleNpcInfo(player->GetBattleID(), protocol.npcGuid);
		if (NULL == sceneNpc)
		{
			ErrorLogStream << "battle scene not find npc:" << protocol.npcId <<" npcGuid:"<<protocol.npcGuid << LogStream::eos;
			res.retCode = ErrorCode::BATTLE_NPC_NO_FIND;
			break;
		}

		NpcDataEntry* npcData = NpcDataEntryMgr::Instance()->FindEntry(protocol.npcId);
		if (NULL == npcData)
		{
			ErrorLogStream << "not find npc config:" << protocol.npcId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		UInt32 param = 0;
		std::set<Item*> itemVec;
		if (npcData->npcType == NPC_SUB_TRADER)		// npc商人, 扣装备
		{
			for (auto& iter : protocol.paramVec)
			{
				Item* item = player->GetItemMgr().FindItem(iter);
				if (NULL == item)
				{
					ErrorLogStream << "not find item guid:" << iter << LogStream::eos;
					res.retCode = ErrorCode::ITEM_DATA_INVALID;
					player->SendProtocol(res);
					return;
				}

				itemVec.insert(item);

				param += item->GetDataEntry()->color;
			}

			UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BATTLE_NPC_EXCHANGE_EQUIP_NUM);
			if (itemVec.size() < num)
			{
				ErrorLogStream << "equip num not enough! num:" << UInt32(itemVec.size()) << LogStream::eos;
				res.retCode = ErrorCode::BATTLE_EQUIP_NOT_ENOUGH;
				break;
			}

			for (auto& it : itemVec)
			{
				player->ReduceItemNum("", it, it->GetNum());
			}
		}
		else if (npcData->npcType == NPC_SUB_MONSTER) // npc怪物，扣血量
		{
			const ChiJiNpcRewardDataEntry* npcRewardEntry = ChiJiNpcRewardDataEntryMgr::Instance()->GetChiJiNpcRewardDataEntry(protocol.npcId, param);
			if (NULL == npcRewardEntry)
			{
				ErrorLogStream << "not find npc reward config npcid:"<< protocol.npcId << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			// 检查装备
			UInt32 colorSum = player->GetItemMgr().GetEquipColorSum();
			if (colorSum < npcRewardEntry->param)
			{
				ErrorLogStream << "npc equip color not enough npcid:" << protocol.npcId << " colorSum:" << colorSum << LogStream::eos;
				res.retCode = ErrorCode::BATTLE_EQUIP_NOT_ENOUGH;
				break;
			}

			// 检查血量够不够
			Int32 hp = player->GetChijiHp();
			if (UInt32(hp) <= npcRewardEntry->param2)
			{
				ErrorLogStream << "player hp not enough " << protocol.npcId << LogStream::eos;
				res.retCode = ErrorCode::BATTLE_BLOOD_NOT_ENOUGH;
				break;
			}

			// 扣血是按当前百分比扣血
			player->DecChijiHp(npcRewardEntry->param2);
			player->SyncProperty();

			// 通知场景，删除npc怪物
			battleScene->RemoveNpc(player->GetBattleID(), protocol.npcGuid, true);
		} 

		UInt32 rewardId = ChiJiNpcRewardDataEntryMgr::Instance()->GetRewards(protocol.npcId, param);
		if (rewardId == 0)
		{
			ErrorLogStream << "npc reward failed! npcId:"<< protocol.npcId << " param:"<< param << LogStream::eos;
			res.retCode = ErrorCode::BATTLE_NPC_TRADER_FAILED;
			break;
		}

		if (npcData->npcType == NPC_SUB_TRADER)		// npc商人, 扣装备
		{
			for (auto& it : itemVec)
			{
				Sys::SendUdpLog("chiji_npc", LOG_CHIJI_NPC, player->GetBattleID(), npcData->npcType, CHIJI_NPC_EXCHANGE, player->GetID(), rewardId, it->GetID());
			}
		}
		else if (npcData->npcType == NPC_SUB_MONSTER) // npc怪物，扣血量
		{
			Sys::SendUdpLog("chiji_npc", LOG_CHIJI_NPC, player->GetBattleID(), npcData->npcType, CHIJI_NPC_EXCHANGE, player->GetID(), rewardId, npcData->id);
		}

		// 给玩家发东西
		player->AddItem("", rewardId, 1);

		res.retCode = ErrorCode::SUCCESS;

		InfoLogStream << "player:"<< player->GetName() << " trade npc:"<< npcData->id << " get reward id:"<< rewardId << LogStream::eos;

	} while (0);

	player->SendProtocol(res);
}

std::vector<UInt32> TrapVec = { 400000001, 400000002, 400000003, 400000004, 400000005, 403000001 };

void HandleProtocol(Player* player, CLProtocol::SceneBattlePlaceTrapsReq& protocol)
{
	CLProtocol::SceneBattlePlaceTrapsRes res;

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	Item* item = player->GetItemMgr().FindItem(protocol.itemGuid);
	if (NULL == item || item->GetDataID() != 402000003)
	{
		ErrorLogStream << "not find item guid:" << protocol.itemGuid << LogStream::eos;
		res.retCode = ErrorCode::ITEM_DATA_INVALID;
		player->SendProtocol(res);
		return;
	}

	SceneItem* tmpItem = new SceneItem;
	if (NULL == tmpItem)
	{
		return;
	}

	Sys::SendUdpLog("chiji_traps", LOG_CHIJI_TRAPS, player->GetBattleID(), player->GetID(), 1);

	tmpItem->dataId = item->GetDataID();
	tmpItem->pos.x = protocol.x;
	tmpItem->pos.y = protocol.y;
	tmpItem->ownerId = player->GetID();
	tmpItem->disapperTime = Avalon::Time::CurrentTime().Sec() + 30;

	battleScene->AddSceneItem(player->GetBattleID(), tmpItem);

	player->ReduceItemNum("", item, 1);

	res.retCode = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	//通知消息
	CLProtocol::SceneItemAdd notify;
	notify.battleID = player->GetBattleID();

	SceneItemInfo info;
	info.sceneId = MAPID_CHIJI_BATTLE;

	std::random_shuffle(TrapVec.begin(), TrapVec.end());

	SceneItem notifyItem;
	notifyItem = *tmpItem;
	notifyItem.dataId = *TrapVec.begin();

	info.items.push_back(notifyItem);

	notify.data.push_back(info);

	battleScene->NotifyMsgToAllPlayerByBattleID(player->GetBattleID(), notify);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleOpenBoxReq& protocol)
{
	CLProtocol::SceneBattleOpenBoxRes res;
	res.itemGuid = protocol.itemGuid;
	res.param = protocol.param;
	res.retCode = ErrorCode::ITEM_DATA_INVALID;
	res.openTime = 0;

	BattleScene* battleScene = dynamic_cast<BattleScene*>(player->GetScene());
	if (!battleScene)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle scene" << LogStream::eos;
		return;
	}

	BattlePlayer* battlePlayer = battleScene->GetBattlePlayer(player->GetBattleID(), player->GetID());
	if (NULL == battlePlayer || battlePlayer->GetStatus() == BPS_OUT)
	{
		InfoLogStream << "player:" << player->GetID() << ", not in battle or dead" << LogStream::eos;
		return;
	}

	SceneItem* tmpItem = battleScene->GetSceneItemByGuid(player->GetBattleID(), player->GetID(), protocol.itemGuid);
	if (NULL == tmpItem)
	{
		InfoLogStream << "battle:" << player->GetBattleID() << ", player:" << player->GetID() << ", item:" << protocol.itemGuid << ", not exist" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_BOX_NOT_EXIST;
		player->SendProtocol(res);
		return;
	}

	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(tmpItem->dataId);
	if (NULL == itemEntry)
	{
		player->SendProtocol(res);
		return;
	}

	// 陷阱
	if (tmpItem->ownerId != 0)
	{
		if (tmpItem->ownerId == player->GetID())
		{
			player->SendProtocol(res);
			return;
		}

		player->DecChijiHp(player->GetChijiHp() * 300 / HP_MP_FULL_VALUE);

		CLProtocol::SceneBattleNotifyBeTraped notify;
		notify.battleID = player->GetBattleID();
		notify.playerID = player->GetID();
		notify.ownerID = tmpItem->ownerId;
		notify.x = tmpItem->pos.x;
		notify.y = tmpItem->pos.y;
		player->SendProtocol(notify);

		Sys::SendUdpLog("chiji_traps", LOG_CHIJI_TRAPS, player->GetBattleID(), player->GetID(), 2);

		auto targetPlayer = PlayerMgr::Instance()->FindNormalPlayer(tmpItem->ownerId);
		if (targetPlayer)
		{
			targetPlayer->SendProtocol(notify);
		}

		battleScene->DeleteSceneItemByGuid(player->GetBattleID(), protocol.itemGuid);

		res.param = 3;
		res.retCode = ErrorCode::SUCCESS;
		player->SendProtocol(res);

		player->AddBuff(401000005);
		return;
	}

	if (battlePlayer->m_openingTimestamp <= Avalon::Time::CurrentTime().Sec())
	{
		battlePlayer->m_openingBoxGuid = 0;
		battlePlayer->m_openingTimestamp = 0;
	}

	if (battlePlayer->m_openingBoxGuid != 0)
	{
		//player->SendProtocol(res);
		return;
	}

	if (1 == protocol.param)
	{
		// 打开
		if (tmpItem->openerId != 0)
		{
			//player->SendProtocol(res);
			return;
		}

		 // 检查技能是不是满了
		if(itemEntry->subType == ST_CHIJI_SKILL && !player->GetPvpSkillMgr().IsCanPickSkill())
		{
			player->SendNotify(4200014);
			return;
		}

		Sys::SendUdpLog("chiji_box", LOG_CHIJI_BOX, player->GetBattleID(), player->GetID(), tmpItem->dataId, 1);

		tmpItem->openerId = player->GetID();
		tmpItem->openTime = Avalon::Time::CurrentTime().Sec() + itemEntry->time;

		battlePlayer->m_openingBoxGuid = tmpItem->guid;
		battlePlayer->m_openingTimestamp = tmpItem->openTime;
	}
	else
	{
		player->SendProtocol(res);
		return;
	}

	res.retCode = ErrorCode::SUCCESS;
	res.openTime = tmpItem->openTime;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::BattleChoiceSkillReq& protocol)
{
	if (NULL == player) return;

	player->GetPvpSkillMgr().ChoiceChiJiSkill(protocol.skillId);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleNoWarChoiceReq& protocol)
{
	if (NULL == player) return;

	if (player->GetChijiNoWarChoiceTime() == 0)
	{
		ErrorLogStream << "chi ji no war timeout player:" << player->GetID() << LogStream::eos;
		return;
	}

	// 免战
	if (protocol.isNoWar != 0)
	{
		do 
		{
			if (player->GetItemNum(402000004) == 0)
			{
				CLProtocol::SceneBattleNoWarChoiceRes res;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				player->SendProtocol(res);
				break;
			}

			player->RemoveItem("", 402000004, 1);

			// 通知玩家被免战
			Player* notifyPlayer = PlayerMgr::Instance()->FindPlayer(player->GetChijiNoWarEnemyId());
			if (notifyPlayer != NULL)
			{
				CLProtocol::SceneBattleNoWarNotify notify;
				notifyPlayer->SendProtocol(notify);
			}
		} while (0);
	}

	CLProtocol::BattleCheckNoWarRes checkRes;
	checkRes.roleId = player->GetChijiNoWarEnemyId();
	checkRes.dstId = player->GetID();
	checkRes.battleID = player->GetChijiNoWarBattleId();
	checkRes.dungeonID = player->GetChijiPKDungeonId();
	checkRes.isNoWar = protocol.isNoWar;
	Router::SendToBattle(checkRes);

	player->SetChijiNoWarInfo(0, 0, 0, 0);

	Sys::SendUdpLog("chiji_no_war", LOG_CHIJI_NO_WAR_CARD, player->GetBattleID(), player->GetID(), player->GetItemNum(402000004), protocol.isNoWar);
}

void HandleProtocol(Player* player, CLProtocol::SceneBattleOccuListReq& protocol)
{
	if (NULL == player) return;

	player->CreateChijiOccu();
}

void HandleProtocol(Player* player, CLProtocol::BattleChoiceEquipReq& protocol)
{
	if (NULL == player) return;

	CLProtocol::BattleChoiceEquipRes res;
	res.retCode = player->GetItemMgr().ChoiceChiJiEquip(protocol.equipId);
	res.equipId = protocol.equipId;
	player->SendProtocol(res);
}