#include "TCSTeamPlayerInfo.h"

#include <CLErrorCode.h>
#include <CLGameSessionProtocol.h>
#include "TCSPlayerMgr.h"

TCSTeamPlayerInfo::TCSTeamPlayerInfo()
{
	m_TeamSquadId = 0;
	m_DungeonId = 0;
	m_FieldId = 0;
	m_TailDungeonId = 0;
	m_TimeOut = 0;
}

TCSTeamPlayerInfo::~TCSTeamPlayerInfo()
{

}


void TCSTeamPlayerInfo::AddTeamPlayer(UInt64 playerId)
{
	m_RacePlayerInfoMap[playerId] = DungeonRacePlayerInfo();
}

bool TCSTeamPlayerInfo::IsHasPlayer(UInt64 playerId)
{
	auto iter = m_RacePlayerInfoMap.find(playerId);
	return iter != m_RacePlayerInfoMap.end();
}

void TCSTeamPlayerInfo::SetDungeonRacePlayerInfo(UInt64 playerId, DungeonRacePlayerInfo& info)
{
	m_RacePlayerInfoMap[playerId] = info;
}

bool TCSTeamPlayerInfo::IsAllHasInfo()
{
	UInt32 k = 0;
	for (auto& iter : m_RacePlayerInfoMap)
	{
		if (iter.second.accId != 0)
		{
			++k;
		}
	}

	return k == m_RacePlayerInfoMap.size();
}

UInt32 TCSTeamPlayerInfo::GetPlayerInfos(std::vector<CLProtocol::DungeonFighter>& fighters, std::vector<DungeonRacePlayerInfo>& playerInfos)
{
	UInt32 k = 0;
	for (auto& iter : m_RacePlayerInfoMap)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
		{
			ErrorLogStream << "not find player id:" << iter.first << LogStream::eos;
			return ErrorCode::TEAM_COPY_DATA_ERROR;
		}

		CLProtocol::DungeonFighter fighter;
		fighter.pos = k;
		fighter.accId = player->GetAccID();
		fighter.roleId = player->GetID();
		fighter.wsId = player->GetWorldNodeId();
		fighter.serviceId = NODE_ID;
		fighter.sourceSceneId = player->GetSceneNodeId();
		fighters.push_back(fighter);

		iter.second.raceInfo.pos = k;
		playerInfos.push_back(iter.second);

		k++;
	}

	return ErrorCode::SUCCESS;
}