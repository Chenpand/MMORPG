#include "USChampionHandler.h"

#include <CLErrorCode.h>

#include "UnionServiceMgr.h"
#include "GameZoneMgr.h"
#include "ChampionService.h"
#include "USRouter.h"
#include "USGamble.h"
#include "ChampionRaceMgr.h"
#include "ChampionPlayer.h"



void HandleProtocol(CLProtocol::SceneChampionReliveReq& sync, USPlayer* player)
{
	InfoLogStream << "receive SceneChampionReliveReq " << PLAYERINFO(player) << LogStream::eos;
	auto zone = GameZoneMgr::Instance()->FindGameZone(sync.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	CLProtocol::SceneChampionReliveRes res;
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		res.errorCode = ErrorCode::CHAMPION_NO_INFO;
		player->SendToScenePlayer(res);
		return;
	}
	res.errorCode = service->BeginAct(CA_RELIVE, player->GetAccID(), player->GetID(), nullptr);
	InfoLogStream << "CA_RELIVE result is " << res.errorCode << " " << PLAYERINFO(player) << LogStream::eos;
	player->SendToScenePlayer(res);
}

void HandleProtocol(CLProtocol::UnionChampionReliveReq & req, USPlayer * player)
{
	InfoLogStream << "receive UnionChampionReliveReq " << PLAYERINFO(player) << LogStream::eos;
	auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	CLProtocol::UnionChampionReliveRes res;
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		res.errorCode = ErrorCode::CHAMPION_NO_INFO;
		player->SendToScenePlayer(res);
		return;
	}
	res.errorCode = service->BeginAct(CA_CHECK_RELIVE, player->GetAccID(), player->GetID(), nullptr);
	InfoLogStream << "CA_CHECK_RELIVE result is " << res.errorCode << " " << PLAYERINFO(player) << LogStream::eos;
	player->SendToScenePlayer(res);
}

void HandleProtocol(CLProtocol::UnionChampionPlayerStatusReq & req, USPlayer * player)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == nullptr)
	{
		return;
	}

	auto newPlayer = static_cast<ChampionPlayer*>(player);

	if (req.param == CLProtocol::SCENE_CHAMPION_SELF_BATTLE_RECORD_REQ)
	{
		CLProtocol::SceneChampionSelfBattleRecordRes res;
		res.win = newPlayer->GetWinCount();
		res.lose = newPlayer->GetLoseCount();
		newPlayer->SendProtocol(res);
	}
	else if (req.param == CLProtocol::SCENE_CHAMPION_GROUP_RANK_REQ)
	{
		CLProtocol::SceneChampionGroupRankRes res;
		if (!IsLogicSameStatus(newPlayer->GetStatus(), service->GetStatus()))
		{
			newPlayer->SendProtocol(res);
			return;
		}
		auto& list  = service->OnPlayerGetGroupScoreRank(newPlayer);
		for (auto& i : list)
		{
			res.rankList.push_back(i->GetRankRole());
		}
		newPlayer->SendProtocol(res);
	}
	else if (req.param == CLProtocol::SCENE_CHAMPION_SCORE_BATTLE_RECORDS_REQ)
	{
		CLProtocol::SceneChampionScoreBattleRecordsRes res;
		res.records = service->OnPlayerGetGroupScoreRecord(newPlayer);
		newPlayer->SendProtocol(res);
	}
	else
	{
		return;
	}

}

void HandleProtocol(CLProtocol::UnionCheckChampionEnrollReq& req)
{
	InfoLogStream << "UnionCheckChampionEnrollReq received, roleid is " << req.roleId << LogStream::eos;
	CLProtocol::UnionCheckChampionEnrollRes res;
	res.errorCode = ErrorCode::CHAMPION_NO_INFO;
	res.roleId = req.roleId;
	auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		zone->TransmitToScene(res);
		return;
	}
	res.errorCode = service->BeginAct(CA_ENROLL, req.accid, req.roleId);
	InfoLogStream << "CA_ENROLL result is " << res.errorCode << " " << req.roleId << LogStream::eos;
	zone->TransmitToScene(res);
}

void HandleProtocol(CLProtocol::UnionChampionJoinPrepareReq& req)
{
	InfoLogStream << "UnionChampionJoinPrepareReq received, roleid is " << req.roleId << LogStream::eos;
	CLProtocol::UnionChampionJoinPrepareRes res;
	res.errorCode = ErrorCode::CHAMPION_NO_INFO;
	res.roleId = req.roleId;
	auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		zone->TransmitToScene(res);
		return;
	}
	JoinRaceActInfo info;
	info.msg = &req;
	res.errorCode = service->BeginAct(CA_JOIN_RACE, req.accId, req.roleId, &info);
	InfoLogStream << "CA_JOIN_RACE result is " << res.errorCode << " " << req.roleId << LogStream::eos;
	res.dstSceneId = req.dstSceneId;
	res.dstDoorId = req.dstDoorId;
	zone->TransmitToScene(res);
}


void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoReq& req)
{
	
	CLProtocol::MatchServerQueryPlayerInfoRes res;
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		res.sessionId = req.msg.sessionId;
		res.roleId = req.msg.roleId;
		res.result = MATCH_QUERY_PLAYER_OFFLINE;
		req.SendBack(res);
		return;
	}

	res.result = service->OnMatchQueryPlayerInfo(req);
	if (res.result != ErrorCode::SUCCESS)
	{
		req.SendBack(res);
	}
}


void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoRes& req)
{
	USRouter::Instance()->SendToMatch(req.msg);
}

void HandleProtocol(CLProtocol::MatchServerChampionBattleRes& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	service->OnMatchBattleRes(req);
}

void HandleProtocol(CLProtocol::MatchServerChampionRaceEnd& req)
{
	InfoLogStream << "receive race end, req(" << req.groupID << "," << req.raceId << "," << req.order << "," << req.result << ","<< req.winner.id << "," << req.loser.id << ")" << LogStream::eos;
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{	
		ErrorLogStream << "receive race end, but service not found,req(" << req.groupID << "," << req.raceId << "," << req.order << "," << req.result << ")" << LogStream::eos;
		return;
	}
	service->OnMatchRaceEnd(req);

}

void HandleProtocol(CLProtocol::MatchServerRaceEnd& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	service->OnMatchOnPVPRes(req);
	
}


void HandleProtocol(CLProtocol::WorldNotifyRaceStart& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	service->OnNotifyRaceStart(req);
}

void HandleProtocol(CLProtocol::SceneChampionGambleReq& req)
{
	InfoLogStream << "receive SceneChampionGambleReq, betinfo is " << req.id << "|" << req.option << "|" << req.coin << LogStream::eos;
	CLProtocol::SceneChampionGambleUnionRes unionRes;
	unionRes.res.errorCode = USGambleMgr::Instance()->OnPlayerBet(CL_GET_ZONEID_FROM_NODEID(req.GetConnID()), req.roleID, req.accid, req.id, req.option, req.coin);
	InfoLogStream << "OnPlayerBet result is " << unionRes.res.errorCode << " betinfo is " << req.id << "|" << req.option << "|" << req.coin << LogStream::eos;
	unionRes.res.id = req.id;
	unionRes.res.option = req.option;
	unionRes.res.num = req.coin;
	unionRes.roleID = req.roleID;
	auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == nullptr)
	{
		return;
	}
	zone->TransmitToScene(unionRes);
	if (unionRes.res.errorCode == ErrorCode::SUCCESS)
	{
		auto msg = USGambleMgr::Instance()->GetSyncMsg();
		zone->SendToPlayer(req.roleID, msg);
	}
}

void HandleProtocol(CLProtocol::UnionChampionObserveReq& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	ObserveActInfo info;
	info.connID = req.GetConnID();
	info.msg = &req;
	auto errorCode =  service->BeginAct(CA_OBSERVE, 0, req.roleID, &info);
	
	if (errorCode != ErrorCode::SUCCESS)
	{
		auto zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
		if (zone == nullptr)
		{
			return;
		}
		CLProtocol::SceneChampionObserveRes res;
		res.raceID = req.raceID;
		res.addr.SetIP("");
		res.addr.SetPort(0);
		zone->SendToPlayer(req.roleID, req);
	}
}

void HandleProtocol(CLProtocol::SceneChampionObserveRes& req)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(service->GetConnID(req.roleID));
	if (zone == nullptr)
	{
		return;
	}
	zone->SendToPlayer(req.roleID, req);
}


void HandleProtocol(CLProtocol::VerifyReportTaskResult& req)
{
	InfoLogStream << "receive verify report " << req.raceId << "," << req.roleId << "," << req.result << LogStream::eos;
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		ErrorLogStream << "receive verify report but can't find service " << req.raceId << "," << req.roleId << "," << req.result << LogStream::eos;
		return;
	}
	auto player = service->GetPlayer(req.roleId);
	if (player == nullptr)
	{
		ErrorLogStream << "receive verify report but can't find player " << req.raceId << "," << req.roleId << "," << req.result << LogStream::eos;
		return;
	}
	
	player->SendToWorld(req);
}

void HandleProtocol(CLProtocol::SceneChampionSelfStatusReq & req, RoleIDConnID& info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
	if (zone == nullptr)
	{
		return;
	}

	CLProtocol::SceneChampionSelfStatusRes res;
	res.status = CS_NULL;
	auto geter = [&](ChampionPlayer* player)->void {
		res.status = player->GetStatus();
	};
	service->GetPlayerAttributes(info.roleID, geter);
	zone->SendToPlayer(info.roleID, res);
}

void HandleProtocol(CLProtocol::SceneChampionTotalStatusReq & req, RoleIDConnID& info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
	if (zone == nullptr)
	{
		return;
	}

	CLProtocol::SceneChampionTotalStatusRes res;
	res.status = service->GetStatusInfo();
	res.slefStatus = CS_NULL;
	res.roleID = 0;
	if (service->IsEnroll(req.accid))
	{
		res.roleID = 1;
	}
	auto geter = [&](ChampionPlayer* player)->void {
		res.slefStatus = player->GetStatus();
		res.roleID = player->GetID();
	};
	service->GetPlayerAttributes(info.roleID, geter);
	zone->SendToPlayer(info.roleID, res);
}

void HandleProtocol(CLProtocol::SceneChampionGroupRecordReq & req, RoleIDConnID & info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
	if (zone == nullptr)
	{
		return;
	}
	auto races = ChampionRaceMgr::Instance()->FindRaces(req.groupID);
	if (races.empty())
	{
		return;
	}
	CLProtocol::SceneChampionGroupRecordRes res;
	for (auto& race : races)
	{
		ChampionBattleRecord record;
		record.order = race.first;
		record.raceID = race.second->GetRaceID();
		record.winner = race.second->GetWinnerID();
		record.isEnd = race.second->IsEnd() ? 1 : 0;
		res.records.push_back(record);
	}
	res.groupID = req.groupID;
	zone->SendToPlayer(info.roleID, res);
}

void HandleProtocol(CLProtocol::SceneChampionGambleRecordReq & req, RoleIDConnID & info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		ErrorLogStream << "player(" << info.roleID << ") request gamble record,but can't find service"  << LogStream::eos;
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
	if (zone == nullptr)
	{
		ErrorLogStream << "player(" << info.roleID << ") request gamble record,but can't find zone: " << info.connId << LogStream::eos;
		return;
	}

	CLProtocol::SceneChampionGambleRecordRes res;

	auto recordVec = USGambleMgr::Instance()->GetRecord(info.roleID);
	for (auto& record : recordVec)
	{
		auto project = USGambleMgr::Instance()->GetProject(record->GetGmableID());
		ChampionGambleRecord msgRecord;
		msgRecord.id = record->GetGmableID();
		msgRecord.option = record->GetOption();
		if (project != nullptr)
		{
			msgRecord.result = project->GetWinOption();
		}
		msgRecord.reward = record->GetReward();
		msgRecord.time = record->GetTime();
		msgRecord.num = record->GetNum();
		res.infos.push_back(msgRecord);
	}

	zone->SendToPlayer(info.roleID, res);
}

void HandleProtocol(CLProtocol::SceneChampionGmableAlreadyBetReq & req, RoleIDConnID & info)
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		ErrorLogStream << "player(" << info.roleID << ") request already gamble num,but can't find service" << LogStream::eos;
		return;
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
	if (zone == nullptr)
	{
		ErrorLogStream << "player(" << info.roleID << ") request already gamble num,but can't find zone: " << info.connId << LogStream::eos;
		return;
	}


	CLProtocol::SceneChampionGmableAlreadyBetRes res;
	res.id = req.id;
	res.option = req.option;
	res.num = 0;
	auto recordVec = USGambleMgr::Instance()->GetRecord(info.roleID);
	for (auto& record : recordVec)
	{
		if (record->GetGmableID() == req.id && record->GetOption() == req.option)
		{
			res.num = record->GetNum();
			break;
		}
	}

	zone->SendToPlayer(info.roleID, res);
}

