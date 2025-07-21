#include "SceneMatchHandler.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "TeamMgr.h"
#include "SSServiceSwitchMgr.h"
#include <CLSortListProtocol.h>
#include "SeasonMgr.h"

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol)
{
    /*auto player = PlayerMgr::Instance()->FindPlayer(protocol.roleId);
    if (!player)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
        ErrorLogStream << "can not find player(" << protocol.roleId << ") when recv race packet("
            << packetId << ")." << LogStream::eos;
        return;
    }*/

    PlayerMgr::Instance()->ExecutePacket(protocol.roleId, protocol.packet);
}

void HandleProtocol(Player* player, CLProtocol::SceneMatchStartReq &req)
{
    if (!player)
    {
        return;
    }

	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), req);
		return;
	}
	//req.type = PK_PREMIUM_LEAGUE_PRELIMINAY;
	/*
	//没有赛季未开始的状态
	if (req.type == PK_SEASON_1V1 && !SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_1V1_SEASON))
	{
		// 发送提示，ToDo...
		return;
	}
	*/

	UInt32 ret = player->CanStartPkMatch((PkType)req.type);
	if (ret != ErrorCode::SUCCESS)
	{
		CLProtocol::WorldMatchStartRes res;
		res.result = ret;
		player->SendProtocol(res);
		return;
	}

    CLProtocol::MatchServerMatchStartReq msReq;
    msReq.type = req.type;
    msReq.info.id = player->GetID();
	msReq.info.accid = player->GetAccID();
	msReq.dailyCount = 0;
	msReq.seasonLevel = player->GetSeasonLevel();
	msReq.level = player->GetLevel();
    if (msReq.type == PK_NORMAL_1V1)
    {
        msReq.matchScore = player->GetPkValue();
    }
    else if (msReq.type == PK_SEASON_1V1 || msReq.type == PK_WUDAODAHUI)
    {
        msReq.matchScore = player->GetMatchScore();
		msReq.dailyCount = player->GetCount(COUNTER_DAY_SEASON_PK);
    }
	else if (msReq.type == PK_GUILD_BATTLE || msReq.type == PK_PREMIUM_LEAGUE_PRELIMINAY)
	{
		// 转发到world
		Router::SendToWorld(msReq);
		return;
	}
	else if (msReq.type == PK_3V3_ACTIVITY)
	{
		CLProtocol::MatchServerPkRoomReq req;
		req.pkType = (UInt8)PK_3V3_ACTIVITY;
		req.roomId = 0;
		PkRoomMatchPlayerInfo info;
		info.wsId = CL_MAKE_NODEID(ZONE_ID, ST_WORLD, 1);
		info.accid = player->GetAccID();
		info.id = player->GetID();
		info.level = player->GetLevel();
		info.loseStreak = player->GetSeasonPkLoseStraight();
		info.winStreak = player->GetSeasonPkWinStraight();
		info.name = player->GetName();
		info.score = player->GetMatchScore();
		info.seasonLevel = player->GetSeasonLevel();
		info.seasonStar = player->GetSeasonStar();
		info.seasonExp = player->GetSeasonExp();
		info.seat = 0;
		req.players.push_back(info);
		Router::SendToWorld(req);
		return;
	}
    else
    {
        ErrorLogStream << "player(" << player->GetID() << ") want start invalid match(" << req.type << ")." << LogStream::eos;
        return;
    }
    
	PkType type = (PkType)msReq.type;
	if (type == PK_WUDAODAHUI)
	{
		type = PK_SEASON_1V1;
	}
	auto statistic = player->GetPkStatisticMgr().FindPkStatistic(type);
	msReq.fightCount = statistic ? statistic->GetTotalNum() : 0;
	msReq.winStreak = statistic ? statistic->GetCurWinStreak() : 0;
	msReq.loseStreak = statistic ? statistic->GetCurLoseStreak() : 0;

	if (type == PK_SEASON_1V1)
	{
		msReq.winStreak = player->GetSeasonPkWinStraight();
		msReq.loseStreak = player->GetSeasonPkLoseStraight();
	}

    Router::SendToWorld(msReq);

	player->SendMatchStartUdpLog((PkType)req.type);

}

void HandleProtocol(Player* player, CLProtocol::SceneMatchCancelReq &req)
{
    if (!player)
    {
        return;
    }

	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), req);
		return;
	}

    CLProtocol::MatchServerMatchCancelReq msReq;
    msReq.id = player->GetID();
	msReq.needReturn = 1;
    Router::SendToWorld(msReq);
}

void HandleProtocol(Player* player, CLProtocol::SceneWudaoJoinReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneWudaoJoinRes res;
	res.result = player->JoinWudaodahui();
	player->SendProtocol(res);
}

void HandleProtocol(Player* player, CLProtocol::SceneWudaoRewardReq& req)
{
	if (!player)
	{
		return;
	}

	player->ClearRewardGroup();

	CLProtocol::SceneWudaoRewardRes res;
	res.result = player->RequestWudaodahuiReward();
	if (res.result == ErrorCode::SUCCESS)
	{
		res.getItems = player->GetRewardGroupNoClear()->GetRewards();
	}
	
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoReq& req)
{
    CLProtocol::MatchServerQueryPlayerInfoRes res;
    res.sessionId = req.sessionId;

    Player* player = PlayerMgr::Instance()->FindPlayer(req.roleId);
    if (!player)
    {
        res.result = MATCH_QUERY_PLAYER_OFFLINE;
    }
    else
    {
        res.roleId = player->GetID();
        res.result = MATCH_QUERY_SUCCESS;
		res.type = req.type;
		if (req.type == MATCH_QUERY_RACE_INFO || req.type == MATCH_QUERY_GUILD_RACE_INFO || req.type == MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO)
		{
			res.racePlayerInfo = player->GetMatchRacePlayerInfo(true);
		}
		else
		{
			res.dungeonPlayerInfo = player->GetDungeonMgr().GetDungeonPlayerInfo(req.dungeonId);
		}
    }

	req.SendBack(res);
    //Router::SendToWorld(res);
}

void HandleProtocol(CLProtocol::MatchServerRaceEnd& req)
{
    Player* player = PlayerMgr::Instance()->FindPlayer(req.result.roleId);
    if (!player)
    {
        ErrorLogStream << "player(" << req.result.roleId << ") race end, but can't find player object." << LogStream::eos;
        return;
    }

	player->OnPVP(req);
}

void HandleProtocol(CLProtocol::MatchServerDungeonRaceEnd& req)
{
    auto team = TeamMgr::Instance()->FindTeam(req.teamId);
    if (!team)
    {
        ErrorLogStream << "recv dungeon race(" << req.endInfo.gamesessionId << ")'s end info, but can't find team(" << req.teamId << ")." << LogStream::eos;
        return;
    }

    //team->OnDungeonRaceEnd(req.endInfo);
}

void HandleProtocol(CLProtocol::WorldChangeMatchScore& req)
{
	DebugLogStream << "match reset score, role(" << req.roleId << ") new score(" << req.matchScore << ")." << LogStream::eos;

	auto player = PlayerMgr::Instance()->FindPlayer(req.roleId);
	if (player)
	{
		player->SetMatchScore(req.matchScore);
	}
	else
	{
		CLUpdateCommand	*cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", req.roleId, true);
		cmd->PutData("match_score", req.matchScore);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void HandleProtocol(CLProtocol::SceneNotifySeason& req)
{
	DebugLogStream << "world notify season(" << req.seasonId << ", " << req.startTime << ", " << req.endTime << ", " << req.seasonAttrStartTime << ", " << req.seasonAttrEndTime << ") ." << LogStream::eos;

	bool isUpdate = false;

	if (SeasonMgr::Instance()->GetSeasonId() != req.seasonId)
	{
		isUpdate = true;
	}
	SeasonMgr::Instance()->SetSeasonId(req.seasonId);
	SeasonMgr::Instance()->SetEndTime(req.endTime);
	SeasonMgr::Instance()->SetStartTime(req.startTime);
	if (SeasonMgr::Instance()->ChangeSeasonAttr(req.seasonAttrStartTime, req.seasonAttrEndTime, req.seasonSortListPlayerIds))
	{
		isUpdate = true;
	}
	if (isUpdate)
	{
		SeasonMgr::Instance()->UpdateSeasonInfo();
	}
}

void HandleProtocol(Player *player, CLProtocol::SceneSeasonPlayStatus &protocol)
{
	UInt8 status = player->GetSeasonPlayStatus();

	InfoLogStream << "In HandleProtocol(Player *player, CLProtocol::SceneSeasonPlayStatus &protocol), player(role: " << player->GetID()
		<< ",accId:" << player->GetAccID() << "), protocol(id:" << protocol.GetProtocolID() << "), level(" << player->GetLevel()
		<< "), status(" << status << ")" << LogStream::eos;

	player->ResetSeasonPlayStatus();
}

