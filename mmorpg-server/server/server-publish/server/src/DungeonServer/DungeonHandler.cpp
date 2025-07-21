#include "DungeonHandler.h"
#include "RelaySessionMgr.h"
#include "DungeonRaceMgr.h"
#include "DSRouter.h"
#include "CLErrorCode.h"
#include "DSApplication.h"
#include "DSNetwork.h"
#include "SceneSessionMgr.h"

//void HandleProtocol(Protocol::DungenServerTeamCreateRaceReq& req)
//{
//	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
//	if (!relaySession)
//	{
//		return;
//	}
//
//	Protocol::DungenServerTeamCreateRaceRes res;
//
//	auto dungeonRace = DungeonRaceMgr::Instance()->CreateDungeonRace(req.dungeonId);
//	if (!dungeonRace)
//	{
//		res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
//		res.teamId = req.teamId;
//		req.SendBack(res);
//		return;
//	}
//
//	dungeonRace->SetServiceId(req.GetConnID());
//	dungeonRace->SetTeamID(req.teamId);
//	dungeonRace->SetStatus(DRS_CREATING);
//
//	Protocol::MatchSvrStartGameReq startReq;
//	startReq.raceType = RT_DUNGEON_CROSS;
//	startReq.gamesessionID = dungeonRace->GetSessionId();
//	for (auto fighter : req.fighters)
//	{
//		startReq.gamesessionFighter.push_back(fighter);
//	}
//	relaySession->SendProtocol(startReq);
//}

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceReq& req)
{
	DebugLogStream << "recv DungenServerTeamStartRaceReq, teamId : " << req.teamId << ", dungeonId : " << req.dungeonId
		<< "battleTempRaceId : " << req.battleTempRaceId << LogStream::eos;
	CLProtocol::DungenServerTeamStartRaceRes res;
	res.battleTempRaceId = req.battleTempRaceId;
	res.teamId = req.teamId;

	if (req.fighters.size() == 0 || req.playerInfoes.size() == 0)
	{
		ErrorLogStream << "start race fail, teamId : " << req.teamId << ", req.fighters size : " << (UInt32)(req.fighters.size())
			<< ", req.playerInfoes size : " << (UInt32)(req.playerInfoes.size()) << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
		req.SendBack(res);
		return;
	}

	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		ErrorLogStream << "start race fail, teamId : " << req.teamId << ", SelectRelayServer fail!" << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
		req.SendBack(res);
		return;
	}

	
	auto dungeonRace = DungeonRaceMgr::Instance()->CreateDungeonRace(req.dungeonId);
	if (!dungeonRace)
	{
		ErrorLogStream << "start race fail, teamId : " << req.teamId << ", CreateDungeonRace fail!" << LogStream::eos;
		res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
		req.SendBack(res);
		return;
	}

	dungeonRace->SetServiceId(req.GetConnID());
	dungeonRace->SetTeamID(req.teamId);
	dungeonRace->SetStatus(DRS_CREATING);
	dungeonRace->SetPlayerInfos(req.playerInfoes);
	dungeonRace->SetPassedAreaIndex(req.passedAreaIndex);
	dungeonRace->SetClearedDungeonIds(req.clearedDungeonIds);
	dungeonRace->SetFighters(req.fighters);
	dungeonRace->SetMonster(req.monster);
	dungeonRace->SetBattleTempRaceId(req.battleTempRaceId);
	dungeonRace->SetSourceSceneNodeId(req.sourceSceneNodeId);
#ifdef _DEBUG
	for (auto fighter : dungeonRace->GetFighters())
	{
		DebugLogStream << "race(" << dungeonRace->GetID() << ") fighter(" << fighter.roleId << ") wsId(" << fighter.wsId << ")." << LogStream::eos;
	}
#endif

	CLProtocol::MatchSvrStartGameReq startReq;
	startReq.raceType = RT_DUNGEON_CROSS;
	startReq.gamesessionID = dungeonRace->GetSessionId();
	for (auto fighter : req.fighters)
	{
		startReq.gamesessionFighter.push_back(fighter);
	}
	relaySession->SendProtocol(startReq);
	return;
}

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& res)
{
	DebugLogStream << "recv MatchSvrStartGameRes, gamesessionID : " << res.gamesessionID << ", result : " << res.result << LogStream::eos;
	DungeonRace* dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(res.gamesessionID);
	if (!dungeonRace)
	{
		ErrorLogStream << "not find dungeon race session id : " << res.gamesessionID << LogStream::eos;
		return;
	}

	if (res.result != 0)
	{
		ErrorLogStream << "relay server return error(" << res.result << "), session(" << res.gamesessionID << ")." << LogStream::eos;
		return;
	}

	UInt32 dungeonId = dungeonRace->GetDungeonID();
	UInt32 teamId = dungeonRace->GetTeamID();
	UInt64 sessionId = dungeonRace->GetSessionId();
	//UInt32 passedAreaIndex = dungeonRace->GetPassedAreaIndex();
	UInt32 servciceId = dungeonRace->GetServiceId();

	std::vector<DungeonRacePlayerInfo>& playerInfoes = dungeonRace->GetDungeonRacePlayerInfo();

	if (res.gamesessionID != sessionId)
	{
		ErrorLogStream << "start dungeon race fail sessionId is err, race session id(" << sessionId << "), req session id(" << res.gamesessionID << ")" << LogStream::eos;
		return;
	}

	if (dungeonRace->GetStatus() != DRS_CREATING)
	{
		ErrorLogStream << "race state error session(" << res.gamesessionID << "), state(" << dungeonRace->GetStatus() << ")." << LogStream::eos;
		return;
	}

	dungeonRace->SetRelayServerID(res.GetConnID());
	dungeonRace->SetStatus(DRS_RACING);
	UInt32 relayId = dungeonRace->GetRelayServerID();

	CLProtocol::DungenServerTeamStartRaceRes protocol;
	protocol.result = ErrorCode::SUCCESS;
	protocol.isContinue = false;
	protocol.teamId = teamId;
	protocol.dungeonId = dungeonId;
	protocol.battleTempRaceId = dungeonRace->GetBattleTempRaceId();
	if (DSApplication::Instance()->IsCheckRepeatStartDungeon())
	{
		bool isPlayerRepeat = false;
		for (auto& playerInfo : playerInfoes)
		{
			auto roleid = playerInfo.raceInfo.roleId;
			if (DungeonRaceMgr::Instance()->IsPlayerInRace(roleid))
			{
				ErrorLogStream << "player(" << roleid << ") is in race." << LogStream::eos;
				isPlayerRepeat = true;
				break;
			}
		}
		if (isPlayerRepeat)
		{
			protocol.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
			DSRouter::Instance()->SendToService(servciceId, protocol);
			return;
		}
	}

	for (auto& playerInfo : playerInfoes)
	{
		auto roleid = playerInfo.raceInfo.roleId;
		DungeonRaceMgr::Instance()->LeaveDungeonRace(roleid, false);
		DungeonRaceMgr::Instance()->JoinDungeonRace(dungeonRace, playerInfo);
	}

	UInt32 ret = dungeonRace->StartRace(sessionId, res.address, relayId);
	if (ret != ErrorCode::SUCCESS)
	{
		protocol.result = ret;
		DSRouter::Instance()->SendToService(servciceId, protocol);
		DungeonRaceMgr::Instance()->DestoryDungeonRace(dungeonRace);
		return;
	}

	//DSRouter::Instance()->SendToService(servciceId, protocol);
	// ¸üÐÂRelayServer¸ºÔØ
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(res.GetConnID(), res.ability);

#ifdef _USE_DIRECT_DUNGEON_SERVER_
	Protocol::WorldDungeonStartRaceRes res_;
	for (auto playerInfo : playerInfoes)
	{
		SceneSession* sceneSession = SceneSessionMgr::Instance()->FindSceneSession(playerInfo.soureSceneId);
		if (sceneSession)
		{
			res_.roleId = playerInfo.raceInfo.roleId;
			res_.dungeonId = dungeonRace->GetDungeonID();
			res_.isHell = 0;
			res_.isAssist = 0;
			res_.isContinue = 0;
			res_.result = ErrorCode::SUCCESS;
			sceneSession->SendProtocol(res_);
		}
	}
#endif

	return;
}

void HandleProtocol(CLProtocol::MatchSvrDungeonRaceEnd& res)
{
	InfoLogStream << "recv MatchSvrDungeonRaceEnd sessionId : " << res.sessionID << ", reason : " << res.reason << LogStream::eos;
	UInt64 sessionId = res.sessionID;
	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(sessionId);
	if (!race)
	{
		ErrorLogStream << "end race err, not find race session id : " << sessionId << ",  reason : " << res.reason << LogStream::eos;
		return;
	}

	if (race->IsSettled())
	{
		ErrorLogStream << "end race err, race is ended again,  race session id : " << sessionId << ",  reason : " << res.reason << LogStream::eos;
		return;
	}

	race->OnRaceEnd(res.reason, res.raceEndInfo);

}

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg)
{
	InfoLogStream << "recv relay server(" << reg.GetConnID() << ") register msg, isRestart(" << reg.isRestart << LogStream::eos;

	RelaySessionMgr::Instance()->DelRelayServerShutdownChecker(reg.GetConnID());
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(reg.GetConnID(), reg.ability);

	if (reg.isRestart)
	{
		DungeonRaceMgr::Instance()->OnRelayServerShutDown(reg.GetConnID());
	}
}

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol)
{
	DungeonRaceMgr::Instance()->LeaveDungeonRace(protocol.roleId, protocol.isDisconnect != 0);
}

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol)
{
	UInt64 raceId = protocol.param;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceById(raceId);
	if (!dungeonRace)
	{
		return;
	}
	auto dungeonPlayer = dungeonRace->FindDungeonPlayer(protocol.playerId);
	if (!dungeonPlayer)
	{
		return;
	}
	dungeonPlayer->OnCheckItemRet(protocol.serial, protocol.result, protocol.counterTimes);
}

void HandleProtocol(CLProtocol::RelayDungeonBossPhaseChange& protocol)
{
	InfoLogStream << "race session id : " << protocol.raceSessionId << ", bossPhase : " << protocol.bossphase << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceSessionId);
	if (!dungeonRace)
	{
		ErrorLogStream << "not find race session id : " << protocol.raceSessionId << LogStream::eos;
		return;
	}
	dungeonRace->SendToService(protocol);
}

void HandleProtocol(CLProtocol::TeamCopyNotifyRaceDestroyDungeon& protocol)
{
	InfoLogStream << "notifySessionId : " << protocol.notifySessionId << ", destroyDungeonId : " << protocol.destroyDungeonId << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.notifySessionId);
	if (!dungeonRace)
	{
		return;
	}

	CLProtocol::SceneServerSyncPlayerEvent sync;
	sync.raceId = dungeonRace->GetSessionId();
	sync.roleId = 0;
	sync.type = SYNC_DESTORY_DUNGEON;
	sync.param1 = protocol.destroyDungeonId;

	dungeonRace->SendToRelay(sync);
}

void HandleProtocol(CLProtocol::TeamCopyAddRaceReviveCnt& protocol)
{
	InfoLogStream << "raceSessionId : " << protocol.raceSessionId << " addReviveCnt : " << protocol.reviveCnt << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceSessionId);
	if (!dungeonRace)
	{
		return;
	}

	dungeonRace->SetAddTeamReviveCount(protocol.reviveCnt);

	CLProtocol::SceneServerSyncPlayerEvent sync;
	sync.raceId = dungeonRace->GetSessionId();
	sync.roleId = 0;
	sync.type = SYNC_ADD_REVIVE_CNT;
	sync.param1 = protocol.reviveCnt;

	dungeonRace->SendToRelay(sync);
}

void HandleProtocol(CLProtocol::DungeonSvrEndRace& protocol)
{
	InfoLogStream << "raceSessionId : " << protocol.raceSessionId << ", resaon : " << protocol.resaon << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceSessionId);
	if (!dungeonRace)
	{
		InfoLogStream << "not find race raceSessionId : " << protocol.raceSessionId << LogStream::eos;
		return;
	}

	if (dungeonRace->GetStatus() != DRS_RACING)
	{
		InfoLogStream << "race is not in DRS_RACING raceSessionId : " << protocol.raceSessionId << LogStream::eos;
		return;
	}

	CLProtocol::SceneServerSyncPlayerEvent sync;
	sync.raceId = dungeonRace->GetSessionId();
	sync.roleId = 0;
	sync.type = SYNC_END_RACE;
	sync.param1 = protocol.resaon;

	dungeonRace->SendToRelay(sync);
}

void HandleProtocol(CLProtocol::DungeonSvrReviveReq& protocol)
{
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceSessionId);
	if (!dungeonRace)
	{
		return;
	}

	if (dungeonRace->GetStatus() != DRS_RACING)
	{
		return;
	}

	DungeonPlayer * player = dungeonRace->FindDungeonPlayer(protocol.playerId);
	if (!player)
	{
		return;
	}

	CLProtocol::SceneDungeonReviveRes res;
	res.result = dungeonRace->OnPlayerReviveRequest(player, protocol.targetId, protocol.reviveId, protocol.reviveCoinNum, 
		protocol.reviveItem, protocol.reviveItemNum, protocol.reviveHp, protocol.reviveMp);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->sendToClient(res);
	}
}

void HandleProtocol(CLProtocol::SceneToDungeonQuickBuyTransactionReq& req)
{
	CLProtocol::SceneQuickBuyTransactionRes res;
	res.roleId = req.roleId;
	res.transactionId = req.transactionId;

	UInt32 serviceId = req.GetConnID();
	UInt32 sourceSceneId = req.sourceSceneId;

	auto race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(req.roleId);
	if (!race)
	{
		res.result = ErrorCode::QUICK_BUY_REVIVE_NOT_IN_RACE;
		DSRouter::Instance()->SendToScene(serviceId, sourceSceneId, res);
		return;
	}

	DungeonPlayer * player = race->FindDungeonPlayer(req.roleId);
	if (player == NULL)
	{
		res.result = ErrorCode::QUICK_BUY_SYSTEM_ERROR;
		DSRouter::Instance()->SendToScene(serviceId, sourceSceneId, res);
		return;
	}

	if (req.type == QUICK_BUY_REVIVE)
	{
		res.result = race->OnCheckReviveRet(player, (UInt32)req.param2, req.param1, HP_MP_FULL_VALUE, HP_MP_FULL_VALUE);
	}
	else
	{
		res.result = ErrorCode::QUICK_BUY_INVALID_TYPE;
	}

	DSRouter::Instance()->SendToScene(serviceId, sourceSceneId, res);
	return;
}

void HandleProtocol(CLProtocol::VerifyReportTaskResult& protocol)
{
	InfoLogStream << "raceSessionId : " << protocol.raceId << ", roleId : " << protocol.roleId 
		<< ", dungeonId : " << protocol.dungeonId << ", " << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceId);
	if (!dungeonRace)
	{
		InfoLogStream << "not find race raceSessionId : " << protocol.raceId << LogStream::eos;
		return;
	}

	DungeonPlayer * player = dungeonRace->FindDungeonPlayer(protocol.roleId);
	if (!player)
	{
		InfoLogStream << "not find player : " << protocol.roleId << "race id : " << dungeonRace->GetID() << ", raceSessionId : " << protocol.raceId << LogStream::eos;
		return;
	}

	CLProtocol::DungeonFighter fighter;
	if (!dungeonRace->GetFighter(protocol.roleId, fighter))
	{
		InfoLogStream << "not find fighter : " << protocol.roleId << "race id : " << dungeonRace->GetID() << ", raceSessionId : " << protocol.raceId << LogStream::eos;
		return;
	}

	DSRouter::Instance()->SendToWorld(dungeonRace->GetServiceId(), fighter.wsId, protocol);
}

void HandleProtocol(CLProtocol::SceneSyncEvent& sync)
{
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(sync.playerId);
	if (!dungeonRace)
	{
		ErrorLogStream << "scene sync event err not find player id = " << sync.playerId << ", event type = " << (UInt32)sync.type << LogStream::eos;
		return;
	}

	DungeonPlayer * player = dungeonRace->FindDungeonPlayer(sync.playerId);
	if (!player)
	{
		InfoLogStream << "not find player : " << sync.playerId << "race id : " << dungeonRace->GetID() << ", raceSessionId : " << dungeonRace->GetSessionId() << LogStream::eos;
		return;
	}

	player->OnSyncSceneEvent((SyncEventType)sync.type, sync.param1);
}

void HandleProtocol(CLProtocol::TeamCopyNotifyBimsEnergyAccuProgress& protocol)
{
	InfoLogStream << "notify bims energy accu SessionId : " << protocol.raceSessionId << ", progress : " << protocol.progress << LogStream::eos;
	auto dungeonRace = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(protocol.raceSessionId);
	if (!dungeonRace)
	{
		ErrorLogStream << "scene sync event err not find dungeon race raceSessionId = " << protocol.raceSessionId << ", progress = " << protocol.progress 
			<< ", dungeonId = " << protocol.dungeonId << LogStream::eos;
		return;
	}

	CLProtocol::SceneServerSyncPlayerEvent sync;
	sync.raceId = dungeonRace->GetSessionId();
	sync.roleId = 0;
	sync.type = SYNC_TEAMCOPY_BIMS_ENERY_PROGRESS;
	sync.param1 = protocol.progress;
	sync.param2 = protocol.dungeonId;
	dungeonRace->SendToRelay(sync);
}

void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonEnterNextAreaReq& protocol)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to enter next area, but can't find race." << LogStream::eos;
		return;
	}

	DungeonPlayer* dPlayer = dynamic_cast<DungeonPlayer*>(player);
	if (NULL == dPlayer)
	{
		return;
	}

	CLProtocol::SceneDungeonEnterNextAreaRes res;
	res.areaId = protocol.areaId;
	res.result = race->OnPlayerEnterNextArea(dPlayer, protocol.areaId);
	dPlayer->sendToClient(res);
}

void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonRewardReq& protocol)
{
	if (!player)
	{
		return;
	}
	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to get dungeon reward, but can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerPickUpDropItems(dynamic_cast<DungeonPlayer*>(player), protocol.lastFrame, protocol.dropItemIds, protocol.md5);
}

void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonKillMonsterReq& protocol)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ")kill monster, can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerKilledMonster(dynamic_cast<DungeonPlayer*>(player), protocol.monsterIds);
}

void HandleProtocol(CLProtocol::RelayAbalitySync& sync)
{
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(sync.GetConnID(), sync.abality);
}