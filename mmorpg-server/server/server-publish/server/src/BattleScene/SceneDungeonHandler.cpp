#include "SceneDungeonHandler.h"
#include <CLGateProtocol.h>
#include "Player.h"
#include "DungeonMgr.h"
#include "TeamMgr.h"
#include "PlayerMgr.h"
#include "SortListMgr.h"
#include "SSRouter.h"
#include "SceneMgr.h"
#include "CityMonster.h"
#include "CLNpcDataEntry.h"
#include "LostDungeonMgr.h"
#include "LostDungeonTeam.h"
#include "LostBattleScene.h"

void HandleProtocol(Player* player, CLProtocol::SceneDungeonStartReq &req)
{
    if (!player)
    {
        return;
    }
	InfoLogStream << "player(" << player->GetID() << "), req start dungeon race dungeonId : " << req.dungeonId << ", cityMonsterId : " << req.cityMonsterId << LogStream::eos;
	CLProtocol::SceneDungeonStartRes res;

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(req.dungeonId);
	if (!dungeonData)
	{
		res.result = ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
		player->SendProtocol(res);
		InfoLogStream << "player(" << player->GetID() << "), req start dungeon race fail, not find dungeon, dungeonId : " << req.dungeonId << ", cityMonsterId : " << req.cityMonsterId << LogStream::eos;
		return;
	}

	SceneNpc battleMonster;
	if (req.cityMonsterId > 0)
	{
		auto monster = SceneMgr::Instance()->FindCityMonster(req.cityMonsterId);
		if (!monster)
		{
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);
			InfoLogStream << "player(" << player->GetID() << "), req start dungeon race fail, not find monster, dungeonId : " << req.dungeonId << ", cityMonsterId : " << req.cityMonsterId << LogStream::eos;
			return;
		}
		if (monster->GetOwnerID() > 0 && monster->GetOwnerID() != player->GetID())
		{
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);

			ErrorLogStream << "player(" << player->GetID() << ") want to kill other player's monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ")." << LogStream::eos;
			return;
		}

		auto tmpResult = monster->CanFight();
		if (tmpResult != ErrorCode::SUCCESS)
		{
			DebugLogStream << "player(" << player->GetID() << ") monster(" << monster->ToString() << ") result(" << tmpResult << ")." << LogStream::eos;
			res.result = tmpResult;
			player->SendProtocol(res);
			return;
		}

		auto npc = NpcDataEntryMgr::Instance()->FindEntry(monster->GetDataID());
		if (!npc)
		{
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);

			ErrorLogStream << "player(" << player->GetID() << ") want to kill o monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ") data(" << monster->GetDataID() << ") can't find in npc data." << LogStream::eos;
			return;
		}

		if (npc->mustTeam && !player->GetTeam())
		{
			res.result = ErrorCode::DUNGEON_START_MUST_TEAM;
			player->SendProtocol(res);

			return;
		}
		req.dungeonId = monster->GetDungeonID();
		battleMonster = monster->GetInfo();
	}

	CLProtocol::DungenServerTeamStartRaceReq protocl;
	protocl.teamId = 0;
	protocl.dungeonId = req.dungeonId;
	protocl.passedAreaIndex = 0;
	protocl.monster = battleMonster;
	protocl.sourceSceneNodeId = NODE_ID;
	//组队
	if (player->GetDungChageTeamId() > 0)
	{
		if (!player->GetScene())	return;
		UInt32 sceneId = player->GetScene()->GetID();

		LostDungeonTeam* team = LostDungeonMgr::Instance()->GetPlayerTeam(sceneId, player->GetID());
		if (!team)	return;

		LostDungTeamMember* member = team->FindMember(player->GetID());
		if (!member)
		{
			return;
		}
		if (member->pos != 1)
		{
			player->SendNotify(1036);
			return;
		}
		protocl.teamId = player->GetDungChageTeamId();
		UInt8 pos = 0;
		team->VistMembers([&](LostDungeonTeam* team, LostDungTeamMember* teamMember) {
			if (!team || !teamMember) return true;
			Player* teamPlayer = PlayerMgr::Instance()->FindPlayer(teamMember->roleId);
			if (!teamPlayer)
			{
				return true;
			}
			CLProtocol::DungeonFighter fighter;
			fighter.pos = pos;
			fighter.accId = teamPlayer->GetAccID();
			fighter.roleId = teamPlayer->GetID();
			fighter.wsId = 0;
			fighter.serviceId = GetBattleNodeID(0);
			fighter.sourceSceneId = NODE_ID;
			protocl.fighters.push_back(fighter);

			DungeonRacePlayerInfo dungeonRaceInfo = teamPlayer->GetDungeonMgr().GetDungeonPlayerInfo(req.dungeonId);
			dungeonRaceInfo.raceInfo = teamPlayer->GetLostBattleRacePlayerInfo();
			dungeonRaceInfo.raceInfo.pos = pos;
			dungeonRaceInfo.soureSceneId = NODE_ID;
			protocl.playerInfoes.push_back(dungeonRaceInfo);
			
			pos++;
			return true;
		});

		team->SetBattleState(LDTBS_MATCH);
	}
	else
	{
		CLProtocol::DungeonFighter fighter;
		fighter.pos = 0;
		fighter.accId = player->GetAccID();
		fighter.roleId = player->GetID();
		fighter.wsId = 0;
		fighter.serviceId = GetBattleNodeID(0);
		fighter.sourceSceneId = NODE_ID;
		protocl.fighters.push_back(fighter);

		DungeonRacePlayerInfo dungeonRaceInfo = player->GetDungeonMgr().GetDungeonPlayerInfo(req.dungeonId);
		dungeonRaceInfo.raceInfo = player->GetLostBattleRacePlayerInfo();
		dungeonRaceInfo.raceInfo.pos = 0;
		dungeonRaceInfo.soureSceneId = NODE_ID;
		protocl.playerInfoes.push_back(dungeonRaceInfo);
	}

	if (req.cityMonsterId > 0)
	{
		player->SetLostDungBatSt(LDPBT_BATTLE_PVE);
	}

	Router::SendToBattle(protocl);
	InfoLogStream << "player(" << player->GetID() << "), req start dungeon race, send proto to battle server,teamId : " << protocl.teamId
		<< ", dungeonId : " << protocl.dungeonId << ", cityMonsterId : " << req.cityMonsterId << LogStream::eos;
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonEnterNextAreaReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToDungeon(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonRewardReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToDungeon(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonRaceEndReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonOpenChestReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonEndDropReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonReviveReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	UInt32 sceneId = player->GetLastLostBattleSceneId();
	LostBattleScene* battleScene = dynamic_cast<LostBattleScene*>(SceneMgr::Instance()->FindNormalScene(sceneId));
	if (battleScene)
	{
		battleScene->OnPlayerRevive(player->GetID(), req.targetId, req.reviveId, req.reviveItem, req.reviveItemNum);
		return;
	}

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonKillMonsterReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToDungeon(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonClearAreaMonsters& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::WorldDungeonEnterRaceReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::WorldDungeonReportFrameReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(CLProtocol::SceneDungeonChestRes& res)
{
    
}

void HandleProtocol(CLProtocol::WorldDungeonStartRaceRes& res)
{
	auto player = PlayerMgr::Instance()->FindPlayer(res.roleId);
	if (!player)
	{
		ErrorLogStream << "world start dungeon(" << res.dungeonId << ") returned, player(" << res.roleId << ") offline." << LogStream::eos;
		return;
	}

	player->SetStatus(CREATURE_STATUS_PK);
	player->GetDungeonMgr().OnWorldStartDungeonRes(res.dungeonId, (res.isHell != 0), res.isContinue != 0, res.isAssist != 0, res.result);
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyRaceEnd& notify)
{
	auto player = PlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		ErrorLogStream << "world dungeon(" << notify.dungeonId << ") race end, player(" << notify.roleId << ") offline." << LogStream::eos;
		return;
	}

	player->SetStatus(CREATURE_STATUS_NORMAL);

	if (notify.score <= LS_C)
	{
		player->GetLostDungeonTaskMgr().OnOwnerDead();
	}
	else
	{
		// 爬塔任务
		player->GetLostDungeonTaskMgr().OnClearDungeon(notify.dungeonId);
	}

	// 结算经验
	player->IncExp(GetReason(SOURCE_TYPE_DUNGEON_END, notify.dungeonId).c_str(), notify.raceEndExp);

	// 记录关卡信息
	player->GetDungeonMgr().OnDungeonRaceEnd(notify.dungeonId, (LevelScore)notify.score, notify.beHitCount, 
												notify.enteredAreaCount, notify.usedTime, notify.reviveCount, 
												notify.endAreaId, notify.isAssist != 0, notify.teammateIds);

	player->GetShopMgr().OnDungeonRaceEnd(notify.dungeonId, notify.mysticalMerchantId);

	// 组队奖励
	if (notify.teamReward.id > 0 && notify.teamReward.num > 0)
	{
		RewardGroup group;
		group.AddReward(notify.teamReward.id, notify.teamReward.num);
		player->AddRewards(GetReason(SOURCE_TYPE_DUNGEON_TEAM_REWARD, notify.dungeonId).c_str(), &group, true);
	}

	// 月卡奖励
	if (notify.monthcardGoldReward > 0)
	{
		player->AddGold(GetReason(SOURCE_TYPE_DUNGEON_MONTH_CARD_GOLD_REWARD, notify.dungeonId).c_str(), notify.monthcardGoldReward);
	}

	// 金币称号奖励
	if (notify.goldTitleGoldReward > 0)
	{
		player->AddGold(GetReason(SOURCE_TYPE_DUNGEON_GOLD_TITLE_GOLD_REWARD, notify.dungeonId).c_str(), notify.goldTitleGoldReward);
	}

	// 杀死怪物
	if (notify.score > LS_C && notify.cityMonster.guid > 0)
	{
		player->OnKillCityMonster(notify.raceId, notify.cityMonster, notify.teammateIds);
	}

	// 结算奖励
	RewardGroup group;
	group.AddRewards(notify.raceEndRewards);
	player->AddRewards(GetReason(SOURCE_TYPE_DUNGEON_RACE_END, notify.dungeonId).c_str(), &group);
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyClearAreaMonster& notify)
{
	auto player = PlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		ErrorLogStream << "world dungeon(" << notify.dungeonId << ") race end, player(" << notify.roleId << ") offline." << LogStream::eos;
		return;
	}

	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(notify.dungeonId);
	if (!dungeonData || dungeonData->type != LT_TOWER)
	{
		return;
	}

	player->OnClearDeathTowerFloorMonsters(notify.dungeonId, notify.areaId, notify.usedTime, notify.remainHp, notify.remainMp);
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyConsumeFatigue& notify)
{
	auto player = PlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		ErrorLogStream << "player(" << notify.roleId << ") consume fatigue(" << notify.value << "), but player is offline." << LogStream::eos;
		return;
	}

	DebugLogStream << "player(" << notify.roleId << ") consume fatigue(" << notify.value << ") in dungeon(" << notify.dungeonDataId << ")." << LogStream::eos;

	player->RemoveFatigue(GetReason(SOURCE_TYPE_DUNGEON, notify.dungeonDataId).c_str(), notify.value);
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyKillMonster& notify)
{
	auto player = PlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		ErrorLogStream << "player(" << notify.roleId << ") dungeon(" << notify.dungeonId << ") kill monster(" << notify.monsterId << "), but player is offline." << LogStream::eos;
		return;
	}

	// 爬塔任务
	player->GetLostDungeonTaskMgr().OnKillMonster(notify.monsterId, 1);

	player->IncExp(GetReason(SOURCE_TYPE_DUNGEON_MONSTER, notify.dungeonId, notify.monsterId).c_str(), notify.exp);
	player->OnNpcDead(notify.monsterId, notify.dungeonId);
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyGotItems& notify)
{
	auto player = PlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		ErrorLogStream << "player(" << notify.roleId << ") dungeon(" << notify.dungeonId << ") got items, but player is offline." << LogStream::eos;
		return;
	}

	RewardGroup group;
	group.AddRewards(notify.items);
	std::string reasonStr = notify.reason.empty() ? GetReason(SOURCE_TYPE_DUNGEON, notify.dungeonId) : notify.reason;
	player->AddRewards(reasonStr.c_str(), &group, true, 1, true, true);

	if (notify.gold > 0)
	{
		player->AddBindGold(reasonStr.c_str(), notify.gold);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneTowerWipeoutRes res;
	res.result = player->OnBeginWipeoutDeathTower();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutResultReq& req)
{
	if (!player)
	{
		return;
	}
	
	CLProtocol::SceneTowerWipeoutResultRes res;
	res. result = player->OnRequestDeathTowerWipeoutResult();
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutQueryResultReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneTowerWipeoutQueryResultRes res;
	res.result = player->OnRequestDeathTowerWipeouttQueryResult(req.beginFloor, req.endFloor);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutQuickFinishReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneTowerWipeoutQuickFinishRes res;
	res.result = player->OnQuickFinishDeathTowerWipeout();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerResetReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneTowerResetRes res;
	res.result = player->OnResetDeathTower();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneTowerFloorAwardReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneTowerFloorAwardRes res;
	res.result = player->OnRequestDeathTowerPassFloorAward(req.floor);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonBuyTimesReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneDungeonBuyTimesRes res;
	res.result = player->RequestBuyDungeonTimes((DungeonSubType)req.subType);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonResetAreaIndexReq& req)
{
	if (!player)
	{
		return;
	}

	player->GetDungeonMgr().TryRecordDailyCount(req.dungeonId);

	CLProtocol::SceneDungeonResetAreaIndexRes res;
	res.dungeonId = req.dungeonId;
	res.areaIndex = 0;
	player->SendProtocol(res);
}

