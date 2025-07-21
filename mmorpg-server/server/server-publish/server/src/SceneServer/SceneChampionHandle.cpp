#include "SceneChampionHandle.h"
#include "Player.h"
#include "SceneChampionMgr.h"
#include <CLErrorCode.h>
#include "PlayerMgr.h"
#include <CLChampionProtocol.h>

void HandleProtocol(CLProtocol::SceneReceiveSyncChampionStatus& sync)
{
	SceneChampionMgr::Instance()->SetStatus(sync.status);
}

void HandleProtocol(CLProtocol::UnionCheckChampionEnrollRes& req)
{
	InfoLogStream << "receive UnionCheckChampionEnrollRes,roleid is " << req.roleId << "result is " << req.errorCode << LogStream::eos;
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == nullptr)
	{
		ErrorLogStream << "can't find player,roleid is " << req.roleId << LogStream::eos;
		return;
	}
	CLProtocol::SceneChampionEnrollRes res;
	res.errorCode = req.errorCode;
	if (req.errorCode == ErrorCode::SUCCESS)
	{
		SceneChampionMgr::Instance()->OnEnrollSuccess(player);
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneChampionEnrollReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_ENROLL, player);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionJoinPrepareReq & req)
{
	//SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_JOIN_RACE, player);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionReliveReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_CHECK_RELIVE, player);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionReliveRes & req)
{
	if (req.errorCode != ErrorCode::SUCCESS)
	{
		player->SetCounter(COUNTER_CHAMPION_RELIVE, 0);
	}
	player->SendProtocol(req);
}

void HandleProtocol(Player * player, CLProtocol::UnionChampionReliveRes & req)
{
	InfoLogStream << "player:" << PLAYERINFO(player) << " receive UnionChampionReliveRes" << LogStream::eos;
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_RELIVE, player, req.errorCode);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampion16TableReq & req)
{
	CLProtocol::SceneChampion16TableRes res;
	res.playerVec = SceneChampionMgr::Instance()->GetTop16Table();
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionGambleReq & req)
{
	SceneChampionMgr::Instance()->OnPlayerBet(player, req);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionObserveReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_OBSERVE, player, req.raceID);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionStatusReq & req)
{
	CLProtocol::SceneChampionStatusRes res;
	res.status = SceneChampionMgr::Instance()->GetStatus();
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionSelfStatusReq & req)
{
	player->SendToUnion(USFT_CHAMPION, req);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionSelfBattleRecordReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_QUERY_SELF_ATTRIBUTES, player, CLProtocol::SCENE_CHAMPION_SELF_BATTLE_RECORD_REQ);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionGroupRankReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_QUERY_SELF_ATTRIBUTES, player, CLProtocol::SCENE_CHAMPION_GROUP_RANK_REQ);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionGroupRecordReq & req)
{
	player->SendToUnion(USFT_CHAMPION, req);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionScoreBattleRecordsReq & req)
{
	SceneChampionMgr::Instance()->BeginAct(ChampionAct::CA_QUERY_SELF_ATTRIBUTES, player, CLProtocol::SCENE_CHAMPION_SCORE_BATTLE_RECORDS_REQ);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionAllGroupReq & req)
{
	CLProtocol::SceneChampionAllGroupRes res;
	auto& groups = SceneChampionMgr::Instance()->GetAllGroupInfo();
	for (auto& groupIter : groups)
	{
		res.groups.push_back(groupIter.second);
	}
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionAllGambleInfoReq & req)
{
	CLProtocol::SceneChampionAllGambleInfoRes res;
	res.infos = SceneChampionMgr::Instance()->GetAllGambleInfo();
	player->SendProtocol(res);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionGambleRecordReq & req)
{
	player->SendToUnion(USFT_CHAMPION, req);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionGmableAlreadyBetReq & req)
{
	player->SendToUnion(USFT_CHAMPION, req);
}

void HandleProtocol(Player * player, CLProtocol::SceneChampionTotalStatusReq & req)
{
	req.accid = player->GetAccID();
	player->SendToUnion(USFT_CHAMPION, req);
}

void HandleProtocol(CLProtocol::UnionChampionJoinPrepareRes & req)
{
	InfoLogStream << "receive UnionChampionJoinPrepareRes,role id is " << req.roleId << " result is " << req.errorCode << LogStream::eos;
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (player == nullptr)
	{
		ErrorLogStream << "can't find player,roleid is " << req.roleId << LogStream::eos;
		return;
	}
	if (req.errorCode == ErrorCode::SUCCESS)
	{
		SceneChampionMgr::Instance()->OnPlayerJoinRaceSuccess(player, req.dstSceneId, req.dstDoorId);
	}
	else
	{
		SceneChampionMgr::Instance()->OnPlayerJoinRaceFail(player, req.dstSceneId, req.dstDoorId);
	}

	//CLProtocol::SceneChampionJoinPrepareRes res;
	//res.errorCode = req.errorCode;
	//player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneChampion16TableSync& sync)
{
	SceneChampionMgr::Instance()->OnUnionSyncTop16Table(sync.playerVec);
}

void HandleProtocol(CLProtocol::SceneChampionGambleInfoSync& sync)
{
	SceneChampionMgr::Instance()->OnUnionSyncGambleInfo(sync.infoVec);
}

void HandleProtocol(CLProtocol::SceneChampionAllGambleInfoRes& sync)
{
	SceneChampionMgr::Instance()->OnUnionSyncGambleInfo(sync.infos);
}

void HandleProtocol(CLProtocol::SceneChampionGambleUnionRes& req)
{
	SceneChampionMgr::Instance()->OnPlayerBetRet(req);
}

void HandleProtocol(CLProtocol::UnionChampionPlayerStatusRes & req)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleID);
	if (player == nullptr)
	{
		return;
	}
}

void HandleProtocol(CLProtocol::SceneChampionAllGroupRes & req)
{
	SceneChampionMgr::Instance()->OnUnionSyncAllGroupInfo(req);
}

void HandleProtocol(CLProtocol::SceneChampionGroupStatusSync & sync)
{
	SceneChampionMgr::Instance()->OnUnionSyncOneGroupInfo(sync);
}

void SceneChampionAllRaceInfoSyncFunction::operator()(Avalon::Packet * packet) const
{
	SceneChampionMgr::Instance()->OnUnionSyncBattleRecord(packet);
}


