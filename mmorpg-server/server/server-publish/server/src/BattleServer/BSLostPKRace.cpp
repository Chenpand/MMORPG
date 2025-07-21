#include "BSLostPKRace.h"
#include "BSLostBattleMgr.h"
#include "RelaySessionMgr.h"
#include "GameZoneMgr.h"
#include "BSNetwork.h"
#include "BSRaceMgr.h"
#include "BSApplication.h"

#include <CLErrorCode.h>
#include <CLGameSessionProtocol.h>
#include <CLRelayServerProtocol.h>

BSLostPKRace::BSLostPKRace()
	:BSBaseRace(0)
{
	m_attacker = 0;
	m_attackeder = 0;
	m_ownerBattle = NULL;
	m_attackerHpBack = 0;
	m_attackerMpBack = 0;
	m_attackederHpBack = 0;
	m_attackederMpBack = 0;
}

BSLostPKRace::BSLostPKRace(UInt8 raceType)
	: BSBaseRace(raceType)
{
	m_attacker = 0;
	m_attackeder = 0;
	m_ownerBattle = NULL;
	m_attackerHpBack = 0;
	m_attackerMpBack = 0;
	m_attackederHpBack = 0;
	m_attackederMpBack = 0;
}

BSLostPKRace::~BSLostPKRace()
{
	std::vector<BSPlayer*> players;
	GetAllPlayers(players);
}

void BSLostPKRace::OnCreate(UInt64 attacker, UInt64 attackeder)
{
	m_attacker = attacker;
	m_attackeder = attackeder;
}

BSLostBattle* BSLostPKRace::GetLostBattle()
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

UInt32 BSLostPKRace::GetAttackerHpBack()
{
	return m_attackerHpBack;
}

void BSLostPKRace::SetAttackerHpBack(UInt32 hp)
{
	m_attackerHpBack = hp;
}

UInt32 BSLostPKRace::GetAttackerMpBack()
{
	return m_attackerMpBack;
}

void BSLostPKRace::SetAttackerMpBack(UInt32 mp)
{
	m_attackerMpBack = mp;
}

UInt32 BSLostPKRace::GetAttackederHpBack()
{
	return m_attackederHpBack;
}

void BSLostPKRace::SetAttackederHpBack(UInt32 hp)
{
	m_attackederHpBack = hp;
}

UInt32 BSLostPKRace::GetAttackederMpBack()
{
	return m_attackederMpBack;
}

void BSLostPKRace::SetAttackederMpBack(UInt32 mp)
{
	m_attackederMpBack = mp;
}

 void BSLostPKRace::GetAllPlayers(std::vector<BSPlayer*>& players)
{
	BSLostBattle* battle = GetLostBattle();
	if (!battle)	return;
		
	BSPlayer* attacker = battle->FindPlayer(m_attacker);
	if (attacker)
	{
		players.push_back(attacker);
	}
	BSPlayer* attackeder = battle->FindPlayer(m_attackeder);
	if (attackeder)
	{
		players.push_back(attackeder);
	}
}

 BSPlayer* BSLostPKRace::FindPlayer(UInt64 playerid)
 {
	 if (playerid != m_attacker && playerid != m_attackeder)
	 {
		 return NULL;
	 }
	 BSLostBattle* battle = GetLostBattle();
	 if (!battle)	return NULL;
	 return battle->FindPlayer(playerid);
 }

UInt32 BSLostPKRace::StartRace()
{
	//请求relay创建比赛	
	std::vector<RacePlayerInfo>& racePlayerInfos = GetRacePlayerInfo();

	if (racePlayerInfos.size() != 2 )
	{
		ErrorLogStream << "req relay start lost dungeon pvp race fail size of racePlayerInfos : "<< (UInt32)racePlayerInfos.size() 
			<< ", attacker : " << m_attacker << ", attackeder : " << m_attackeder << LogStream::eos;
		this->SetStatus(BRCS_WAIT_CLOSE);
		this->SetDestoryTime(Avalon::Time::CurrentTime());
		return ErrorCode::LOSTDUNGEON_CREATE_RACE_FIAL;
	}

	CLProtocol::MatchSvrStartGameReq startReq;
	startReq.raceType = RT_LOST_DUNGEON_PK;
	startReq.gamesessionID = GetRaceID();
	UInt8 pos = 0;
	CLProtocol::Fighter fighter;
	for (auto racePlayer : racePlayerInfos)
	{
		fighter.roleId = racePlayer.roleId;
		fighter.accId = racePlayer.accid;
		fighter.pos = pos++;
		fighter.wsId = 0;

		startReq.gamesessionFighter.push_back(fighter);
	}
	SendToRelay(startReq);

	std::vector<BSPlayer*> players;
	GetAllPlayers(players);
	for (auto player : players)
	{
		player->SetLostState(LDBPS_MATCHING, true);
		player->SetLostRaceId(GetRaceID());
	}
	return ErrorCode::SUCCESS;
}

void BSLostPKRace::OnRelayStartRaceRes(UInt32 result, UInt64 gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	DebugLogStream << "lost dungeon PK race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;
	
	if (result != 0)
	{
		ErrorLogStream << "relay server return error(" << result << "), race id(" << gamesessionID << ")." << LogStream::eos;
		this->SetStatus(BRCS_WAIT_CLOSE);
		this->SetDestoryTime(Avalon::Time::CurrentTime());

		this->OnStartRaceFail();
		return;
	}

	SetStatus(BRCS_RACING);
	SetRaceAddr(address);

	// 更新RelayServer负载
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(GetRelayServerID(), abality);

	//通知客户端比赛信息
	CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = RT_LOST_DUNGEON_PK;
	raceStartInfo.session = gamesessionID;
	raceStartInfo.addr = address;
	raceStartInfo.dungeonID = GetDungeonID();
	raceStartInfo.battleFlag = BSApplication::Instance()->GetBattleFlag();

	CLProtocol::MatchSvrNotifyRacePlayerInfo notify;
	notify.gamesessionID = gamesessionID;
	notify.pkDungeonId = raceStartInfo.dungeonID;
	notify.battleFlag = raceStartInfo.battleFlag;
	UInt8 pos = 0;

	for (auto& racePlayeInfo : m_playerInfoes)
	{
		racePlayeInfo.pos = pos++;
		raceStartInfo.players.push_back(racePlayeInfo);

		notify.pkType = PK_LOST_DUNGEON;
		notify.players.push_back(racePlayeInfo);
	}

	std::vector<BSPlayer*> players; 
	GetAllPlayers(players);
	for (auto player : players)
	{
		player->SetLostState(LDBPS_RACING, true);
		player->SetLostRaceId(GetRaceID());
		raceStartInfo.roleId = player->GetID();
		BSRaceMgr::Instance()->OnPlayerJoinRace(this, player);
		auto zone = GameZoneMgr::Instance()->FindGameZoneByID(player->GetSrcZoneId());
		if (!zone)
		{
			continue;
		}
		zone->SendToPlayer(raceStartInfo.roleId, raceStartInfo);
	}

	SendToRelay(notify);

	// 战斗初始包
	{
		CLProtocol::RelaySvrRaceInitPacket init;
		init.raceId = gamesessionID;
		if (raceStartInfo.Encode(init.packet))
		{
			SendToRelay(init);
		}
	}
}

void BSLostPKRace::OnStartRaceFail()
{
	std::vector<BSPlayer*> players;
	GetAllPlayers(players);
	for (auto player : players)
	{
		player->SetLostState(LDBPS_NORAML);
		player->SetLostRaceId(0);
	}
}

bool BSLostPKRace::OnRaceEnd(EGameSessionDismissType ret, const PkRaceEndInfo& raceEndInfo)
{
	for (auto& info : raceEndInfo.results)
	{
		BSPlayer* player = FindPlayer(info.roleId);
		if (!player)	continue;

		if(player->GetLostState() != LDBPS_RACING) continue;
		if(player->GetLostRaceId() != GetRaceID()) continue;

		//reset state
		//player->SetLostState(LDBPS_NORAML);
		//player->SetLostRaceId(0);

		if (info.resultFlag & resultBitmask_win || info.resultFlag & resultBitmask_draw)
		{
			player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, info.remainHp, info.remainMp);
		}
		else if (info.resultFlag & resultBitmask_invalid)
		{
			if (info.roleId == m_attacker)
			{
				player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, GetAttackerHpBack(), GetAttackerMpBack());
			}
			else if (info.roleId == m_attackeder)
			{
				player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, GetAttackederHpBack(), GetAttackederMpBack());
			}
		}
		else
		{
			player->SyncEventToBScene(SET_LOST_DUNG_HP_MP, 0, 0);
		}

		UInt64 opponenterId = 0;
		if (info.roleId == m_attacker)
		{
			opponenterId = m_attackeder;
		}
		else if (info.roleId == m_attackeder)
		{
			opponenterId = m_attacker;
		}

		CLProtocol::MatchOpponent opponent;
		BSPlayer* opponenter = FindPlayer(opponenterId);
		if (opponenter)
		{
			opponent.zoneId = opponenter->GetSrcZoneId();
			opponent.roleId = opponenter->GetID();
			opponent.name = opponenter->GetName();
			opponent.occu = opponenter->GetOccu();
		}

		CLProtocol::MatchServerRaceEnd endReq;
		endReq.type = PK_LOST_DUNGEON;
		endReq.opponent = opponent;
		endReq.result = info;
		SendToBScene(endReq);
	}
	return true;
}

BSBaseBattle* BSLostPKRace::GetBattle()
{
	return GetLostBattle();
}