#include "DungeonTeamService.h"
#include <CLErrorCode.h>
#include "DungeonMatchRace.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"
#include "RelaySessionMgr.h"

DungeonTeamService::DungeonTeamService()
: MatchService(MST_DUNGEON_TEAM)
{

}

bool DungeonTeamService::Init()
{
	return true;
}

bool DungeonTeamService::OnTeamStartMatch(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	if (!AddTeam(team))
	{
		return false;
	}

	DebugLogStream << "team(" << team->GetID() << ") start dungeon match." << LogStream::eos;

	VMatchTeam teams;
	teams.push_back(team);
	return OnMatchSuccess(teams);
}

bool DungeonTeamService::OnJoinMatchPool(MatchTeam* team)
{
	return true;
}

UInt32 DungeonTeamService::OnTeamCancelMatch(ObjID_t teamId)
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

bool DungeonTeamService::OnMatchSuccess(VMatchTeam allTeams)
{
	DungeonMatchRace* race = new DungeonMatchRace();
	race->SetID(MatchServiceMgr::Instance()->GenRaceID());

	race->OnCreate(allTeams);

	AddRace(race);

	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		DismissMatchRace(race, false);
		return false;
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
	return true;
}

void DungeonTeamService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
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

	// ¸üÐÂRelayServer¸ºÔØ
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

	CLProtocol::WorldServerTeamCreateRaceRes res;
	res.teamId = race->GetWsTeamID();
	res.dungeonId = race->GetDungeonID();
	res.cityMonster = race->GetCityMonster();
	res.isHell = race->IsHell();
	res.relayServerID = race->GetRelayServerID();
	res.result = result;
	res.gamesessionID = gamesessionID;
	res.address = address;
	MSRouter::Instance()->SendToWorld(race->GetWsID(), res);
}