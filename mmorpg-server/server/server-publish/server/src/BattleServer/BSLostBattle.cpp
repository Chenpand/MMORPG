#include "BSLostBattle.h"

#include <CLSceneDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLLostDungeonDefine.h>
#include <CLLostDungeonBattleDataEntry.h>
#include <CLBattleDefine.h>
#include <CLLostDungeonDefine.h>

#include "BSNetwork.h"
#include "BSLostBattleMgr.h"
#include "BSPlayer.h"
#include "CLErrorCode.h"
#include "RelaySessionMgr.h"
#include "BSRaceMgr.h"
#include "BSPlayerMgr.h"

BSLostBattle::BSLostBattle()
{
	m_floorNum = 0;
	m_state = LDBS_NONE;
	m_playerLimit = 0;
	
	m_DestroyBattleTimer.SetInterval(40 * 60 * 1000);
};

BSLostBattle::~BSLostBattle()
{
};

void	BSLostBattle::InitBattle(LostDungeonBattleDataEntry* battleData)
{
	if (!battleData)
	{
		return;
	}
	this->SetBattleDataId(battleData->id);
	this->SetSceneId(battleData->mapId);
	m_playerLimit = battleData->maxPlayerNum;
}

UInt32	BSLostBattle::GetPlayerNum()
{ 
	return playerSet.size() + waitPlayers.size(); 
}

bool   BSLostBattle::IsFull()
{
	return GetPlayerNum() >= GetPlayerLimit();
}

bool BSLostBattle::IsTimeToDestory(const Avalon::Time& now)
{
	if (GetState() != LDBS_WAIT_DESTORY)
	{
		return false;
	}

	if (m_destoryTime == 0 || now < m_destoryTime)
	{
		return false;
	}

	return true;
}

void BSLostBattle::NotifyCreateSceneBattle()
{
	CLProtocol::SceneBattleCreateBattleReq req;
	req.battleID = GetId();
	req.battleType = BTY_DILAO;
	req.sceneId = GetSceneId();
	req.battleDataId = GetBattleDataId();
	req.lostDungeonFloor = GetFloor();
	BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), req);
	this->SetState(LDBS_CREATING);
	InfoLogStream << "req battle scene create lost dungeon battle floor : " << this->GetFloor() << ", battle id : " << this->GetId() << ", battleDataId : " 
		<< ", battleDataId : " << this->GetBattleDataId() << ", sceneId : " << this->GetSceneId() << LogStream::eos;
}

void BSLostBattle::NotifyCloseSceneBattle(UInt8 reason)
{
	CLProtocol::SceneBattleDestroyBattle req;
	req.battleType = BTY_DILAO;
	req.battleID = GetId();
	req.reason = reason;
	BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), req);
	this->SetState(LDBS_CLOSEING);
	m_DestroyBattleTimer.SetLastTime(0);
	InfoLogStream << "request scene destroy, battle id : " << GetId() << ", reason : " << (UInt32)reason << LogStream::eos;
}

UInt32 BSLostBattle::PkPlayer(UInt64 roleId, BSPlayer* attacker, BSPlayer* attackeder, UInt32 dungeonID)
{
	if (!attacker || !attackeder)
	{
		return ErrorCode::ITEM_INVALID_INDEX;
	}

	if (attackeder->GetLostState() == LDBPS_MATCHING || attackeder->GetLostState() == LDBPS_RACING)
	{
		return	ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_BATELING;
	}
	
	if (attackeder->GetProtectTimeStamp() > 0 && attackeder->GetProtectTimeStamp() > (UInt32)CURRENT_TIME.Sec())
	{
		return ErrorCode::LOSTDUNGEON_PK_FAIL_TARGT_PROTEDING;
	}

	UInt64 attackerId = attacker->GetID();
	UInt64 attackederId = attackeder->GetID();
	//开始创建比赛
 	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		ErrorLogStream << "no free relay server, roleId : " << attackerId << ", dstId : "
			<< attackederId << ", battle id : " << GetId() << LogStream::eos;
		return ErrorCode::LOSTDUNGEON_CREATE_RACE_FIAL;
	}

	BSBaseRace* race = BSRaceMgr::Instance()->CreateRace(LDRT_PVP);
	if (!race)
	{
		ErrorLogStream << "create race fail race type : " << LDRT_PVP << ", role: "
			<< attackerId << ", dstId : "<< attackederId << ", battle id : " << GetId() << LogStream::eos;
		return ErrorCode::LOSTDUNGEON_CREATE_RACE_FIAL;
	}
	BSLostPKRace* pkRace = dynamic_cast<BSLostPKRace*>(race);
	if (!pkRace)
	{
		return ErrorCode::LOSTDUNGEON_CREATE_RACE_FIAL;
	}
	pkRace->SetLostBattle(this);
	pkRace->SetRaceID(GenRaceID());
	pkRace->SetDungeonID(dungeonID);
	pkRace->OnCreate(attackerId, attackederId);
	pkRace->SetRelayServerID(relaySession->GetNodeID());
	std::vector<RacePlayerInfo> racePlayerInfos;
	racePlayerInfos.push_back(attacker->GetRacePlayerInfo());
	racePlayerInfos.push_back(attackeder->GetRacePlayerInfo());
	pkRace->SetPlayerInfos(racePlayerInfos);
	pkRace->SetStatus(BRCS_CREATING);
	
	pkRace->SetAttackerHpBack(attacker->GetRacePlayerInfo().remainHp);
	pkRace->SetAttackerMpBack(attacker->GetRacePlayerInfo().remainMp);
	pkRace->SetAttackederHpBack(attackeder->GetRacePlayerInfo().remainHp);
	pkRace->SetAttackederMpBack(attackeder->GetRacePlayerInfo().remainMp);
	BSRaceMgr::Instance()->AddRace(race);

	//请求relay创建比赛
	return pkRace->StartRace();
}

bool BSLostBattle::OnPlayerBeginEnterBattle(UInt32 accId, ObjID_t roleId, 
	UInt32 sourceSceneNodeId, UInt32 dstSceneNodeId, RacePlayerInfo& racePlayerInfo)
{
	auto player = FindPlayer(roleId);
	if (player)
	{
		player->SetSourceSceneNodeId(sourceSceneNodeId);
		player->SetDstSceneNodeId(dstSceneNodeId);
		player->SetSrcZoneId(CL_GET_ZONEID_FROM_NODEID(sourceSceneNodeId));
		player->SetRacePlayerInfo(racePlayerInfo);
		player->SetLostState(LDBPS_ENTERING);
	}
	else
	{
		player = new BSPlayer;
		player->SetAccId(accId);
		player->SetID(roleId);
		player->SetSourceSceneNodeId(sourceSceneNodeId);
		player->SetDstSceneNodeId(dstSceneNodeId);
		player->SetSrcZoneId(CL_GET_ZONEID_FROM_NODEID(sourceSceneNodeId));
		player->SetRacePlayerInfo(racePlayerInfo);
		player->SetLostState(LDBPS_ENTERING);
		BSPlayerMgr::Instance()->AddPlayer(player);
	}
	return true;
}

void BSLostBattle::OnPlayerEnterBattle(BSPlayer* player)
{
	if (!player) return;
	player->SetLostState(LDBPS_NORAML);
	player->SetLostBattleId(GetId());

	auto it = waitPlayers.find(player->GetID());
	if (it == waitPlayers.end())
	{
		InfoLogStream << "player(" << player->GetID() << ") enter battle(" << GetId() << ", floor : " << GetFloor() 
			<< ", battle data id"<< GetBattleDataId() << ", not find wait player" << LogStream::eos;
	}
	else
	{
		waitPlayers.erase(player->GetID());
	}

	auto iter = playerSet.find(player->GetID());
	if (iter != playerSet.end())
	{
		InfoLogStream << "player(" << player->GetID() << ") enter battle(" << GetId() << ", floor : " << GetFloor()
			<< ", battle data id" << GetBattleDataId() << ",  find player already in player set" << LogStream::eos;
	}
	else
	{
		playerSet.insert(player->GetID());
	}
}

void BSLostBattle::OnPlayerLeaveBattle(BSPlayer* player)
{
	if (!player) return;
	player->SetLostState(LDBPS_LEAVE);
	player->SetLostBattleId(0);

	auto iter = playerSet.find(player->GetID());
	if (iter == playerSet.end())
	{
		InfoLogStream << "player(" << player->GetID() << ") leave battle(" << GetId() << ", floor : " << GetFloor()
			<< ", battle data id" << GetBattleDataId() << ",not  find player already in player set" << LogStream::eos;
	}
	else
	{
		playerSet.erase(player->GetID());
	}
}

void BSLostBattle::OnPlayerBeginMatchBattle(ObjID_t roleId)
{
	auto it = waitPlayers.find(roleId);
	if (it == waitPlayers.end())
	{
		WaitEnterBatPlayer st;
		st.roleId = roleId;
		st.startTime = (UInt32)CURRENT_TIME.Sec();
		waitPlayers.insert(std::pair<UInt64, WaitEnterBatPlayer>(roleId, st));
	}
	else
	{
		ErrorLogStream << "find wait player already in wait list, role id : " << roleId
			<< ", battle id : " << GetId() << ", floor : " << GetFloor() << LogStream::eos;
		it->second.roleId = roleId;
		it->second.startTime = (UInt32)CURRENT_TIME.Sec();
	}
}

void BSLostBattle::OnTick(const Avalon::Time& now)
{
	std::vector<UInt64> overTimes;
	for (auto it : waitPlayers)
	{
		if (it.second.startTime + LOST_BATTLE_MATCH_OVERTIME > (UInt32)CURRENT_TIME.Sec())
		{
			overTimes.push_back(it.first);
		}
	}
	for (auto id : overTimes)
	{
		waitPlayers.erase(id);
	}

	if (GetState() == LDBS_CLOSEING && m_DestroyBattleTimer.IsTimeout(now))
	{
		InfoLogStream << "destroy battle by timer id : " << GetId() << LogStream::eos;
		this->SetState(LDBS_WAIT_DESTORY);
		this->SetDestoryTime((UInt32)CURRENT_TIME.Sec() + 10);
	}

}

void BSLostBattle::OnSceneBattleDestoryed()
{
	InfoLogStream << "recv scene destroyed res, begin destroy battle id : " << GetId() << LogStream::eos;
	this->SetState(LDBS_WAIT_DESTORY);
	this->SetDestoryTime((UInt32)CURRENT_TIME.Sec() + 10);
}