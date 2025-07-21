#include "BSDungeonHandler.h"
#include <CLLostDungeonDefine.h>
#include <CLSystemValueDataEntry.h>

#include "BSPlayerMgr.h"
#include "BSRaceMgr.h"
#include "BSLostMonsterRace.h"
#include "BSBaseBattle.h"
#include "BSLostBattleMgr.h"
#include "BSLostTeamDungeonRace.h"
#include "BSRouter.h"

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceReq& protocol)
{
	InfoLogStream << "rev team start race req, teamId : " << protocol.teamId << ", dungeonId : " << protocol.dungeonId << ", protocol.fighters.size() = "
		<< (UInt32)protocol.fighters.size() << LogStream::eos;
	if (protocol.fighters.size() == 0) return;
	ObjID_t playerId = protocol.fighters[0].roleId;
	auto player = BSPlayerMgr::Instance()->FindPlayer(playerId);
	if (!player) return;

	if (!player->IsCanPK())
	{
		ErrorLogStream << "start team race fail, teamId : " << protocol.teamId << ", dungeonId : " << protocol.dungeonId << ", player can not pk id : " << player->GetID() << LogStream::eos;
		return;
	}

	BSLostBattle* battle = BSLostBattleMgr::Instance()->FindLostBattle(player->GetLostBattleId());
	if (!battle)
	{
		ErrorLogStream << "start team race fail, teamId : " << protocol.teamId << ", dungeonId : " << protocol.dungeonId << ", player id : " << player->GetID() 
			<< ", not find lost battle id : " << player->GetLostBattleId() << LogStream::eos;
		return;
	}
	
	//打小怪
	if (protocol.monster.guid > 0)
	{
		BSBaseRace* race = BSRaceMgr::Instance()->CreateRace(LDRT_MONSTER);
		if (!race)
		{
			ErrorLogStream << "start team race fail, teamId : " << protocol.teamId << ", dungeonId : " << protocol.dungeonId << ", player id : " << player->GetID() 
				<<", create race fail race type : " << LDRT_MONSTER << ", role: " << protocol.fighters[0].roleId << LogStream::eos;
			return;
		}
		BSLostMonsterRace* monsterRace = dynamic_cast<BSLostMonsterRace*>(race);
		if (!monsterRace)	return;

		monsterRace->SetLostBattle(battle);
		monsterRace->SetBattleID(battle->GetId());
		monsterRace->SetRaceID(BSBaseBattle::GenRaceID());
		monsterRace->OnCreate(playerId, protocol);
		BSRaceMgr::Instance()->AddRace(race);

		race->StartRace();
		return;
	}
	else
	{
		BSBaseRace* race = BSRaceMgr::Instance()->CreateRace(LDRT_DUNGEON);
		if (!race)
		{
			ErrorLogStream << "start team race fail, teamId : " << protocol.teamId << ", dungeonId : " << protocol.dungeonId << ", player id : " << player->GetID() << ", create race fail race type : " << LDRT_DUNGEON << LogStream::eos;
			return;
		}

		BSLostTeamDungeonRace* teamRace = dynamic_cast<BSLostTeamDungeonRace*>(race);
		if (!teamRace)	return;

		teamRace->SetLostBattle(battle);
		teamRace->SetBattleID(battle->GetId());
		teamRace->SetRaceID(BSBaseBattle::GenRaceID());
		teamRace->OnCreate(protocol);

		BSRaceMgr::Instance()->AddRace(race);
		race->StartRace();
		return;
	}
	
}

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceRes& protocol)
{
	BSBaseRace* race = BSRaceMgr::Instance()->FindRace(protocol.battleTempRaceId);
	if (!race)	return;

	race->OnStartDungeonRes(protocol.result, protocol.gamesessionID);
}

void HandleProtocol(CLProtocol::DungeonSvrDungeonRaceEnd& protocol)
{
	BSBaseRace* race = BSRaceMgr::Instance()->FindRace(protocol.gamesessionID);
	if (!race)	return;

	race->OnDungeonEnd(protocol.result, protocol.endInfo);
}

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol)
{
	BSRaceMgr::Instance()->OnPlayerLeaveRace(protocol.roleId, protocol.isDisconnect != 0);

	BSPlayer* player = BSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player) return;

	UInt8 oldSt = player->GetLostState();

	player->SetLostState(LDBPS_NORAML);
	player->SetLostRaceId(0);

	if (oldSt == LDBPS_RACING)
	{
		//计算保护时间
		UInt32 secs = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_BATTLE_PROTECT_TIME);
		secs = (secs == 0) ? 5 : secs;
		player->SetProtectTimeStamp((UInt32)CURRENT_TIME.Sec() + secs);
	}
	Router::SendToDungeon(protocol);
}

void HandleProtocol(CLProtocol::DungeonSvrReviveReq& protocol)
{
	auto race = BSRaceMgr::Instance()->FindRaceByRoleId(protocol.playerId);
	if (!race)
	{
		return;
	}
	protocol.raceSessionId = race->GetRaceID();
	Router::SendToDungeon(protocol);
}