#include "BSLostMonsterRace.h"
#include "BSLostBattleMgr.h"
#include "BSPlayerMgr.h"
#include "BSRaceMgr.h"
#include "BSRouter.h"

BSLostMonsterRace::BSLostMonsterRace()
	:BSBaseRace(0)
{
	m_playerId = 0;
	m_ownerBattle = NULL;
}

BSLostMonsterRace::BSLostMonsterRace(UInt8 raceType)
	: BSBaseRace(raceType)
{
	m_playerId = 0;
	m_ownerBattle = NULL;
}

BSLostMonsterRace::~BSLostMonsterRace()
{
	auto player = BSPlayerMgr::Instance()->FindPlayer(m_playerId);
	if (player)
	{
	}
}

void BSLostMonsterRace::OnCreate(UInt64 playerId, CLProtocol::DungenServerTeamStartRaceReq& teamStartRaceReq)
{
	m_playerId = playerId;
	m_teamStartRaceReq = teamStartRaceReq;
	m_battleMonster = teamStartRaceReq.monster;
	SetDungeonID(teamStartRaceReq.dungeonId);
}

BSLostBattle* BSLostMonsterRace::GetLostBattle()
{
	if (!m_ownerBattle)
	{
		auto battle = BSLostBattleMgr::Instance()->FindBattle(m_battleID);
		if (!battle) return NULL;
		m_ownerBattle = dynamic_cast<BSLostBattle*> (battle);
		if (!m_ownerBattle)  return NULL;
	}
	return m_ownerBattle;
}

UInt32 BSLostMonsterRace::StartRace()
{
	this->SetStatus(BRCS_CREATING);
	auto player = BSPlayerMgr::Instance()->FindPlayer(m_playerId);
	if (player)
	{
		player->SetLostState(LDBPS_MATCHING);
		player->SetLostRaceId(GetRaceID());
	}
	//×ª·¢¸øDungeonServer
	m_teamStartRaceReq.battleTempRaceId = this->GetRaceID();
	Router::SendToDungeon(m_teamStartRaceReq);
	return ErrorCode::SUCCESS;
}

void BSLostMonsterRace::OnStartRaceFail()
{
	auto player = BSPlayerMgr::Instance()->FindPlayer(m_playerId);
	if (player)
	{
		player->SetLostState(LDBPS_NORAML);
		player->SetLostRaceId(0);
	}
}

void BSLostMonsterRace::OnStartDungeonRes(UInt32 result, UInt64 gamesessionID)
{
	DebugLogStream << "monster dungeon race (" << gamesessionID << ") recv start response, result(" << result << LogStream::eos;
	if (result != 0)
	{
		ErrorLogStream << "dungeon server return error(" << result << "), race id(" << GetRaceID() << ")." << LogStream::eos;
		SetStatus(BRCS_WAIT_CLOSE);
		SetDestoryTime((UInt32)CURRENT_TIME.Sec());

		this->OnStartRaceFail();
		return;
	}

	SetStatus(BRCS_RACING);

	BSRaceMgr::Instance()->RemoveRace(GetRaceID());
	this->SetRaceID(gamesessionID);
	BSRaceMgr::Instance()->AddRace(this);

	auto player = BSPlayerMgr::Instance()->FindPlayer(m_playerId);
	if (player)
	{
		player->SetLostState(LDBPS_RACING);
		player->SetLostRaceId(GetRaceID());
		BSRaceMgr::Instance()->OnPlayerJoinRace(this, player);
	}
}

void BSLostMonsterRace::OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo)
{
	auto player = BSPlayerMgr::Instance()->FindPlayer(m_playerId);
	if (player)
	{
		for (auto playerInfo : endInfo.infoes)
		{
			if (playerInfo.roleId == m_playerId )
			{
				player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, playerInfo.playerRemainHp, playerInfo.playerRemainMp);
			}
		}
	}
}