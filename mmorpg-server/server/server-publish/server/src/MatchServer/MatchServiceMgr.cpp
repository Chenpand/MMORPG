#include "MatchServiceMgr.h"
#include "PvPMatchService.h"
#include "DungeonTeamService.h"
#include "PracticeMatchService.h"
#include "SeasonPvPMatchService.h"
#include "GuildBattleMatchService.h"
#include "DungeonMatchService.h"
#include "PremiumLeaguePreliminayMatchService.h"
#include "PremiumLeagueBattleMatchService.h"
#include "PKRoomMatchService.h"
#include "PK3V3MatchService.h"
#include "Pk3V3ActivityMatchService.h"
#include "Pk3V3TumbleMatchService.h"
#include <CLErrorCode.h>
#include "MSRouter.h"
#include "EqualPvPMatchSercive.h"
#include "Pk2V2ActivityMatchService.h"
#include "ChampionMatchService.h"
#include "Pk5V5MatchService.h"

MatchServiceMgr::MatchServiceMgr()
{

}

MatchServiceMgr::~MatchServiceMgr()
{

}

void MatchServiceMgr::OnTick(const Avalon::Time& now)
{
    for (auto itr : m_services)
    {
		auto service = itr.second;
		if (!service)
		{
			continue;
		}

        service->OnTick(now);
    }

	if (m_reportTimer.IsTimeout(now))
	{
		InfoLogStream << "current total player num(" << (UInt32)m_players.size() << ")." << LogStream::eos;
		for (auto itr : m_services)
		{
			auto service = itr.second;
			if (!service)
			{
				continue;
			}

			service->Report();
		}
	}
}

void MatchServiceMgr::OnWorldServerConnected(UInt32 nodeid)
{
	InfoLogStream << "world server(" << nodeid << ") connected, start to create service." << LogStream::eos;
	_CreateSingleZoneMatchServices(nodeid); 
}

bool MatchServiceMgr::Init()
{
    /*for (UInt32 i = 0; i < MST_NUM; i++)
    {
        m_services[i] = new MatchService();

    }*/

	m_reportTimer.SetInterval(Avalon::Time::SECS_OF_MIN * Avalon::Time::MSECS_OF_SEC);

	_CreateCrossZoneMatchServices();
    return true;
}

UInt32 MatchServiceMgr::OnPlayerStartPvPMatch(UInt32 wsId, const CLProtocol::MatchServerMatchStartReq& req)
{
	//InfoLogStream << "pkType is" << static_cast<UInt32>(req.type) << LogStream::eos;
    MatchService* service = NULL;
	if (req.type == PK_NORMAL_1V1)
    {
        service = _GetMatchService(wsId, MST_PVP);
    }
	else if (req.type == PK_SEASON_1V1 || req.type == PK_WUDAODAHUI)
    {
		service = _GetMatchService(wsId, MST_SEASON_PVP);
    }
	else if (req.type == PK_EQUAL_1V1)
	{
		service = _GetMatchService(wsId, MST_EQUAL_PK);
	}
	else if (req.type == PK_2V2_ACTIVITY)
	{
		service = _GetMatchService(wsId, MST_PK_2V2_ACTIVITY);
	}
	else if (req.type == PK_5V5_MATCH)
	{
		service = _GetMatchService(wsId, MST_PK_5V5_MATCH);
	}
	else
	{
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

    if (!service)
    {
        ErrorLogStream << "player(" << req.info.id << ") want start match(" << req.type << "), but can't find match service." << LogStream::eos;
        return ErrorCode::MATCH_START_SYSTEMERROR;
    }

	if (FindPlayer(req.info.id))
	{
		ErrorLogStream << "player(" << req.info.id << ") is matching." << LogStream::eos;
		return ErrorCode::MATCH_START_REPEAT;
	}

	auto player = CreatePlayer(wsId, req.info.id, req.info.accid, req.info.name);

    MatchTeam* team = new MatchTeam();
    team->SetID(MatchTeam::GenID());
    
    MatchTeamMember* member = new MatchTeamMember();
    member->SetID(req.info.id);
	member->SetName(req.info.name);
	member->SetScore(req.matchScore);
    member->SetWsID(wsId);
	member->SetLevel(req.level);
	member->SetPkType((PkType)req.type);
	member->SetFightCount(req.fightCount);
	member->SetDailyCount(req.dailyCount);
	member->SetWinStreak(req.winStreak);
	member->SetloseStreak(req.loseStreak);
	member->SetSeasonLevel(req.seasonLevel);
	member->SetServerCreateTime(req.info.serverCreateTime);
    team->AddMember(member);

    player->SetMatchType(service->GetType());
    player->SetTeamID(team->GetID());

    if (!service->OnTeamStartMatch(team))
    {
		DelPlayer(req.info.id);
		return ErrorCode::MATCH_START_SYSTEMERROR;
    }

    return ErrorCode::SUCCESS;
}

UInt32 MatchServiceMgr::OnPlayerStartGuildBattleMatch(UInt32 wsId, const CLProtocol::MatchServerGuildBattleMatchReq& req)
{
	MatchServiceType serviceType = MST_GUILD_BATTLE;
	if (req.isCross)
	{
		serviceType = MST_CROSS_GUILD_BATTLE;
	}

	MatchService* service = _GetMatchService(wsId, serviceType);
	if (!service)
	{
		ErrorLogStream << "player(" << req.info.id << ") want start match(" << (UInt8)serviceType << "), but can't find match service." << LogStream::eos;
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

	if (FindPlayer(req.info.id))
	{
		ErrorLogStream << "player(" << req.info.id << ") is matching." << LogStream::eos;
		return ErrorCode::MATCH_START_REPEAT;
	}

	auto player = CreatePlayer(wsId, req.info.id, req.info.accid, req.info.name);

	MatchTeam* team = new MatchTeam();
	team->SetID(MatchTeam::GenID());
	team->SetGuildID(req.guildId);
	team->SetTerrID(req.terrId);

	MatchTeamMember* member = new MatchTeamMember();
	member->SetID(req.info.id);
	member->SetName(req.info.name);
	member->SetGuildID(req.guildId);
	member->SetTerrID(req.terrId);
	member->SetScore(req.winStreak);
	member->SetWsID(wsId);
	member->SetPkType(PK_GUILD_BATTLE);
	member->SetWinStreak(req.winStreak);
	team->AddMember(member);

	player->SetMatchType(service->GetType());
	player->SetTeamID(team->GetID());

	if (!service->OnTeamStartMatch(team))
	{
		DelPlayer(req.info.id);
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

	return ErrorCode::SUCCESS;
}

UInt32 MatchServiceMgr::OnPlayerStartPremiumLeagueMatch(UInt32 wsId, const CLProtocol::MatchServerPremiumLeagueMatchReq& req)
{
	MatchService* service = _GetMatchService(wsId, MST_PREMIUM_LEAGUE_PRELIMINAY);

	if (!service)
	{
		ErrorLogStream << "player(" << req.info.id << ") want start match(" << (UInt8)MST_PREMIUM_LEAGUE_PRELIMINAY << "), but can't find match service." << LogStream::eos;
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

	if (FindPlayer(req.info.id))
	{
		ErrorLogStream << "player(" << req.info.id << ") is matching." << LogStream::eos;
		return ErrorCode::MATCH_START_REPEAT;
	}

	auto player = CreatePlayer(wsId, req.info.id, req.info.accid, req.info.name);

	MatchTeam* team = new MatchTeam();
	team->SetID(MatchTeam::GenID());

	MatchTeamMember* member = new MatchTeamMember();
	member->SetID(req.info.id);
	member->SetName(req.info.name);
	member->SetScore(req.winStreak);
	member->SetWsID(wsId);
	member->SetPkType(PK_PREMIUM_LEAGUE_PRELIMINAY);
	member->SetWinStreak(req.winStreak);
	member->SetSeasonLevel(req.seasonLevel);
	team->AddMember(member);

	player->SetMatchType(service->GetType());
	player->SetTeamID(team->GetID());

	if (!service->OnTeamStartMatch(team))
	{
		DelPlayer(req.info.id);
		return ErrorCode::MATCH_START_SYSTEMERROR;
	}

	return ErrorCode::SUCCESS;
}

bool MatchServiceMgr::OnPlayerStartDungeonTeamRace(UInt32 wsId, UInt32 wsTeamId, const SceneNpc& cityMonster, UInt32 dungeonId, UInt8 isHell, const std::vector<CLProtocol::Fighter>& fighters)
{
	MatchService* service = _GetMatchService(wsId, MST_DUNGEON_TEAM);

	if (!service)
	{
		ErrorLogStream << "world(" << wsId << ") want start match(" << (UInt8)MST_DUNGEON_TEAM << "), but can't find match service." << LogStream::eos;
		return false;
	}

    MatchTeam* team = new MatchTeam();
    team->SetID(MatchTeam::GenID());
    team->SetWsTeamID(wsTeamId);
    team->SetWsID(wsId);
    team->SetDungeonID(dungeonId);
    team->SetHell(isHell);
	team->SetCityMonster(cityMonster);

    for (auto& fighter : fighters)
    {
        MatchTeamMember* member = new MatchTeamMember();
        member->SetWsID(wsId);
        member->SetByFigher(fighter);

        team->AddMember(member);

		DebugLogStream << "player(" << fighter.roleId << ") join dungeon team(" << team->GetID() << ") race dungeon(" << dungeonId << ")." << LogStream::eos;
    }

	if (!service->OnTeamStartMatch(team))
    {
		ErrorLogStream << "dungeon team(" << team->GetID() << ") race dungeon(" << dungeonId << ") start failed." << LogStream::eos;
        return false;
    }

    return true;
}

bool MatchServiceMgr::OnPlayerStartDungeonMatch(UInt32 wsId, UInt32 wsTeamId, UInt32 dungeonId, UInt8 isHell, const std::vector<MatchPlayerBaseInfo>& fighters, UInt32 level)
{
	CLProtocol::MatchServerMatchDungeonRes res;

	MatchService* service = _GetMatchService(wsId, MST_DUNGEON_MATCH);
	if (!service)
	{
		ErrorLogStream << "world(" << wsId << ") want start match(" << (UInt8)MST_DUNGEON_MATCH << "), but can't find match service." << LogStream::eos;

		res.result = ErrorCode::DUNGEON_MATCH_START_FAILED;
		MSRouter::Instance()->SendToWorld(wsId, res);
		return false;
	}

	bool check = true;
	for (auto& fighter : fighters)
	{
		res.playerIds.push_back(fighter.id);
		if (FindPlayer(fighter.id))
		{
			ErrorLogStream << "player(" << fighter.id << ") is matching." << LogStream::eos;
			check = false;
		}
	}

	if (!check)
	{
		return false;
	}

	MatchTeam* team = new MatchTeam();
	team->SetID(MatchTeam::GenID());
	team->SetWsID(wsId);
	team->SetWsTeamID(wsTeamId);
	team->SetDungeonID(dungeonId);
	team->SetHell(isHell);
	team->SetScore(level);

	std::vector<MatchServicePlayer*> players;
	for (auto& fighter : fighters)
	{
		MatchTeamMember* member = new MatchTeamMember();
		member->SetWsID(wsId);
		member->SetID(fighter.id);
		member->SetName(fighter.name);

		auto player = CreatePlayer(wsId, fighter.id, fighter.accid, fighter.name);
		player->SetMatchType(MST_DUNGEON_MATCH);
		player->SetTeamID(team->GetID());
		players.push_back(player);
		
		team->AddMember(member);
	}

	if (!service->OnTeamStartMatch(team))
	{
		for (auto player : players)
		{
			DelPlayer(player->GetID());
		}

		res.result = ErrorCode::DUNGEON_MATCH_START_FAILED;
		MSRouter::Instance()->SendToWorld(wsId, res);
		return false;
	}

	res.result = ErrorCode::SUCCESS;
	MSRouter::Instance()->SendToWorld(wsId, res);
	return true;
}

UInt32 MatchServiceMgr::OnPlayerStartPkRoom(UInt32 wsId, UInt32 wsRoomId, const std::vector<PkRoomMatchPlayerInfo>& fighters, UInt8 pkType)
{
	//InfoLogStream << "pkType is" << static_cast<UInt32>(pkType) << LogStream::eos;
	MatchService* service = NULL;
	if (pkType == PK_3V3_MATCH)
	{
		service = _GetMatchService(wsId, MST_PK_3V3_MATCH);
	}
	else if (pkType == PK_3V3_ROOM)
	{
		service = _GetMatchService(wsId, MST_PK_ROOM);
	}
	else if (pkType == PK_3V3_ACTIVITY)
	{
		service = _GetMatchService(wsId, MST_PK_3V3_ACTIVITY);
	}
	else if (pkType == PK_3V3_TUMBLE)
	{
		service = _GetMatchService(wsId, MST_PK_3V3_TUMBLE);
	}
	
	if (!service)
	{
		ErrorLogStream << "world(" << wsId << ") want start match(" << (UInt8)pkType << "), but can't find match service." << LogStream::eos;
		return ErrorCode::DUNGEON_MATCH_START_FAILED;
	}

	bool check = true;
	for (auto& fighter : fighters)
	{
		//res.playerIds.push_back(fighter.id);
		if (FindPlayer(fighter.id))
		{
			ErrorLogStream << "player(" << fighter.id << ") is matching." << LogStream::eos;
			check = false;
		}
	}

	if (!check)
	{
		return ErrorCode::MATCH_START_REPEAT;
	}

	MatchTeam* team = new MatchTeam();
	team->SetID(MatchTeam::GenID());
	team->SetWsID(wsId);
	team->SetWsTeamID(wsRoomId);

	std::vector<MatchServicePlayer*> players;
	for (auto& fighter : fighters)
	{
		MatchTeamMember* member = new MatchTeamMember();
		member->SetWsID(fighter.wsId);
		member->SetServerCreateTime(fighter.serverCreateTime);
		member->SetID(fighter.id);
		member->SetName(fighter.name);
		member->SetScore(fighter.score);
		member->SetLevel(fighter.level);
		member->SetWinStreak(fighter.winStreak);
		member->SetloseStreak(fighter.loseStreak);
		member->SetSeasonLevel(fighter.seasonLevel);
		member->SetSeasonStar(fighter.seasonStar);
		member->SetSeasonExp(fighter.seasonExp);
		member->SetSeat(fighter.seat);
		member->SetTeamID(wsRoomId);
		member->SetPkType((PkType)pkType);

		auto player = CreatePlayer(fighter.wsId, fighter.id, fighter.accid, fighter.name);
		player->SetMatchType(service->GetType());
		player->SetTeamID(team->GetID());
		players.push_back(player);

		DebugLogStream << "[3v3] team(" << team->GetID() << ") room(" << wsRoomId << ") add member(" << member->GetID() << ")." << LogStream::eos;

		team->AddMember(member);
	}

	if (!service->OnTeamStartMatch(team))
	{
		for (auto player : players)
		{
			DelPlayer(player->GetID());
		}

		return ErrorCode::DUNGEON_MATCH_START_FAILED;
	}

	return ErrorCode::SUCCESS;
}

bool MatchServiceMgr::OnPlayerStartPracticeMatch(UInt32 wsId, std::vector<ObjID_t> roleIds,UInt8 type)
{
	MatchServiceType ServiceType;
	if (type == PK_EQUAL_PRACTICE)
	{
		ServiceType = MST_EQUAL_PK_PRACTICE;
	}
	else
	{
		ServiceType = MST_PRACTICE;
	}
	MatchService* service = _GetMatchService(wsId, ServiceType);
	if (!service)
	{
		ErrorLogStream << "world(" << wsId << ") want start match(" << (UInt8)ServiceType << "), but can't find match service." << LogStream::eos;
		return false;
	}

    MatchTeam* team = new MatchTeam();
    team->SetID(MatchTeam::GenID());
    team->SetWsID(wsId);

    for (auto& roleId : roleIds)
    {
        auto player = FindPlayer(roleId);
        if (player)
        {
            return false;
        }
    }

    for (auto& roleId : roleIds)
    {
        auto player = CreatePlayer(wsId, roleId, 0, "");
		player->SetMatchType(ServiceType);
        player->SetTeamID(team->GetID());
        player->SetWsID(wsId);

        MatchTeamMember* member = new MatchTeamMember();
        member->SetWsID(wsId);
        member->SetID(roleId);

        team->AddMember(member);
    }

	if (!service->OnTeamStartMatch(team))
    {
        return false;
    }

    return true;
}

bool MatchServiceMgr::OnPlayerStartPremiumLeagueBattle(UInt32 wsId, ObjID_t battleId, std::vector<ObjID_t> roleIds)
{
	MatchService* service = _GetMatchService(wsId, MST_PREMIUM_LEAGUE_BATTLE);
	if (!service)
	{
		ErrorLogStream << "world(" << wsId << ") want start match(" << (UInt8)MST_PREMIUM_LEAGUE_BATTLE << "), but can't find match service." << LogStream::eos;
		return false;
	}

	MatchTeam* team = new MatchTeam();
	team->SetID(MatchTeam::GenID());
	team->SetWsID(wsId);
	team->SetPremiumLeagueBattleID(battleId);

	for (auto& roleId : roleIds)
	{
		MatchTeamMember* member = new MatchTeamMember();
		member->SetWsID(wsId);
		member->SetID(roleId);
		member->SetPkType(PK_PREMIUM_LEAGUE_BATTLE);

		team->AddMember(member);
	}

	if (!service->OnTeamStartMatch(team))
	{
		return false;
	}

	return true;
}

bool MatchServiceMgr::OnPlayerStartChampionBattle(UInt32 usId, UInt32 groupID, UInt32 order, std::vector<ObjID_t> roleIds)
{
	MatchService* service = _GetMatchService(usId, MST_CHAMPION_BATTLE);
	if (!service)
	{
		ErrorLogStream << "union(" << usId << ") want start match(" << (UInt8)MST_CHAMPION_BATTLE << "), but can't find match service." << LogStream::eos;
		return false;
	}

	UInt32 teamNum = roleIds.size() / 2;
	for (UInt32 i = 0; i < teamNum; ++i)
	{
		MatchTeam* team = new MatchTeam();
		team->SetID(MatchTeam::GenID());
		team->SetWsID(usId);
		team->SetGroupID(groupID);
		team->SetOrderID(order);

		MatchTeamMember* member = new MatchTeamMember();
		member->SetWsID(usId);
		member->SetID(roleIds[i*2]);
		member->SetPkType(PK_CHAMPION_BATTLE);
		team->AddMember(member);

		member = new MatchTeamMember();
		member->SetWsID(usId);
		member->SetID(roleIds[i * 2 + 1]);
		member->SetPkType(PK_CHAMPION_BATTLE);
		team->AddMember(member);

		service->OnTeamStartMatch(team);
	}


	return true;
}

UInt32 MatchServiceMgr::OnPlayerCancelMatch(ObjID_t id)
{
    auto player = FindPlayer(id);
    if (!player)
    {
        ErrorLogStream << "player(" << id << ") request to cancel match, but can't find player." << LogStream::eos;
        return ErrorCode::MATCH_CANCLE_NOT_MATCHING;
    }

    auto service = _GetMatchService(player->GetWsID(), player->GetMatchType());
    if (!service)
    {
        ErrorLogStream << "player(" << id << ") request to cancel match, player's match type(" 
                        << player->GetMatchType() << ") is invalid." << LogStream::eos;
        return ErrorCode::MATCH_START_SYSTEMERROR;
    }

	CLProtocol::MatchServerPlayerMatchCancelNotify notify;
	notify.roleId = id;
	auto team = service->FindTeam(player->GetTeamID());
	if (team)
	{
		notify.matchTime = (UInt32)CURRENT_TIME.Sec() - team->GetCreateTime();

		auto member = team->FindMember(id);
		if (member)
		{
			notify.seasonLevel = member->GetSeasonLevel();
			notify.matchScore = member->GetScore();
		}
	}
	
    auto result = service->OnTeamCancelMatch(player->GetTeamID());
	if (result == ErrorCode::SUCCESS)
	{
		// 发送取消匹配日志
		MSRouter::Instance()->SendToWorld(player->GetWsID(), notify);
	}

	return result;
}

void MatchServiceMgr::OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
    for (auto itr : m_services)
    {
		auto service = itr.second;
        if (!service)
        {
            continue;
        }

        service->OnQueryPlayerRaceInfoRes(sessionId, roleId, result, info);
    }
}

void MatchServiceMgr::OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info)
{
	for (auto itr : m_services)
	{
		auto service = itr.second;
		if (!service)
		{
			continue;
		}

		service->OnQueryPlayerDungeonInfoRes(sessionId, roleId, result, info);
	}
}

void MatchServiceMgr::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	for (auto itr : m_services)
	{
		auto service = itr.second;
		if (!service)
        {
            continue;
        }

        service->OnRelayServerStartResponse(result, gamesessionID, abality, address);
    }
}

void MatchServiceMgr::OnPvPRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo)
{
	for (auto itr : m_services)
	{
		auto service = itr.second;
        if (service && service->IsPkService())
        {
			service->OnRaceEnd(gamesessionID, ret, (const void*)&raceEndInfo);
        }
    }
}

void MatchServiceMgr::OnDungeonRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const DungeonRaceEndInfo& raceEndInfo)
{
	for (auto itr : m_services)
	{
		auto service = itr.second;
        if (service && service->IsDungeonService())
        {
            service->OnRaceEnd(gamesessionID, ret, (const void*)&raceEndInfo);
        }
    }
}

MatchServicePlayer* MatchServiceMgr::FindPlayer(ObjID_t id)
{
    auto itr = m_players.find(id);
    return itr == m_players.end() ? NULL : itr->second;
}

void MatchServiceMgr::DelPlayer(ObjID_t id)
{
    auto player = FindPlayer(id);
    if (player)
    {
        m_players.erase(id);
        delete player;
    }
}

MatchServicePlayer* MatchServiceMgr::CreatePlayer(UInt32 wsId, ObjID_t id, UInt32 accid, std::string name)
{
    auto player = new MatchServicePlayer();
    player->SetID(id);
    player->SetWsID(wsId);
    player->SetAccountID(accid);
    player->SetName(name);

    m_players[id] = player;
    return player;
}

ObjID_t MatchServiceMgr::GenRaceID()
{
    // 生成唯一的比赛ID,括号内为相应字段的位数
    // NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
    Avalon::Time now = Avalon::Time::CurrentTime();
    UInt64 randNum = (UInt64)rand();
    UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
    return raceID;
}

MatchService* MatchServiceMgr::_GetMatchService(UInt32 nodeid, MatchServiceType type)
{
	if (_IsCrossZoneMatch(type))
	{
		nodeid = 0;
	}

	MatchServiceKey key;
	key.zoneid = nodeid;
	key.type = type;
	auto itr = m_services.find(key);
	if (itr == m_services.end())
	{
		return NULL;
	}

	return itr->second;
}

MatchService* MatchServiceMgr::_CreateMatchService(UInt32 nodeid, MatchServiceType type)
{
	MatchService* service = NULL;
	switch (type)
	{
	case MST_PVP:
		service = new PvPMatchService();
		break;
	case MST_PRACTICE:
		service = new PracticeMatchService();
		break;
	case MST_SEASON_PVP:
		service = new SeasonPvPMatchService();
		break;
	case MST_GUILD_BATTLE:
		service = new GuildBattleMatchService();
		break;
	case MST_DUNGEON_TEAM:
		service = new DungeonTeamService();
		break;
	case MST_DUNGEON_MATCH:
		service = new DungeonMatchService();
		break;
	case MST_PREMIUM_LEAGUE_PRELIMINAY:
		service = new PremiumLeaguePreliminayMatchService();
		break;
	case MST_PREMIUM_LEAGUE_BATTLE:
		service = new PremiumLeagueBattleMatchService();
		break;
	case MST_PK_ROOM:
		service = new PkRoomMatchService();
		break;
	case MST_PK_3V3_MATCH:
		service = new Pk3V3MatchService();
		break;
	case MST_PK_3V3_ACTIVITY:
		service = new Pk3V3ActivityMatchService();
		break;
	case MST_PK_3V3_TUMBLE:
		service = new Pk3V3TumbleMatchService();
		break;
	case MST_CROSS_GUILD_BATTLE:
		service = new CrossGuildBattleMatchService();
		break;
	case MST_EQUAL_PK:
		service = new EqualPvPMatchSercive();
		break;
	case MST_EQUAL_PK_PRACTICE:
		service = new EqualPvPPracticeMatchService();
		break;
	case MST_PK_2V2_ACTIVITY:
		service = new Pk2V2ActivityMatchService();
		break;
	case MST_CHAMPION_BATTLE:
		service = new ChampionMatchService();
		break;
	case MST_PK_5V5_MATCH:
		service = new Pk5V5MatchService();
		break;
	default:
		break;
	}

	if (!service->Init())
	{
		ErrorLogStream << "service(" << (UInt32)type << ") init failed." << LogStream::eos;
		delete service;
		return NULL;
	}

	service->SetNodeID(nodeid);
	return service;
}

void MatchServiceMgr::_CreateSingleZoneMatchServices(UInt32 nodeid)
{
	for (int i = 0; i < MST_NUM; i++)
	{
		auto type = (MatchServiceType)i;
		if (_IsCrossZoneMatch(type))
		{
			continue;
		}

		auto service = _GetMatchService(nodeid, type);
		if (service)
		{
			continue;
		}

		service = _CreateMatchService(nodeid, type);
		if (!service)
		{
			ErrorLogStream << "zone(" << nodeid << ") create match service(" << (UInt32)type << ") failed." << LogStream::eos;
			continue;
		}

		MatchServiceKey key(nodeid, type);
		m_services[key] = service;
	}
}

void MatchServiceMgr::_CreateCrossZoneMatchServices()
{
	for (int i = 0; i < MST_NUM; i++)
	{
		auto type = (MatchServiceType)i;
		if (!_IsCrossZoneMatch(type))
		{
			continue;
		}

		auto service = _GetMatchService(0, type);
		if (service)
		{
			continue;
		}

		service = _CreateMatchService(0, type);
		if (!service)
		{
			ErrorLogStream << "create cross match service(" << (UInt32)type << ") failed." << LogStream::eos;
			continue;
		}

		MatchServiceKey key(0, type);
		m_services[key] = service;
	}
}

bool MatchServiceMgr::_IsCrossZoneMatch(MatchServiceType type)
{
	if (type == MST_SEASON_PVP || type == MST_PK_3V3_MATCH || type == MST_CROSS_GUILD_BATTLE || type == MST_PK_3V3_ACTIVITY || type == MST_PK_2V2_ACTIVITY || type == MST_CHAMPION_BATTLE)
	{
		return true;
	}

	return false;
}