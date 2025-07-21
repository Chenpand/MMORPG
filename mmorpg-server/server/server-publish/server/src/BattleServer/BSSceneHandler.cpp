#include "BSSceneHandler.h"

#include <CLErrorCode.h>
#include "BSNetwork.h"
#include "BSBattleMgr.h"
#include <CLSceneDefine.h>
#include <CLBattleDefine.h>

#include "BSTeamMgr.h"
#include "RobotMgr.h"
#include "BattleSceneMgr.h"
#include "BSSortListMgr.h"
#include "BSApplication.h"
#include "RelaySessionMgr.h"
#include "BSLostBattleMgr.h"
#include "BSRaceMgr.h"
#include "BSLostBattle.h"

void HandleProtocol(CLProtocol::WorldRegisterSceneReq& protocol)
{
	CLProtocol::WorldRegisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	ret.result = ErrorCode::SUCCESS;
	protocol.SendBack(ret);

	BattleSceneMgr::Instance()->OnSceneCreated(protocol.GetConnID(), protocol.sceneid);
}

void HandleProtocol(CLProtocol::WorldUnregisterSceneReq& protocol)
{
	CLProtocol::WorldUnregisterSceneRet ret;
	ret.sceneid = protocol.sceneid;
	protocol.SendBack(ret);

	BattleSceneMgr::Instance()->OnSceneDestroyed(protocol.sceneid);
}

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg)
{
	InfoLogStream << "recv relay server(" << reg.GetConnID() << ") register msg, isRestart(" << reg.isRestart << ") ability(" << reg.ability << ")." << LogStream::eos;

	RelaySessionMgr::Instance()->DelRelayServerShutdownChecker(reg.GetConnID());
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(reg.GetConnID(), reg.ability);
}

void HandleProtocol(CLProtocol::BattleSortListReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
		return;

	BSSortListMgr::Instance()->HandleSortListReq(zone, protocol.playerId, protocol.type, protocol.start, protocol.num);
}

void BattleEnterSceneReqFunction::operator()(Avalon::Packet* packet) const
{
	UInt32 accId = 0;
	ObjID_t roleId = 0;
	UInt32 gateId = 0;
	UInt32 srcSceneNodeId = 0;
	UInt32 dstSceneId = 0;
	UInt32 battleId = 0;
	UInt32 chijiScore = 0;
	UInt32 dstSceneNodeId = 0;
	UInt8  battleType = 0;
	RacePlayerInfo racePlayerInfo;

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		stream & accId & roleId & gateId & srcSceneNodeId & dstSceneId & chijiScore & dstSceneNodeId & battleId & battleType & racePlayerInfo;
	}
	catch (Avalon::NetSerializeException* e)
	{
		//
		return;
	}

	if (battleType == BTY_CHIJI)
	{
		auto battle = BSBattleMgr::Instance()->GetBattle(battleId);
		if (!battle)
		{
			ErrorLogStream << "player:" << roleId << ", can't find battle: " << battleId << LogStream::eos;
			return;
		}

		UInt32 sceneNodeId = battle->GetSceneNodeID();
		BSNetwork::Instance()->SendToBScene(sceneNodeId, packet);

		InfoLogStream << "player:" << roleId << ", enter scene node id: " << sceneNodeId << LogStream::eos;

		BSBattleMgr::Instance()->OnPlayerEnterBattleScene(roleId, sceneNodeId);

		BSSortListMgr::Instance()->SetPlayerScore(roleId, chijiScore);
	}
	else if (battleType == BTY_DILAO)
	{
		auto battle = BSLostBattleMgr::Instance()->FindBattle(battleId);
		if (!battle)
		{
			ErrorLogStream << "player:" << roleId << ", can't find lost dungeon battle: " << battleId << LogStream::eos;
			return;
		}

		UInt32 sceneNodeId = battle->GetSceneNodeID();
		battle->OnPlayerBeginEnterBattle(accId, roleId, srcSceneNodeId, sceneNodeId, racePlayerInfo);
		BSNetwork::Instance()->SendToBScene(sceneNodeId, packet);

		InfoLogStream << "player:" << roleId << ", enter lost dungeon battle scene node id: " << sceneNodeId << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& protocol)
{
	BSBattleMgr::Instance()->OnQueryPlayerRaceInfoRes(protocol.sessionId, protocol.roleId, protocol.result, protocol.racePlayerInfo);
}

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& protocol)
{
	if (protocol.raceType == RT_LOST_DUNGEON_PK)
	{
		auto race = BSRaceMgr::Instance()->FindRace(protocol.gamesessionID);
		if (!race)
		{
			return;
		}
		race->OnRelayStartRaceRes(protocol.result, protocol.gamesessionID, protocol.ability, protocol.address);
	}
	else
	{
		BSBattleMgr::Instance()->OnRelayServerStartResponse(protocol.result, protocol.gamesessionID, protocol.ability, protocol.address);
	}
}

void HandleProtocol(CLProtocol::MatchSvrEndGameReq& protocol)
{
	if (protocol.raceType == RT_LOST_DUNGEON_PK)
	{
		auto race = BSRaceMgr::Instance()->FindRace(protocol.gamesessionID);
		if (!race)
		{
			return;
		}
		race->OnRaceEnd((EGameSessionDismissType)protocol.endReason, protocol.raceEndInfo);
	}
	else
	{
		BSBattleMgr::Instance()->OnRaceEnd(protocol.gamesessionID, (EGameSessionDismissType)protocol.endReason, protocol.raceEndInfo);
	}
}

void HandleProtocol(CLProtocol::BattleEnrollReq& protocol)
{
	DebugLogStream << "player:" << protocol.roleId << ", enroll battle:" << protocol.isMatch << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) 
		return;

	CLProtocol::BattleEnrollRes res;
	res.isMatch = protocol.isMatch;

	// 报名
	if (protocol.isMatch != 0)
	{
		BSTeamMember member;
		member.m_AccId = protocol.accId;
		member.m_PlayerId = protocol.roleId;
		member.m_PlayerName = protocol.playerName;
		member.m_PlayerOccu = protocol.playerOccu;
		member.m_ConnId = protocol.GetConnID();

		res.retCode = BSTeamMgr::Instance()->CreateTeam(member);
	}
	else
	{
		// 取消报名
		UInt32 teamId = BSTeamMgr::Instance()->GetTeamID(protocol.roleId);
		if (teamId != 0)
		{
			res.retCode = BSTeamMgr::Instance()->DismissTeam(protocol.roleId, teamId);
		}
		else
		{
			res.retCode = ErrorCode::SUCCESS;
		}
	}

	zone->SendToPlayer(protocol.roleId, res);

	//通知最好成绩
	BSSortListMgr::Instance()->NotifyPlayerBestRank(protocol.roleId);
}

void HandleProtocol(CLProtocol::BattlePkSomeOneReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
	{
		return;
	}

	auto battle = BSBattleMgr::Instance()->GetBattle(protocol.battleID);
	if (!battle)
	{
		ErrorLogStream << "player:" << protocol.roleId << ", can't find battle: " << protocol.battleID << LogStream::eos;
		return;
	}

	InfoLogStream << "playerId:" << protocol.roleId << " pk dungeonId:" << protocol.dungeonID << LogStream::eos;

	// 容错处理
	if (DungeonDataEntryMgr::Instance()->FindEntry(protocol.dungeonID) == NULL)
	{
		protocol.dungeonID = 10;
	}

	UInt32 ret = battle->CheckIsCanPk(protocol.roleId, protocol.dstId);
	if (ret != ErrorCode::SUCCESS)
	{
		CLProtocol::BattlePkSomeOneRes res;
		res.roleId = protocol.roleId;
		res.dstId = protocol.dstId;
		res.retCode = ret;
		zone->SendToPlayer(res.roleId, res);
		return;
	}

	// 检查免战CD
	if (battle->CheckNoWarCD(protocol.dstId, protocol.roleId))
	{
		CLProtocol::BattlePkSomeOneRes res;
		res.roleId = protocol.roleId;
		res.dstId = protocol.dstId;
		res.retCode = ErrorCode::BATTLE_NO_WAR_CD;
		zone->SendToPlayer(res.roleId, res);
		return;
	}

	// 设置玩家免战选择状态
	battle->UpdatePlayerState(protocol.roleId, BPS_NO_WAR);
	battle->UpdatePlayerState(protocol.dstId, BPS_NO_WAR);

	// 检查被挑战玩家是否有免战牌 toScene
	CLProtocol::BattleCheckNoWarReq req;
	req.roleId = protocol.roleId;
	req.dstId = protocol.dstId;
	req.battleID = protocol.battleID;
	req.dungeonID = protocol.dungeonID;
	BSNetwork::Instance()->SendToBScene(battle->GetSceneNodeID(), req);
}

void HandleProtocol(CLProtocol::BattleLockSomeOneReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
	{
		return;
	}

	BSBattleMgr::Instance()->LockSomeOne(protocol.battleID, protocol.roleId, protocol.dstId);
}

void HandleProtocol(CLProtocol::BattleServerSpecifyPkRobot& protocol)
{
	if (0 == BSApplication::Instance()->GetRobotFlag())
	{
		return;
	}

	RobotMgr::Instance()->SetSepcifyRobotOccu(protocol.occu);
	RobotMgr::Instance()->SetSepcifyRobotHard((RobotAttrHardType)protocol.hard);
	RobotMgr::Instance()->SetSepcifyRobotAI((RobotAIHardType)protocol.ai);

	//InfoLogStream << "specify robot(" << protocol.occu << ", " << protocol.hard << ", " << protocol.ai << ")" << LogStream::eos;
}

void HandleProtocol(CLProtocol::BattleServerAddPkRobot& protocol)
{
	if (0 == BSApplication::Instance()->GetRobotFlag())
	{
		return;
	}

	BSTeamMgr::Instance()->AddRobot();
}

void HandleProtocol(CLProtocol::SceneBattleNotifySomeoneDead& protocol)
{
	BSBattleMgr::Instance()->OnSomeoneDead(protocol.battleID, protocol.playerID, protocol.killerID, protocol.reason);
}

void HandleProtocol(CLProtocol::SceneBattleCreateBattleRes& res)
{
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "battle(" << res.battleID << ") create scene failed." << LogStream::eos;
		return;
	}

	if (res.battleType == BTY_CHIJI)  //吃鸡
	{
		auto battle = BSBattleMgr::Instance()->GetBattle(res.battleID);
		if (!battle)
		{
			ErrorLogStream << "battle(" << res.battleID << ") create scene res, cant find battle." << LogStream::eos;
			return;
		}

		// 通知客户端完成匹配，可以切入战场场景
		battle->NotifySwitchBattleScene();
	}
	else if(res.battleType == BTY_DILAO) //迷失地牢
	{
		auto battle = BSLostBattleMgr::Instance()->FindBattleWithType<BSLostBattle>(res.battleID);
		if (!battle)
		{
			ErrorLogStream << "lost dungeon battle(" << res.battleID << ") create scene res, cant find battle." << LogStream::eos;
			return;
		}
		battle->SetSceneId(res.sceneId);;
		battle->SetState(LDBS_NORMAL);
		InfoLogStream << "lost dungeon battle(" << res.battleID << ")" << ", floor : " << battle->GetFloor() << ", create scene success, set state =  " << (UInt32)LDBS_NORMAL << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::BattleNotifyActivityInfo& protocol)
{
	InfoLogStream << "protocol.actName : " << protocol.actName << LogStream::eos;
	if (protocol.actName == "chiji")
	{
		BSBattleMgr::Instance()->SetActivityTime(protocol.startTime, protocol.endTime);
	}
	else if (protocol.actName == "lostDungeon")
	{
		BSLostBattleMgr::Instance()->SetActivityTime(protocol.preTime, protocol.startTime, protocol.endTime);
	}
}

void HandleProtocol(CLProtocol::BattleCheckNoWarRes& protocol)
{
	auto battle = BSBattleMgr::Instance()->GetBattle(protocol.battleID);
	if (!battle)
	{
		ErrorLogStream << "player:" << protocol.roleId << ", can't find battle: " << protocol.battleID << LogStream::eos;
		return;
	}

	// 解除玩家状态
	battle->UpdatePlayerState(protocol.roleId, BPS_ALIVE);
	battle->UpdatePlayerState(protocol.dstId, BPS_ALIVE);

	if (protocol.isNoWar == 0)
	{
		auto member = battle->GetPlayer(protocol.roleId);
		if (NULL == member)
		{
			return;
		}

		CLProtocol::BattlePkSomeOneRes res;
		res.roleId = protocol.roleId;
		res.dstId = protocol.dstId;
		res.retCode = BSBattleMgr::Instance()->PkSomeOne(protocol.battleID, protocol.dungeonID, protocol.roleId, protocol.dstId);

		member->SendProtocol(res);
	}
	else
	{
		// 加30秒免战CD
		battle->AddNoWarCD(protocol.dstId, protocol.roleId);
	}
}

void HandleProtocol(CLProtocol::BattleObserveReq& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL)
		return;

	CLProtocol::BattleObserveRes res;
	res.retCode = ErrorCode::ITEM_DATA_INVALID;

	do 
	{
		auto battle = BSBattleMgr::Instance()->GetBattle(protocol.battleId);
		if (!battle)
		{
			ErrorLogStream << "player:" << protocol.playerId << ", can't find battle: " << protocol.battleId << LogStream::eos;
			break;
		}

		auto dstPlayer = battle->GetPlayer(protocol.dstId);
		if (!dstPlayer)
		{
			ErrorLogStream << "player:" << protocol.playerId << ", can't find dstPlayer: " << protocol.dstId << LogStream::eos;
			break;
		}

		auto battleRace = BSBattleMgr::Instance()->GetBattleRace(dstPlayer->GetRaceId());
		if (!battleRace)
		{
			ErrorLogStream << "player:" << protocol.playerId << ", can't find battle race: " << dstPlayer->GetRaceId() << LogStream::eos;
			break;
		}

		res.playerId = protocol.playerId;
		res.dstId = protocol.dstId;
		res.raceId = dstPlayer->GetRaceId();
		res.addr = battleRace->GetRaceAddr();
		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	zone->SendToPlayer(protocol.playerId, res);
}

void HandleProtocol(CLProtocol::RelayAbalitySync& sync)
{
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(sync.GetConnID(), sync.abality);
}