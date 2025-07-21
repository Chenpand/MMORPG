#include "DungeonMatchService.h"
#include <CLErrorCode.h>
#include <CLDungeonProtocol.h>
#include "DungeonMatchRace.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"
#include "RelaySessionMgr.h"
#include "DungeonMatchAlgorithm.h"

DungeonMatchService::DungeonMatchService()
: MatchService(MST_DUNGEON_MATCH)
{

}

bool DungeonMatchService::Init()
{
	auto algorithm = new DungeonMatchAlgorithm();
	if (!algorithm->Initialize())
	{
		return false;
	}

	SetAlgorithm(algorithm);
	return true;
}

bool DungeonMatchService::OnTeamStartMatch(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	if (!AddTeam(team))
	{
		return false;
	}

	CountTeamMatchScore(team);
	DebugLogStream << "team(" << team->GetID() << ") start dungeon match." << LogStream::eos;

	if (!OnJoinMatchPool(team))
	{
		ErrorLogStream << "team(" << team->GetID() << ") add to match pool failed." << LogStream::eos;
		DelTeam(team->GetID());
		return false;
	}

	return true;
}

bool DungeonMatchService::OnJoinMatchPool(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	Match::DungeonMatchKey key;
	key.id = team->GetDungeonID();
	key.level = team->GetScore();
	return m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key);
}

UInt32 DungeonMatchService::OnTeamCancelMatch(ObjID_t teamId)
{
	auto team = FindTeam(teamId);
	if (!team)
	{
		return ErrorCode::MATCH_CANCLE_NOT_MATCHING;
	}

	if (team->GetRaceID())
	{
		return ErrorCode::MATCH_CANCLE_RACING;
	}

	InfoLogStream << "team(" << teamId << ") cancel match." << LogStream::eos;

	DelTeam(teamId);
	return ErrorCode::SUCCESS;
}

bool DungeonMatchService::OnMatchSuccess(VMatchTeam allTeams)
{
	DungeonMatchRace* race = (DungeonMatchRace*)CreateRace();
	race->SetID(MatchServiceMgr::Instance()->GenRaceID());

	race->OnCreate(allTeams);

	AddRace(race);

	auto members = race->GetAllMember();

	CLProtocol::MatchServerQueryPlayerInfoReq req;
	req.sessionId = race->GetID();
	req.dungeonId = race->GetDungeonID();
	req.type = MATCH_QUERY_DUNGEON_INFO;
	UInt8 seat = 0;
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		member->SetSeat(seat++);
		req.roleId = member->GetID();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), req);
	}

	return true;
}

void DungeonMatchService::OnQueryPlayerDungeonInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, DungeonRacePlayerInfo& info)
{
	auto race = FindRace(sessionId);
	if (!race)
	{
		return;
	}

	InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

	// 获取玩家信息失败
	if (result != 0)
	{
		ErrorLogStream << "player(" << roleId << ") is offline, match result (" << sessionId << ")." << LogStream::eos;
		DismissMatchRace(race, true);
		MatchServiceMgr::Instance()->OnPlayerCancelMatch(roleId);
		return;
	}

	auto member = race->FindMember(roleId);
	if (!member)
	{
		ErrorLogStream << "race(" << sessionId << ") can't find member(" << roleId << ")." << LogStream::eos;
		return;
	}

	member->SetDungeonInfo(info);

	if (!race->IsAllPlayerHasRaceInfo())
	{
		return;
	}

	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		DismissMatchRace(race, false);
		return;
	}

	CLProtocol::MatchSvrStartGameReq startReq;
	startReq.raceType = RT_DUNGEON;
	startReq.gamesessionID = race->GetID();
	//UInt32 seat = 0;
	auto members = race->GetAllMember();
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		startReq.gamesessionFighter.push_back(member->GetFighter());
	}

	relaySession->SendProtocol(startReq);

	race->SetRelayServerID(relaySession->GetNodeID());
	race->ChangeState(MRS_WAITING_RELAY_SERVER_RES);
}

void DungeonMatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	DungeonMatchRace* race = (DungeonMatchRace*)FindRace(gamesessionID);
	if (!race)
	{
		return;
	}

	InfoLogStream << "race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;

	if (result != 0)
	{
		ErrorLogStream << "relay server return error(" << result << "), match(" << gamesessionID << ")." << LogStream::eos;
		DismissMatchRace(race, true);
		return;
	}

	if (race->GetAllTeam().empty())
	{
		ErrorLogStream << "match(" << gamesessionID << ") team list is empty." << LogStream::eos;
		DismissMatchRace(race, true);
		return;
	}

	race->ChangeState(MRS_RACE);

	// 更新RelayServer负载
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

	CLProtocol::WorldDungeonStartRaceReq req;
	req.dungeonId = race->GetDungeonID();
	req.isHell = race->IsHell();
	req.passedAreaIndex = 0;
	req.session = gamesessionID;
	req.relayAddr = address;
	for (auto member : race->GetAllMember())
	{
		req.playerInfoes.push_back(member->GetDungeonInfo());
	}
	MSRouter::Instance()->SendToWorld(race->GetWsID(), req);
}