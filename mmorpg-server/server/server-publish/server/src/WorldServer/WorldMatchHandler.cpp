#include "WorldMatchHandler.h"
#include "WSPlayer.h"
#include "RelaySessionMgr.h"
#include "WSRouter.h"
#include "WSPlayerMgr.h"
#include "WSTeamMgr.h"
#include "WSNetwork.h"
#include "DungeonRaceMgr.h"
#include "GuildBattleMgr.h"
#include "GuildMgr.h"
#include "PremiumLeagueMgr.h"
#include "GameParamMgr.h"
#include "CLRoomProtocol.h"
#include "WSApplication.h"

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol)
{
    // 如果目标是RelayServer，就转发给MatchServer
    if (CL_GET_SERVERTYPE_FROM_NODEID(protocol.nodeId) == ST_RELAY)
    {
        Router::SendToMatch(protocol);
        return;
    }
    else if (CL_GET_SERVERTYPE_FROM_NODEID(protocol.nodeId) == ST_MATCHSERVER)
    {
        Router::SendToMatch(protocol.packet);
        return;
    }

    Avalon::NetConnectionPtr node = WSNetwork::Instance()->FindConnection(protocol.nodeId);
    if (node == NULL)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
        ErrorLogStream << "can not find connection(" << protocol.nodeId << ") when transmit race packet(" 
                        << packetId << ")." << LogStream::eos;
        return;
    }

    CL_SEND_PROTOCOL(node, protocol);
}

void HandleProtocol(CLProtocol::SysTransmitRaceProtocolToScenePlayer& protocol)
{
    auto player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
    if (!player)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
        ErrorLogStream << "can not find player(" << protocol.roleId << ") when transmit race packet("
            << packetId << ")." << LogStream::eos;
        return;
    }

    player->SendToScene(protocol);
}

void HandleProtocol(CLProtocol::MatchServerMatchStartReq &req)
{
	req.info.serverCreateTime = GameParamMgr::Instance()->GetValue(PARAM_NAME_GAME_FIRST_START);
	if (req.type == PK_GUILD_BATTLE)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(req.info.id);
		UInt32 nRes = GuildBattleMgr::Instance()->OnMatchBattleStart(player);
		if (nRes != ErrorCode::SUCCESS && nRes != ErrorCode::GUILD_BATTLE_MATCH_TRANSACTION)
		{
			CLProtocol::WorldMatchStartRes res;
			res.result = nRes;
			player->SendProtocol(res);
		}
		ErrorLogStream << "guild battle match start(" << req.info.id << ") errorcode(" << nRes << ") " << LogStream::eos;
		return;
	}
	else if (req.type == PK_PREMIUM_LEAGUE_PRELIMINAY)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(req.info.id);
		if (!player)
		{
			return;
		}

		UInt32 nRes = PremiumLeagueMgr::Instance()->OnPlayerStartPreliminay(player);
		
		if (nRes != ErrorCode::SUCCESS)
		{
			CLProtocol::WorldMatchStartRes res;
			res.result = nRes;
			player->SendProtocol(res);
			ErrorLogStream << "guild battle match start(" << req.info.id << ") errorcode(" << nRes << ") " << LogStream::eos;
		}
		return;
	}
	else if (req.type == PkType::PK_3V3_ACTIVITY)
	{
		return;
	}

	Router::SendToMatch(req);

	// 添加判断，
	// Router::SendToMatch(req);
}

void HandleProtocol(CLProtocol::MatchServerMatchStartRes &res)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(res.roleid);
	if (!player)
	{
		return;
	}

	if (res.result == ErrorCode::SUCCESS)
	{
		player->OnStartMatch();
	}
	
	CLProtocol::WorldMatchStartRes notify;
	notify.result = res.result;
	player->SendProtocol(notify);
}

void HandleProtocol(CLProtocol::MatchServerMatchCancelReq &req)
{
    Router::SendToMatch(req);
}

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoReq& req)
{
    WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
    if (!player)
    {
        CLProtocol::MatchServerQueryPlayerInfoRes res;
        res.sessionId = req.sessionId;
        res.result = MATCH_QUERY_PLAYER_OFFLINE;
        Router::SendToMatch(res);
    }
    else
    {
        player->SendToScene(req);
    }
}

void HandleProtocol(CLProtocol::MatchServerQueryPlayerInfoRes& res)
{
	if (res.type == MATCH_QUERY_GUILD_RACE_INFO)
	{
		auto member = GuildMgr::Instance()->FindGuildMember(res.roleId);
		if (member)
		{
			auto guild = GuildMgr::Instance()->FindGuildByID(member->GetGuildID());
			if (guild)
			{
				res.racePlayerInfo.guildName = guild->GetName();
			}
			res.racePlayerInfo.remainHp = member->GetGuildBattleHP();
			res.racePlayerInfo.remainMp = member->GetGuildBattleMP();
			member->SetInspireSkill(res.racePlayerInfo.skills);
		}
	}
	else if (res.type == MATCH_PREMIUM_LEAGUE_PRELIMINAY_INFO)
	{
		auto gamer = PremiumLeagueMgr::Instance()->FindGamer(res.roleId);
		if (gamer)
		{
			res.racePlayerInfo.remainHp = gamer->GetRemainHp();
			res.racePlayerInfo.remainMp = gamer->GetRemainMp();
		}
	}
    Router::SendToMatch(res);
}

void HandleProtocol(CLProtocol::WorldNotifyRaceStart& res)
{
	DebugLogStream << "player(" << res.roleId << ") match race start." << LogStream::eos;

    WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(res.roleId);
    if (!player)
    {
		DebugLogStream << "player(" << res.roleId << ") match race started, but player offline." << LogStream::eos;
        return;
    }

	res.isRecordLog = WSApplication::Instance()->IsRecordDungeonLog() ? 1 : 0;
	res.isRecordReplay = WSApplication::Instance()->IsRecordPkReplay() ? 1 : 0;
    player->SendProtocol(res);

    // 设置玩家状态
    // ToDo...

    // 通知Scene玩家进入比赛了
    /*CLProtocol::WorldMatchRaceStart req;
    req.roleId = player->GetID();
    player->SendToScene(req);*/
}

void HandleProtocol(CLProtocol::MatchServerRaceEnd& res)
{
    WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(res.result.roleId);
    if (!player)
    {
        InfoLogStream << "player(" << res.result.roleId << ") not online." << LogStream::eos;
        return;
    }

    // 发到场景服务器
    player->SendToScene(res);
}

void HandleProtocol(CLProtocol::MatchServerDungeonRaceEnd& res)
{
    /*auto team = WSTeamMgr::Instance()->FindTeam(res.teamId);
    if (!team)
    {
        ErrorLogStream << "recv dungeon race(" << res.endInfo.gamesessionId << ")'s end info, but can't find team(" << res.teamId << ")." << LogStream::eos;
        return;
    }

    team->BroadcastToScene(res);*/

	auto team = WSTeamMgr::Instance()->FindTeam(res.teamId);
	if (team)
	{
		team->SetStatus(TEAM_STATUS_IDEL);
	}

	auto race = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(res.endInfo.gamesessionId);
	if (!race)
	{
		ErrorLogStream << "recv dungeon race(" << res.endInfo.gamesessionId << ")'s end info, but can't find race." << LogStream::eos;
		return;
	}

	for (auto& endInfo : res.endInfo.infoes)
	{
		auto player = WSPlayerMgr::Instance()->FindPlayer(endInfo.roleId);
		if (player)
		{
			race->OnPlayerRaceEnd(player, (LevelScore)endInfo.score, endInfo.beHitCount, res.endInfo.usedTime, endInfo.md5, 0,0,0,0,0, endInfo.bossDamage);
		}
	}
}

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceReq& req)
{
    Router::SendToMatch(req);
}

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceRes& res)
{
    WSTeam* team = WSTeamMgr::Instance()->FindTeam(res.teamId);
    if (!team)
    {
        // 发送到RelayServer，取消比赛
        // ToDo...
        return;
    }

    team->BroadcastToScene(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMatchQueryFriendStatusReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldMatchQueryFriendStatusRes res;

	class MatchFriendVistor : public RelationVisitor
	{
	public:
		virtual bool operator()(WSRelation* relation)
		{
			auto friendPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());
			if (friendPlayer)
			{
				FriendMatchStatusInfo info;
				info.roleId = friendPlayer->GetID();
				info.status = (UInt8)friendPlayer->GetMatchStatus();
				infoes.push_back(info);
			}
			return true;
		}

		std::vector<FriendMatchStatusInfo> infoes;
	};

	MatchFriendVistor vistor;
	player->GetRelationMgr().VisitRelations(RELATION_MASTER, vistor);
	player->GetRelationMgr().VisitRelations(RELATION_DISCIPLE, vistor);
	player->GetRelationMgr().VisitRelations(RELATION_FRIEND, vistor);
	res.infoes = vistor.infoes;
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMatchCancelRes& res)
{
	if (!player)
	{
		return;
	}

	player->SendProtocol(res);

	if (res.result == ErrorCode::SUCCESS)
	{
		player->OnCancleMatch();
	}
}

void HandleProtocol(CLProtocol::MatchServerMatchDungeonRes& res)
{
	if (res.result != ErrorCode::SUCCESS)
	{
		CLProtocol::SceneDungeonStartRes notify;
		notify.result = res.result;

		for (auto playerId : res.playerIds)
		{
			auto player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
			if (player)
			{
				player->SendProtocol(notify);
			}
		}
		return;
	}
}

void HandleProtocol(CLProtocol::MatchServerGuildBattleRaceEnd& res)
{
	GuildBattleMgr::Instance()->OnBattleRaceEnd(res);
}

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueRaceEnd& res)
{
	if (PremiumLeagueMgr::Instance()->GetStatus() > PLS_PRELIMINAY)
	{
		PremiumLeagueMgr::Instance()->OnBattleRaceEnd(res);
	}
	else
	{
		PremiumLeagueMgr::Instance()->OnPreliminayRaceEnd(res);
	}
	
}

void HandleProtocol(CLProtocol::RelayServerNotifyRaceStart& notify)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		return;
	}

	if (notify.raceType == RaceType::RT_GUILD_BATTLE)
	{
		GuildBattleMgr::Instance()->OnBattleRaceStart(player);
	}
	else if (notify.raceType == RaceType::RT_PK || notify.raceType == RaceType::RT_PREMIUM_LEAGUE_PRELIMINAY || notify.raceType == RaceType::RT_EQULA_PK)
	{
		player->OnStartPvPRace();
	}
	else if (notify.raceType == RaceType::RT_PK_3V3)
	{
		player->OnStartRoomRace();
	}
	else if (notify.raceType == RaceType::RT_SCORE_WAR)
	{
		player->OnStartRoomRace();
	}
	else if (notify.raceType == RaceType::RT_3V3_TUMBLE)
	{
		player->OnStartRoomRace();
	}
	else if (notify.raceType == RaceType::RT_2V2_TUMNLE)
	{
		player->OnStartRoomRace();
	}

	CLProtocol::WorldMatchRaceStart req;
	req.roleId = notify.roleId;
	player->SendToScene(req);
}

void HandleProtocol(CLProtocol::MatchServerPlayerMatchSuccessNotify& notify)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		return;
	}

	player->SendMatchSuccessLog(notify.zoneId, notify.opponentId, notify.type, notify.seasonLevel, notify.matchScore, notify.usedTime);
}

void HandleProtocol(CLProtocol::MatchServerPlayerMatchCancelNotify& notify)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(notify.roleId);
	if (!player)
	{
		return;
	}

	player->SendMatchCancelLog(notify.type, notify.seasonLevel, notify.matchScore, notify.matchTime);
}

void HandleProtocol(CLProtocol::MatchServerPkRoomReq& protocol)
{
	for (auto& fighter : protocol.players)
	{
		fighter.serverCreateTime = GameParamMgr::Instance()->GetValue(PARAM_NAME_GAME_FIRST_START);
	}

	Router::SendToMatch(protocol);
}


void HandleProtocol(CLProtocol::UnionMatchServerQueryPlayerInfoReq& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.msg.roleId);
	if (!player)
	{
		CLProtocol::UnionMatchServerQueryPlayerInfoRes res;
		res.msg.sessionId = protocol.msg.sessionId;
		res.msg.result = MATCH_QUERY_PLAYER_OFFLINE;
		Router::SendToUnion(USFT_CHAMPION, res);
	}
	else
	{
		player->SendToScene(protocol);
	}
}
