#include "MatchHandler.h"
#include <CLErrorCode.h>
#include "MatchMgr.h"
#include "RelaySessionMgr.h"
#include "MSRouter.h"
#include "MatchServiceMgr.h"
#include "MSInternalNetwork.h"
#include "RobotMgr.h"

struct MSTeamInfo
{
    UInt32  wsID;
    UInt32  teamId;
    UInt32  dungeonId;
};

HashMap<UInt64, MSTeamInfo> g_teamSessionMgr;

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol)
{
    Avalon::NetConnectionPtr node = MSInternalNetwork::Instance()->FindConnection(protocol.nodeId);
    if (node == NULL)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
		if (packetId == CLProtocol::VERIFY_TASK_RESULT)
		{
			MSRouter::Instance()->SendToUnion(protocol.packet);
		}
        ErrorLogStream << "can not find connection(" << protocol.nodeId << ") when transmit race packet("
            << packetId << ")." << LogStream::eos;
        return;
    }

	node->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol)
{
    auto player = MatchServiceMgr::Instance()->FindPlayer(protocol.roleId);
    if (!player)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
        ErrorLogStream << "can not find player(" << protocol.roleId << ") when transmit race packet("
            << packetId << ")." << LogStream::eos;
    }

    MSRouter::Instance()->SendToWorld(player->GetWsID(), protocol);
}

void HandleProtocol(CLProtocol::MatchServerMatchStartReq &req)
{
    InfoLogStream << "player(" << req.info.id << ") request to start match" << LogStream::eos;

#ifndef _NEW_MATCH_
	// 创建匹配玩家
	MatchPlayer* matchPlayer = new MatchPlayer();
    matchPlayer->SetServerID(req.GetConnID());
	matchPlayer->SetID(req.id);
	matchPlayer->SetAccId(req.accid);
	matchPlayer->SetMatchScore(req.matchScore);
	matchPlayer->SetStartTime(Avalon::Time::CurrentTime());

    CLProtocol::WorldMatchStartRes res;
    res.result = MatchMgr::Instance()->AddMatchPlayer(matchPlayer);
    MSRouter::Instance()->SendToPlayer(req.serverid, req.id, res);

    if (res.result)
    {
        delete matchPlayer;
    }
#else
	CLProtocol::MatchServerMatchStartRes res;
	res.roleid = req.info.id;
	res.type = req.type;
	res.result = MatchServiceMgr::Instance()->OnPlayerStartPvPMatch(req.GetConnID(), req);
	MSRouter::Instance()->SendToWorld(req.GetConnID(), res);
#endif
}

void HandleProtocol(CLProtocol::MatchServerMatchDungeonReq& req)
{
	MatchServiceMgr::Instance()->OnPlayerStartDungeonMatch(req.GetConnID(), 0, req.dungeonId, req.isHell, req.fighters, req.level);
}

void HandleProtocol(CLProtocol::MatchServerGuildBattleMatchReq &req)
{
	CLProtocol::WorldMatchStartRes res;
	res.result = MatchServiceMgr::Instance()->OnPlayerStartGuildBattleMatch(req.GetConnID(), req);
	MSRouter::Instance()->SendToPlayer(req.GetConnID(), req.info.id, res);
}

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueMatchReq &req)
{
	CLProtocol::WorldMatchStartRes res;
	res.result = MatchServiceMgr::Instance()->OnPlayerStartPremiumLeagueMatch(req.GetConnID(), req);
	MSRouter::Instance()->SendToPlayer(req.GetConnID(), req.info.id, res);
}

void HandleProtocol(CLProtocol::MatchServerMatchCancelReq &req)
{
    InfoLogStream << "player(" << req.id << ") request to cancel match" << LogStream::eos;

#ifndef _NEW_MATCH_
    CLProtocol::WorldMatchCancelRes res;
	res.result = MatchMgr::Instance()->OnCancelMatch(req.id);
    MSRouter::Instance()->SendToPlayer(req.GetConnID(), req.id, res);
#else
    CLProtocol::WorldMatchCancelRes res;
    res.result = MatchServiceMgr::Instance()->OnPlayerCancelMatch(req.id);
	if (req.needReturn > 0 || res.result == ErrorCode::SUCCESS)
	{
		// 这里做个临时调整
		// 当不在匹配状态但是玩家想取消匹配，强制把错误码改成匹配成功
		if (req.needReturn == 1 && res.result == ErrorCode::MATCH_CANCLE_NOT_MATCHING)
		{
			ErrorLogStream << "[MatchBug] player(" << req.id << ") invalid status, player is not in matching..." << LogStream::eos;
			res.result = ErrorCode::SUCCESS;
		}

		MSRouter::Instance()->SendToWorldPlayer(req.GetConnID(), req.id, res);
	}
#endif
}

void HandleProtocol(CLProtocol::MatchServerPkRoomReq &req)
{
	CLProtocol::MatchServerPkRoomRes res;
	res.roomId = req.roomId;
	for (UInt32 i = 0; i < req.players.size(); ++i)
	{
		res.playerIds.push_back(req.players[i].id);
	}
	res.result = MatchServiceMgr::Instance()->OnPlayerStartPkRoom(req.GetConnID(), req.roomId, req.players, req.pkType);
	MSRouter::Instance()->SendToWorld(req.GetConnID(), res);
}

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& res)
{
#ifndef _NEW_MATCH_
    MatchMgr::Instance()->OnQueryPlayerInfoRes(res.sessionId, res.roleId, res.result, res.info);
#else
	if (res.type == MATCH_QUERY_RACE_INFO || res.type == MATCH_QUERY_GUILD_RACE_INFO || res.type == MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO || res.type == MATCH_QUERY_EQUAL_RACE_INFO)
	{
		MatchServiceMgr::Instance()->OnQueryPlayerRaceInfoRes(res.sessionId, res.roleId, res.result, res.racePlayerInfo);
	}
	else
	{
		MatchServiceMgr::Instance()->OnQueryPlayerDungeonInfoRes(res.sessionId, res.roleId, res.result, res.dungeonPlayerInfo);
	}
#endif
}

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg)
{
    InfoLogStream << "recv relay server(" << reg.GetConnID() << ") register msg, isRestart(" << reg.isRestart << LogStream::eos;

    RelaySessionMgr::Instance()->DelRelayServerShutdownChecker(reg.GetConnID());
    RelaySessionMgr::Instance()->UpdateRelayServerAbality(reg.GetConnID(), reg.ability);

    if (reg.isRestart)
    {
        MatchMgr::Instance()->OnRelayServerShutDown(reg.GetConnID());
    }
}

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& res)
{
    auto itr = g_teamSessionMgr.find(res.gamesessionID);
    if (itr != g_teamSessionMgr.end())
    {
        auto teamInfo = itr->second;

        CLProtocol::WorldServerTeamCreateRaceRes teamRes;
        teamRes.teamId = teamInfo.teamId;
        teamRes.dungeonId = teamInfo.dungeonId;
        teamRes.relayServerID = res.GetConnID();
        teamRes.result = res.result;
        teamRes.gamesessionID = res.gamesessionID;
        teamRes.address = res.address;
        MSRouter::Instance()->SendToWorld(teamInfo.wsID, teamRes);

        g_teamSessionMgr.erase(itr);
        return;
    }

#ifndef _NEW_MATCH_
	MatchMgr::Instance()->OnRelayServerStartResponse(res.result, res.gamesessionID, res.ability, res.address);
#else
    MatchServiceMgr::Instance()->OnRelayServerStartResponse(res.result, res.gamesessionID, res.ability, res.address);
#endif
}

void HandleProtocol(CLProtocol::MatchSvrEndGameReq& res)
{
#ifndef _NEW_MATCH_
    MatchMgr::Instance()->OnRaceEnd(res.gamesessionID, (EGameSessionDismissType)res.endReason, res.results);
#else
	MatchServiceMgr::Instance()->OnPvPRaceEnd(res.gamesessionID, (EGameSessionDismissType)res.endReason, res.raceEndInfo);
#endif
}

void HandleProtocol(CLProtocol::MatchSvrDungeonRaceEnd& req)
{
    MatchServiceMgr::Instance()->OnDungeonRaceEnd(req.sessionID, (EGameSessionDismissType)req.reason, req.raceEndInfo);
}

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceReq& req)
{
#ifndef _NEW_MATCH_
    RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
    if (!relaySession)
    {
        return;
    }

    MSTeamInfo teamInfo;
    teamInfo.wsID = req.GetConnID();
    teamInfo.teamId = req.teamId;
    teamInfo.dungeonId = req.dungeonId;
    UInt64 session = MatchMgr::GenGameSession();
    g_teamSessionMgr[session] = teamInfo;

    CLProtocol::MatchSvrStartGameReq startReq;
    startReq.raceType = RT_DUNGEON;
    startReq.gamesessionID = session;
    startReq.gamesessionFighter = req.fighters;
    
    relaySession->SendProtocol(startReq);
#else
    MatchServiceMgr::Instance()->OnPlayerStartDungeonTeamRace(req.GetConnID(), req.teamId, req.cityMonster, req.dungeonId, req.isHell, req.fighters);
#endif
}

void HandleProtocol(CLProtocol::WorldMatchPracticeReq& req)
{
	MatchServiceMgr::Instance()->OnPlayerStartPracticeMatch(req.GetConnID(), req.roleIds, req.pkType);
}

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueBattleReq& req)
{
	MatchServiceMgr::Instance()->OnPlayerStartPremiumLeagueBattle(req.GetConnID(), req.battleId, req.roleIds);
}

void HandleProtocol(CLProtocol::SceneServerGiveupReconn& protocol)
{
	MSRouter::Instance()->BroadcastToRelay(protocol);
}

void HandleProtocol(CLProtocol::MatchServerSpecifyPkRobot& req)
{
	RobotMgr::Instance()->SetSepcifyRobotOccu(req.occu);
	RobotMgr::Instance()->SetSepcifyRobotHard((RobotAttrHardType)req.hard);
	RobotMgr::Instance()->SetSepcifyRobotAI((RobotAIHardType)req.ai);

	InfoLogStream << "specify robot(" << req.occu << ", " << req.hard << ", " << req.ai << ")" << LogStream::eos;
}

void HandleProtocol(CLProtocol::MatchServerChampionBattleReq & req)
{
	MatchServiceMgr::Instance()->OnPlayerStartChampionBattle(req.GetConnID(), req.groupID, req.order, req.roleIds);
}

void HandleProtocol(CLProtocol::RelayAbalitySync& sync)
{
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(sync.GetConnID(), sync.abality);
}