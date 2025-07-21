#include "BSLostDungeonHandler.h"

#include <CLDungeonDataEntry.h>
#include <CLSceneDefine.h>
#include <CLBattleDefine.h>
#include <CLRelationDefine.h>

#include "BSLostBattleMgr.h"
#include "CLErrorCode.h"
#include "BSNetwork.h"
#include "GameZoneMgr.h"
#include "BSPlayer.h"
#include "BSRouter.h"
#include "BSPlayerMgr.h"
#include "BSRaceMgr.h"
#include "BSLostTeamDungeonRace.h"
#include "BSLostBattle.h"

void HandleProtocol(CLProtocol::LostDungeonMathBattleReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
		return;

	CLProtocol::LostDungeonChallengeRes res;
	res.code = ErrorCode::SUCCESS;

	if (protocol.floor == 0)
	{
		protocol.floor = 1;
	}

	auto battle = BSLostBattleMgr::Instance()->SelectOneBattle(protocol.floor);
	if (battle == NULL)
	{

		res.code = ErrorCode::LOSTDUNGEON_SELECT_BATTLE_FAIL;
		zone->SendToPlayer(protocol.playerId, res);
		return;
	}

	res.floor = battle->GetFloor();
	res.battleId = battle->GetId();
	res.battleDataId = battle->GetBattleDataId();
	res.sceneId = battle->GetSceneId();
	res.hardType = protocol.hardType;
	zone->SendToPlayer(protocol.playerId, res);
	battle->OnPlayerBeginMatchBattle(protocol.playerId);
}

void HandleProtocol(CLProtocol::BattleLostDungeonPkReq& protocol)
{
	UInt64 attackerId = protocol.attacker.roleId;
	UInt64 attackederId = protocol.attackeder.roleId;

	BSPlayer* attacker = BSPlayerMgr::Instance()->FindPlayer(attackerId);
	if (!attacker)
	{
		ErrorLogStream << " pvp fail can't find attacker id: " << attackerId << LogStream::eos;
		return;
	}

	BSPlayer* attackeder = BSPlayerMgr::Instance()->FindPlayer(attackederId);
	if (!attackeder)
	{
		ErrorLogStream << " pvp fail can't find attackeder id: " << attackederId << LogStream::eos;
		return;
	}

	auto battle = BSLostBattleMgr::Instance()->FindBattle(protocol.battleId);
	if (!battle)
	{
		ErrorLogStream << "player:" << attackerId << ", can't find lost dungeon battle: " << protocol.battleId << LogStream::eos;
		return;
	}

	InfoLogStream << "lost dungeon pk req playerId:" << attackerId << " pk dungeonId:" << protocol.dungeonId << LogStream::eos;

	// 容错处理
	if (protocol.dungeonId == 0 || DungeonDataEntryMgr::Instance()->FindEntry(protocol.dungeonId) == NULL)
	{
		protocol.dungeonId = 10;
	}

	attacker->SetRacePlayerInfo(protocol.attacker);
	attackeder->SetRacePlayerInfo(protocol.attackeder);
	CLProtocol::SceneLostDungeonPkRes res;
	res.roleId = attackerId;
	res.dstId = attackederId;
	res.retCode = battle->PkPlayer(protocol.battleId, attacker, attackeder, protocol.dungeonId);

	if (res.retCode != ErrorCode::SUCCESS)
	{
		attacker->sendToClient(res);
	}
}

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol)
{
	if (protocol.battleType == BTY_DILAO)
	{
		auto battle = BSLostBattleMgr::Instance()->FindBattleWithType<BSLostBattle>(protocol.battleId);
		if (!battle)
		{
			ErrorLogStream << "handle battle enter scene res err player:" << protocol.roleId << ", can't find lost dungeon battle: " << protocol.battleId << LogStream::eos;
			return;
		}
		BSPlayer* player = battle->FindPlayer(protocol.roleId);
		if (!player)
		{
			ErrorLogStream << "handle battle enter scene res err player:" << protocol.roleId << ", can't find player object, battle id: " << protocol.battleId << LogStream::eos;
			return;
		}
		
		battle->OnPlayerEnterBattle(player);
	}
}

void HandleProtocol(CLProtocol::BattleTeamReportVoteChoice& protocol)
{
	auto player = BSPlayerMgr::Instance()->FindPlayer(protocol.playerId);
	if (!player)
	{
		return;
	}
	auto race = BSRaceMgr::Instance()->FindRaceWithType<BSLostTeamDungeonRace>(player->GetLostRaceId());
	if (!race)
	{
		return;
	}
	race->OnPlayerReportVote(player, protocol.agree != 0);
}

void HandleProtocol(CLProtocol::BattleExitNotify& protocol)
{
	InfoLogStream << "recv BattleExitNotify playerID : " << protocol.roleId << ", battleId : "<< protocol.battleId
		<< ", battleType : " << protocol.battleType << ", battleResult : " << protocol.battleResult << LogStream::eos;
	if (protocol.battleType == BTY_DILAO)
	{
		//异常退出要特殊处理
		if (protocol.battleResult == LDBR_ABN_EXIT)
		{
			auto battle = BSLostBattleMgr::Instance()->FindBattleWithType<BSLostBattle>(protocol.battleId);
			BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
			if (battle && player)
			{
				battle->OnPlayerLeaveBattle(player);
			}

			//战场结果通知玩家源scene
			CLProtocol::SceneLostDungeonSettleFloor req;
			req.roleId = protocol.roleId;
			req.floor = battle ? battle->GetFloor() : 0;
			req.battleResult = protocol.battleResult;
			req.score = protocol.score;
			req.btState = protocol.btState;
			player->SendToSelfScene(req);

			InfoLogStream << "send SceneLostDungeonSettleFloor playerID : " << protocol.roleId << ", battleId : " << protocol.battleId
				<< ", floor : " << req.floor << ", battleResult : " << req.battleResult << ", score : " << req.score << LogStream::eos;
			return;
		}

		auto battle = BSLostBattleMgr::Instance()->FindBattleWithType<BSLostBattle>(protocol.battleId);
		if (!battle)
		{
			ErrorLogStream << "handle battle enter scene res err player:" << protocol.roleId << ", can't find lost dungeon battle: " << protocol.battleId << LogStream::eos;
			return;
		}
		BSPlayer* player = battle->FindPlayer(protocol.roleId);
		if (!player)
		{
			ErrorLogStream << "handle battle enter scene res err player:" << protocol.roleId << ", can't find player object, battle id: " << protocol.battleId << LogStream::eos;
			return;
		}

		battle->OnPlayerLeaveBattle(player);

		//战场结果通知玩家源scene
		CLProtocol::SceneLostDungeonSettleFloor req;
		req.roleId = player->GetID();
		req.floor = battle->GetFloor();
		req.battleResult = protocol.battleResult;
		req.score = protocol.score;
		player->SendToSelfScene(req);

		InfoLogStream << "send SceneLostDungeonSettleFloor playerID : " << protocol.roleId << ", battleId : " << protocol.battleId
			<< ", floor : " << req.floor << ", battleResult : " << req.battleResult << ", score : " << req.score << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::BattleDestoryRes& protocol)
{
	if (protocol.battleType == BTY_DILAO)
	{
		auto battle = BSLostBattleMgr::Instance()->FindLostBattle(protocol.battleId);
		if (battle)
		{
			battle->OnSceneBattleDestoryed();
		}
	}
}

void HandleProtocol(CLProtocol::BattleQuerylayerDetailOne& protocol)
{
	if (protocol.queryType == QPT_BATTLE_LOST)
	{
		BSPlayer* srcPlayer = BSPlayerMgr::Instance()->FindPlayer(protocol.srcRoleId);
		if (!srcPlayer)	return;
		BSPlayer* tarPlayer = BSPlayerMgr::Instance()->FindPlayer(protocol.targetRoleId);
		if (!tarPlayer)	return;

		if (srcPlayer->GetLostBattleId() != tarPlayer->GetLostBattleId())
		{
			return;
		}
		auto lostBattle = BSLostBattleMgr::Instance()->FindLostBattle(srcPlayer->GetLostBattleId());
		if (!lostBattle)
		{
			return;
		}
		lostBattle->SendToBattleScene(protocol);
	}
}

void HandleProtocol(CLProtocol::BattleSyncRacePlayerInfo& protocol)
{
	BSPlayer* srcPlayer = BSPlayerMgr::Instance()->FindPlayer(protocol.racePlayerInfo.roleId);
	if (!srcPlayer) return;
	
	srcPlayer->SetRacePlayerInfo(protocol.racePlayerInfo);
	srcPlayer->SendToBScene(protocol);
}