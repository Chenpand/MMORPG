#include "SceneLostDungeonHandler.h"
#include <CLRelationDefine.h>
#include <CLRelationProtocol.h>

#include "CLErrorCode.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "LostDungeonMgr.h"

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonSwitchChageModeReq& req)
{
	if (!player) return;
	
	CLProtocol::SceneLostDungeonSwitchChageModeRes res;
	res.code = ErrorCode::SUCCESS;
	if (player->GetDungChageMode() == req.chageMode)
	{
		res.code = ErrorCode::ITEM_INVALID_INDEX;
		player->SendProtocol(res);
		return;
	}
	res.code = LostDungeonMgr::Instance()->OnPlayerSwitchChallengeMode(player, req.chageMode);
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonPkReq& req)
{
	if (!player) return;
	CLProtocol::SceneLostDungeonPkRes res;
	res.retCode = ErrorCode::SUCCESS;

	auto dstPlayer = PlayerMgr::Instance()->FindNormalPlayer(req.dstId);
	if (!dstPlayer)
	{
		res.retCode = ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_LEAVING;
		player->SendProtocol(res);
		return;
	}

	res.retCode = LostDungeonMgr::Instance()->OnPlayerPkReq(player, dstPlayer, req.dungeonID);
	if (res.retCode != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonTaskListReq& req)
{
	if (!player) return;
	player->GetLostDungeonTaskMgr().HandlerRandTaskList(req.grade);
}

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonChoiceTaskReq& req)
{
	if (!player) return;
	player->GetLostDungeonTaskMgr().HandlerChoiceTask(req.taskId);
}

void HandleProtocol(Player* player, CLProtocol::BattleLostDungSeeIntellReq& req)
{
	if (!player) return;
	CLProtocol::BattleLostDungSeeIntellRes res;
	res.playerId = req.playerId;

	res.code = player->OnPlayerSeeIntell(req.playerId, res.artifacts);

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::BattleSynLostDungTeamSt& req)
{
	LostDungeonMgr::Instance()->SetTeamState(req.battleId, req.teamId, req.state);
}

void HandleProtocol(Player* player, CLProtocol::SceneQueryKillTargetPosReq& req)
{
	if (!player) return;
	CLProtocol::SceneQueryKillTargetPosRes res;
	player->GetLostDunegonTaskKillTargetPos(req.playerIds, res.playerPos);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::BattleQuerylayerDetailOne& req)
{
	if (req.queryType == QPT_BATTLE_LOST)
	{
		Player* srcPlayer = PlayerMgr::Instance()->FindPlayer(req.srcRoleId);
		if (!srcPlayer)	return;
		Player* tarPlayer = PlayerMgr::Instance()->FindPlayer(req.targetRoleId);
		if (!tarPlayer)	return;

		UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_QIBAO_ITEM);

		if (srcPlayer->GetItemNum(itemId) < 1)
		{
			return;
		}
		CLProtocol::WorldQueryPlayerDetailsRet res;
		res.info = tarPlayer->GetLostBattleRacePlayerInfo();
		srcPlayer->SendProtocol(res);
	}
}

void HandleProtocol(CLProtocol::BattleNotifyTeamRaceEnd& req)
{
	LostDungeonMgr::Instance()->OnTeamRaceEnd(req.teamId);
}

void HandleProtocol(CLProtocol::BattleSyncRacePlayerInfo& req)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(req.racePlayerInfo.roleId);
	if (!player)	return;

	player->SetLostBattleRacePlayerInfo(req.racePlayerInfo);
	player->CountMoveSpeed();
}