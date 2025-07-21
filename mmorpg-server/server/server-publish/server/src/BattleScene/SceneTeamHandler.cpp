#include "SceneTeamHandler.h"

#include <CLSysNotify.h>
#include "TeamMgr.h"
#include "SceneMgr.h"
#include "SSRouter.h"
#include "PlayerMgr.h"
#include "ScriptMgr.h"
#include "PlayerScript.h"
#include "SSDungeonConfigMgr.h"
#include "CLTeamDungeonDataEntry.h"
#include "CityMonster.h"
#include "SceneMgr.h"

void HandleProtocol(CLProtocol::WorldSyncTeamInfoToScene& req)
{
	Team* team = TeamMgr::Instance()->FindTeam(req.id);
	if(team == NULL)
	{
		team = new Team(req.id);
		if(!TeamMgr::Instance()->AddTeam(team))
		{
			CL_SAFE_DELETE(team);
			return;
		}
	}

	team->SetMaster(req.master);
	team->SetMembers(req.members);
	team->UpdateTeamInfo();
}

void HandleProtocol(CLProtocol::WorldSyncTeamMemberStatusToScene& req)
{
    Team* team = TeamMgr::Instance()->FindTeam(req.id);
    if (team)
    {
        team->UpdateTeamMemberStatus(req.roleId, req.statusMask);
    }
}

void HandleProtocol(CLProtocol::WorldNotifyDelTeamToScene& req)
{
	Team* team = TeamMgr::Instance()->FindTeam(req.id);
	if(team != NULL)
	{
		TeamMgr::Instance()->RemoveTeam(team);
		CL_SAFE_DELETE(team);
	}
}

void HandleProtocol(CLProtocol::WorldSyncTeamBaseToScene& req)
{
	Team* team = TeamMgr::Instance()->FindTeam(req.id);
	if(team != NULL)
	{
		team->SetMaster(req.master);
		team->UpdateTeamInfo();
	}
}

void HandleProtocol(CLProtocol::WorldNotifyMemberInToScene& req)
{
	Team* team = TeamMgr::Instance()->FindTeam(req.id);
	if(team != NULL)
	{
		team->AddMember(req.memberId);
		team->UpdateTeamInfo();
	}
}

void HandleProtocol(CLProtocol::WorldNotifyMemberOutToScene& req)
{
	Team* team = TeamMgr::Instance()->FindTeam(req.id);
	if(team != NULL)
	{
		team->RemoveMember(req.memberId);
		team->UpdateTeamInfo();
	}
}

void HandleProtocol(CLProtocol::WorldServerTeamCreateRaceRes& res)
{
    Team* team = TeamMgr::Instance()->FindTeam(res.teamId);
    if (!team)
    {
        // 发送到RelayServer，取消比赛
        // ToDo...
        return;
    }

    if (res.result)
    {
        return;
    }

    team->OnRelayServerCreateRaceRet(res.relayServerID, res.dungeonId, res.gamesessionID, res.address, res.isHell != 0, res.cityMonster);
}

void HandleProtocol(CLProtocol::WorldServerTeamStartDungeonRes& res)
{
	Team* team = TeamMgr::Instance()->FindTeam(res.teamId);
	if (!team)
	{
		ErrorLogStream << "world reponse team(" << res.teamId << ") start dungeon(" << res.dungeonId << ")'s request, team unexist." << LogStream::eos;
		return;
	}

	team->OnWorldServerStartDungeonRes(res.dungeonId, res.cityMonster);
}

void HandleProtocol(Player* player, CLProtocol::SceneTeamMatchStartReq& req)
{
	CLProtocol::SceneTeamMatchStartRes res;

	auto teamTaget = TeamDungeonDataEntryMgr::Instance()->FindEntryByDungeonID(req.dungeonId);
	if (!teamTaget)
	{
		res.result = ErrorCode::DUNGEON_START_DUNGEON_NOT_EXIST;
		player->SendNotify(res.result);
		return;
	}

	if (teamTaget->type == TEAM_CITY_MONSTER)
	{
		CLProtocol::WorldTeamMatchStartReq worldReq;
		worldReq.dungeonId = req.dungeonId;
		worldReq.roleId = player->GetID();
		Router::SendToWorld(worldReq);
		return;
	}

	auto team = player->GetTeam();
	if (team)
	{
		if (team->GetMaster() != player->GetID())
		{
			res.result = ErrorCode::TEAM_MATCH_ONLY_MASTER;
			player->SendNotify(res.result);
		}

		// 组队情况下
		SceneNpc cityMonster;
		if (!team->CanStartDungeonRace(req.dungeonId, true, true, teamTaget->type == TEAM_CITY_MONSTER, cityMonster))
		{
			res.result = ErrorCode::TEAM_MATCH_START_FAILED;
		}
		else
		{
			res.result = ErrorCode::SUCCESS;
		}
	}
	else
	{
		res.result = player->GetDungeonMgr().CanPlayThisMap(req.dungeonId, true);
	}

	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
		return;
	}

	CLProtocol::WorldTeamMatchStartReq worldReq;
	worldReq.dungeonId = req.dungeonId;
	worldReq.roleId = player->GetID();
	Router::SendToWorld(worldReq);
}

void HandleProtocol(CLProtocol::WorldTeamQueryTargetCondReq& req)
{
	auto player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (!player)
	{
		return;
	}

	auto teamTargetData = TeamDungeonDataEntryMgr::Instance()->FindEntry(req.teamTargetId);
	if (!teamTargetData)
	{
		return;
	}

	if (teamTargetData->type == TEAM_CITY_MONSTER)
	{
		player->SyncCityMonsterRemainTimesToWorld();
	}
	else
	{
		player->GetDungeonMgr().SyncRemainTimes2World(teamTargetData->dungeonId);
	}

	
}

void HandleProtocol(CLProtocol::WorldTeamDungeonRaceStart& protocol)
{
	if (protocol.cityMonster.guid > 0 && protocol.cityMonster.type == SOT_CITYMONSTER && protocol.cityMonster.funcType == CITY_MONSTER_LOST_DUNGEON)
	{
		auto monster = SceneMgr::Instance()->FindCityMonster(protocol.cityMonster.guid);
		if (monster)
		{
			monster->AddRacingRace(protocol.raceId);
		}
	}
}


void HandleProtocol(CLProtocol::WorldTeamDungeonRaceOver& protocol)
{
	if (protocol.cityMonster.guid > 0 && protocol.cityMonster.type == SOT_CITYMONSTER && protocol.cityMonster.funcType == CITY_MONSTER_LOST_DUNGEON)
	{
		auto monster = SceneMgr::Instance()->FindCityMonster(protocol.cityMonster.guid);
		if (monster)
		{
			monster->DelRacingRace(protocol.raceId);
		}
	}
}