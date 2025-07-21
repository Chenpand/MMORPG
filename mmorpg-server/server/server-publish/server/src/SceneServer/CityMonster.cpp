#include "CityMonster.h"
#include <CLSyncObjectProtocol.h>

#include "PlayerMgr.h"
#include "Player.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"

CityMonster::CityMonster()
{
	m_Owner = 0;
	m_SceneId = 0;
	m_Type = CITY_MONSTER_INVALID;
	m_DataId = 0;
	m_DungeonId = 0;
	m_RemainTimes = 0;
	m_TotalTimes = 0;
}

CityMonster::~CityMonster()
{

}

void CityMonster::OnCreated(bool notify)
{
	if (notify)
	{
		CLProtocol::SceneNpcAdd protocol;
		SceneNpcInfo info;
		info.sceneId = GetSceneID();
		info.npcs.push_back(GetInfo());
		protocol.data.push_back(info);

		if (GetOwnerID() > 0)
		{
			// 只同步给玩家自己
			auto owner = PlayerMgr::Instance()->FindNormalPlayer(GetOwnerID());
			if (owner)
			{
				owner->SendProtocol(protocol);
			}
		}
		else
		{
			Router::BroadcastToPlayer(protocol);
		}
	}
	
	Sys::SendUdpLog("city_monster", LOG_CITY_MONSTER_FORMAT, GAME_VERSION.c_str(), ZONE_ID, UInt32(CURRENT_TIME.Sec()), PLATFORM.c_str(), GetID(), GetDataID(), (UInt8)GetMonsterType(), GetDungeonID(), GetSceneID(), GetTotalTimes(), GetOwnerID());
}

void CityMonster::OnKilled(ObjID_t raceId)
{
	if (m_RaceIds.find(raceId) == m_RaceIds.end())
	{
		m_RaceIds.insert(raceId);
		DecRemainTimes();
	}
}

void CityMonster::OnRemoved(bool notify)
{
	if (notify)
	{
		CLProtocol::SceneNpcDel protocol;
		protocol.guids.push_back(GetID());

		if (GetOwnerID() > 0)
		{
			// 只同步给玩家自己
			auto owner = PlayerMgr::Instance()->FindNormalPlayer(GetOwnerID());
			if (owner)
			{
				owner->SendProtocol(protocol);
			}
		}
		else
		{
			Router::BroadcastToPlayer(protocol);
		}
	}
}

std::string CityMonster::ToString()
{
	std::stringstream ss;
	ss << "guid(" << GetID() << ") name(" << GetName() << ") scene(" << GetSceneID() << ") type(" << (UInt32)GetMonsterType() << ") dataId(" << GetDataID() << ") dungeon(" << GetDungeonID() << ") times(" << GetRemainTimes() << ")";

	return ss.str();
}

void CityMonster::DecRemainTimes()
{
	if (GetRemainTimes() > 0)
	{
		SetRemainTimes(GetRemainTimes() - 1);

		if (GetRemainTimes() > 0)
		{
			CLProtocol::SceneNpcUpdate update;
			update.data.sceneId = GetSceneID();
			update.data.npcs.push_back(GetInfo());
			Router::BroadcastToPlayer(update);
		}
	}
}

UInt32 CityMonster::CanFight()
{
	auto remainTimes = GetRemainTimes();
	if (remainTimes == 0)
	{
		return ErrorCode::DUNGEON_START_MONSTER_NOT_EXIST;
	}

	if (GetRacingRaceNum() >= remainTimes)
	{
		return ErrorCode::DUNGEON_START_MONSTER_BUSY;
	}

	return ErrorCode::SUCCESS;
}

void CityMonster::AddRacingRace(ObjID_t raceId)
{
	InfoLogStream << "race(" << raceId << ") start fight with monster(" << ToString() << ")." << LogStream::eos;
	m_RacingRaceList.insert(raceId);
}

void CityMonster::DelRacingRace(ObjID_t raceId)
{
	InfoLogStream << "race(" << raceId << ")  monster(" << ToString() << ") race over." << LogStream::eos;
	m_RacingRaceList.erase(raceId);
}

SceneNpc CityMonster::GetInfo()
{
	SceneNpc npc;
	npc.guid = GetID();
	npc.type = SOT_CITYMONSTER;
	npc.funcType = GetMonsterType();
	npc.id = GetDataID();
	npc.pos = GetPos();
	npc.totalTimes = GetTotalTimes();
	npc.remainTimes = GetRemainTimes();
	npc.dungeonId = GetDungeonID();
	return npc;
}