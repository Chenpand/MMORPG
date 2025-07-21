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
#include "SSApplication.h"
#include <CLUltimateChallengeDungeonDataEntry.h>
#include <CLUltimateChallengeBuffDataEntry.h>
#include <CLUltimateChallengeRewardDataEntry.h>

void HandleProtocol(Player* player, CLProtocol::SceneDungeonStartReq &req)
{
    if (!player)
    {
        return;
    }

	// 加一个简单的频率控制, 同一秒内不能同时请求N次地下城开始
	do 
	{
		if (!SSApplication::Instance()->IsCheckStartDungeonFrequence() || SSApplication::Instance()->GetMaxStartDungeonNumInSecond() == 0)
		{
			break;
		}

		// 获取玩家一秒内打了几次
		struct PlayerDungeonTimes
		{
			UInt32			time;
			UInt32			num;
		};

		static std::map<ObjID_t, PlayerDungeonTimes> s_records;

		UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
		auto itr = s_records.find(player->GetID());
		if (itr == s_records.end())
		{
			PlayerDungeonTimes times;
			times.time = curTime;
			times.num = 1;
			s_records[player->GetID()] = times;
			break;
		}

		if (itr->second.time != curTime)
		{
			itr->second.time = curTime;
			itr->second.num = 1;
			break;
		}

		// 时间跟之前一致
		itr->second.num++;
		if (itr->second.num >= SSApplication::Instance()->GetMaxStartDungeonNumInSecond())
		{
			ErrorLogStream << "player(" << player->GetID() << ") start dungeon(" << req.dungeonId << ") too frequency, num(" 
							<< itr->second.num << ") max(" << SSApplication::Instance()->GetMaxStartDungeonNumInSecond() 
							<< ")." << LogStream::eos;

			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
			player->SendProtocol(res);
			return;
		}
	} while (0);
	
	SceneNpc cityMonster;

	// 如果是重新开始，通知world玩家已经离开地下城了
	if (req.isRestart)
	{
		if (!SSApplication::Instance()->IsOpenRestartDungeon())
		{
			ErrorLogStream << "player(" << player->GetID() << ") request restart dungeon(" << req.dungeonId << ") failed, closed..." << LogStream::eos;
			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
			player->SendProtocol(res);
			return;
		}

		player->NotifyLeaveDungeon(false);
	}

	if (req.cityMonsterId > 0)
	{
		auto monster = SceneMgr::Instance()->FindCityMonster(req.cityMonsterId);
		if (!monster)
		{
			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);
			return;
		}

		if (monster->GetOwnerID() > 0 && monster->GetOwnerID() != player->GetID())
		{
			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);

			ErrorLogStream << "player(" << player->GetID() << ") want to kill other player's monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ")." << LogStream::eos;
			return;
		}

		auto npc = NpcDataEntryMgr::Instance()->FindEntry(monster->GetDataID());
		if (!npc)
		{
			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
			player->SendProtocol(res);

			ErrorLogStream << "player(" << player->GetID() << ") want to kill o monster(" << monster->GetID() << ", " << monster->GetOwnerID() << ") data(" << monster->GetDataID() << ") can't find in npc data." << LogStream::eos;
			return;
		}

		if (npc->mustTeam && !player->GetTeam())
		{
			CLProtocol::SceneDungeonStartRes res;
			res.result = ErrorCode::DUNGEON_START_MUST_TEAM;
			player->SendProtocol(res);

			return;
		}

		req.dungeonId = monster->GetDungeonID();
		cityMonster = monster->GetInfo();
	}

    if (player->GetTeam())
    {
        player->GetTeam()->OnPlayerRequestStartDungeonRace(player, req.dungeonId, cityMonster);
        return;
    }

    player->GetDungeonMgr().OnStartDungeonReq(req.dungeonId, req.buffDrugs, cityMonster);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonEnterNextAreaReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonRewardReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
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

	if (player->GetTeamCopyMgr().IsTeamCopy())
	{
		player->GetTeamCopyMgr().TeamCopyDungeonRevive(req.targetId, req.reviveId, req.reviveCoinNum);
	}
	else
	{
		Router::SendToWorld(trans);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonKillMonsterReq& req)
{
	CLProtocol::GateTransmitPacket trans;
	trans.id = player->GetID();
	req.Encode(trans.packet);

	Router::SendToWorld(trans);
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

	// 终极试炼爬塔
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(notify.dungeonId);
	if (!dungeonData || dungeonData->subType != DST_ZJSL_TOWER)
	{
		return;
	}

	player->SendUdpLog("zjsl_tower", LOG_ZJSL_TOWER, player->GetCounter(COUNTER_ZJSL_TOP_FLOOR) + 1, player->GetCounter(COUNTER_ZJSL_TOP_FLOOR_TOTAL), notify.usedTime, notify.score, notify.dungeonId, player->GetCounter(COUNTER_ZJSL_DUNGEON_BUFF), player->GetCounter(COUNTER_ZJSL_INSPIRE_BUFF));

	if (notify.score <= LS_C)
	{
		player->IncCounter(COUNTER_ZJSL_CHALLENGE_TIMES, 1);

		player->SetCounter(COUNTER_ZJSL_PLAYER_HP, 0);
		player->SetCounter(COUNTER_ZJSL_PLAYER_MP, 0);
		player->SetCounter(COUNTER_ZJSL_BOSS1_ID, notify.boss1ID);
		player->SetCounter(COUNTER_ZJSL_BOSS2_ID, notify.boss2ID);
		player->SetCounter(COUNTER_ZJSL_BOSS1_HP, notify.boss1RemainHp);
		player->SetCounter(COUNTER_ZJSL_BOSS2_HP, notify.boss2RemainHp);

		//鼓舞BUFF
		UInt32 updateTime = player->GetCounterMgr().GetCounterUpdateTime(COUNTER_ZJSL_INSPIRE_BUFF_DAY);
		UInt32 now = Avalon::Time::CurrentTime().Sec();
		if (!CheckInSameLogicDay(updateTime, now) || 0 == player->GetCounter(COUNTER_ZJSL_INSPIRE_BUFF))
		{
			UltimateChallengeDungeonDataEntry* zjslDataEntry = UltimateChallengeDungeonDataEntryMgr::Instance()->FindEntry(notify.dungeonId);
			if (zjslDataEntry)
			{

				UInt32 day = player->GetCounter(COUNTER_ZJSL_INSPIRE_BUFF_DAY);
				UInt32 buffId = zjslDataEntry->GetNextInspireBuffId(day);

				player->SetCounter(COUNTER_ZJSL_INSPIRE_BUFF, buffId);
				player->SetCounter(COUNTER_ZJSL_INSPIRE_BUFF_DAY, day+1);
			}
		}
	}
	else
	{
		player->IncCounter(COUNTER_ZJSL_TOP_FLOOR, 1);
		if (player->GetCounter(COUNTER_ZJSL_TOP_FLOOR) > player->GetCounter(COUNTER_ZJSL_TOP_FLOOR_TOTAL))
		{
			player->IncCounter(COUNTER_ZJSL_TOP_FLOOR_TOTAL, 1);
		}

		player->SetCounter(COUNTER_ZJSL_PLAYER_HP, notify.playerRemainHp);
		player->SetCounter(COUNTER_ZJSL_PLAYER_MP, notify.playerRemainMp);
		player->SetCounter(COUNTER_ZJSL_BOSS1_ID, 0);
		player->SetCounter(COUNTER_ZJSL_BOSS2_ID, 0);
		player->SetCounter(COUNTER_ZJSL_BOSS1_HP, 0);
		player->SetCounter(COUNTER_ZJSL_BOSS2_HP, 0);

		player->SetCounter(COUNTER_ZJSL_INSPIRE_BUFF, 0);
		player->SetCounter(COUNTER_ZJSL_INSPIRE_BUFF_DAY, 0);

		player->SetCounter(COUNTER_ZJSL_DUNGEON_TIMES, 0);
	}
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

	player->IncExp(GetReason(SOURCE_TYPE_DUNGEON_MONSTER, notify.dungeonId, notify.monsterId).c_str(), notify.exp);
	player->OnNpcDead(notify.monsterId, notify.dungeonId, notify.isAssit);
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

	// 重置掉落时不需要添加次数了，因为在进入关卡时就已经扣了次数，只需要清空关卡标记就可以
	//																	modify by zjy.
	//																	2019.06.10
	{
		std::vector<UInt32> param = { DungeonMgr::GetNormalHardDungeonID(req.dungeonId) };
		InfoLogStream << "player(" << player->GetID() << ") reset devilddom dungeon(" << req.dungeonId << ")." << LogStream::eos;
		if (player->GetCounterMgr().GetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, param) == 0)
		{
			ErrorLogStream << "player(" << player->GetID() << ") reset devilddom dungeon(" << req.dungeonId << ") failed." << LogStream::eos;
			return;
		}
		player->GetCounterMgr().SetCounterDynamic(COUNTER_DUNGEON_PROCESS_PREFIX, 0, param);
	}

	// 通知World重置该地下城的房间Index
	CLProtocol::WorldDungeonNotifyResetAreaIndex notify;
	notify.roleId = player->GetID();
	notify.dungeonId = req.dungeonId;
	Router::SendToWorld(notify);

	CLProtocol::SceneDungeonResetAreaIndexRes res;
	res.dungeonId = req.dungeonId;
	res.areaIndex = 0;
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonZjslRefreshBuffReq& req)
{
	CLProtocol::SceneDungeonZjslRefreshBuffRes res;

	UltimateChallengeDungeonDataEntry* zjslDataEntry = UltimateChallengeDungeonDataEntryMgr::Instance()->FindEntry(req.dungeonId);
	if (!zjslDataEntry)
	{
		return;
	}

	UInt32 floor = player->GetCounter(COUNTER_ZJSL_TOP_FLOOR);
	if (zjslDataEntry->level != (floor + 1))
	{
		return;
	}

	UInt32 oldBuffId = player->GetCounter(COUNTER_ZJSL_DUNGEON_BUFF);
	UInt32 newBuffId = zjslDataEntry->RandExceptBuffId(oldBuffId);

	UltimateChallengeBuffDataEntry* zjslBuffDataEntry = UltimateChallengeBuffDataEntryMgr::Instance()->FindEntry(newBuffId);
	if (!zjslBuffDataEntry)
	{
		return;
	}

	// 使用刷新票
	if (req.useItem != 0)
	{
		UInt32 hasNum = player->GetItemNum(zjslDataEntry->dungeonBuffRefreshMat.matId);
		if (hasNum < zjslDataEntry->dungeonBuffRefreshMat.num)
		{
			res.result = ErrorCode::ITEM_NOT_ENOUGH_MAT;
			player->SendProtocol(res);
			return;
		}

		player->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_ZJSL_REFRESH_BUFF).c_str(), zjslDataEntry->dungeonBuffRefreshMat.matId, zjslDataEntry->dungeonBuffRefreshMat.num);
	}
	else
	{
		UInt32 oldBuffFloor = player->GetCounter(COUNTER_ZJSL_DUNGEON_BUFF_FLOOR);
		UltimateChallengeBuffDataEntry* oldBuffDataEntry = UltimateChallengeBuffDataEntryMgr::Instance()->FindEntry(oldBuffId);
		if (oldBuffDataEntry && zjslDataEntry->level < (oldBuffFloor + oldBuffDataEntry->sustain))
		{
			return;
		}
	}

	UInt32 buffTarget = zjslBuffDataEntry->target;
	if (0 == buffTarget)
	{
		buffTarget = Avalon::Random::RandBetween(1, 2);
	}

	player->SetCounter(COUNTER_ZJSL_DUNGEON_BUFF, newBuffId);
	player->SetCounter(COUNTER_ZJSL_DUNGEON_BUFF_TARGET, buffTarget);
	player->SetCounter(COUNTER_ZJSL_DUNGEON_BUFF_FLOOR, zjslDataEntry->level);

	if (req.useItem != 0)
	{
		res.result = ErrorCode::SUCCESS;
		player->SendProtocol(res);
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneDungeonZjslClearGetAwardReq& req)
{
	CLProtocol::SceneDungeonZjslClearGetAwardRes res;
	res.result = ErrorCode::SUCCESS;

	do 
	{
		if (player->GetCounter(COUNTER_ZJSL_CLEAR_AWARD) != 0)
		{
			res.result = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		UInt32 floor = player->GetCounter(COUNTER_ZJSL_TOP_FLOOR);
		if (floor < UltimateChallengeDungeonDataEntryMgr::Instance()->topLevel)
		{
			res.result = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		UInt32 days = player->GetCounter(COUNTER_ZJSL_CHALLENGE_DAYS);
		const UltimateChallengeRewardDataEntry* dataEntry = UltimateChallengeRewardDataEntryMgr::Instance()->GetReward(days);
		if (!dataEntry)
		{
			res.result = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		RewardGroup* rewardGroup = player->GetRewardGroup();
		rewardGroup->GetFitRewards(player, dataEntry->rewards);
		res.result = player->CheckAddRewards(rewardGroup);
		if (ErrorCode::SUCCESS != res.result)
		{
			break;
		}

		player->SetCounter(COUNTER_ZJSL_CLEAR_AWARD, 1);
		player->AddRewards(GetReason(LogSourceType::SOURCE_TYPE_ZJSL_CLEAR_GET_AWARD, dataEntry->id).c_str(), rewardGroup, true);

	} while (0);

	player->SendProtocol(res);
}
