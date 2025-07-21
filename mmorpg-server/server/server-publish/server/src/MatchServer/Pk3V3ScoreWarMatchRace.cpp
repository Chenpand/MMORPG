#include "Pk3V3ScoreWarMatchRace.h"
#include "MSRouter.h"
#include <CLRoomDefine.h>

Pk3V3ScoreWarMatchRace::Pk3V3ScoreWarMatchRace(PkType pkType)
{
	m_PkType = pkType;
}

Pk3V3ScoreWarMatchRace::~Pk3V3ScoreWarMatchRace()
{

}

bool Pk3V3ScoreWarMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
	InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

	PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

	if (GetState() != MRS_RACE)
	{
		ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
		return false;
	}

	std::set<UInt32> wsIds;
	CLProtocol::MatchServerPkRoomRaceEnd endReq;
	endReq.raceId = GetID();
	endReq.type = m_PkType;
	endReq.dungeonID = GetDungeonID();
	endReq.usedTime = GetUsedTime();

	for (auto& info : raceEndInfo.results)
	{
		auto member = FindMember(info.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << info.roleId << ")." << LogStream::eos;
			continue;
		}

		wsIds.insert(member->GetWsID());

		Pk3V3PlayerRaceEndInfo result;
		result.roomId = member->GetTeamID();
		result.info = info;
		result.matchScore = member->GetScore();
		result.seasonLevel = member->GetSeasonLevel();
		result.seasonStar = member->GetSeasonStar();
		result.seasonExp = member->GetSeasonExp();
		result.scoreWarBaseScore = _GetBaseScore(GetRaceResult(info.resultFlag));
		result.scoreWarContriScore = _GetContriScore(GetRaceResult(info.resultFlag), info.roleId, info.damagePercent);

		InfoLogStream << "race (" << GetID() << ") player(" << info.roleId << "), flag(" << info.resultFlag << ") damagePercent(" << info.damagePercent
			<< ") baseScore(" << result.scoreWarBaseScore << ") contriScore(" << result.scoreWarContriScore << ")" << LogStream::eos;

		endReq.results.push_back(result);
	}

	for (auto id : wsIds)
	{
		MSRouter::Instance()->SendToWorld(id, endReq);
	}
	return true;
}

UInt32 Pk3V3ScoreWarMatchRace::_GetBaseScore(MatchRaceResult result)
{
	if (result == MATCH_RACE_RESULT_WIN)
	{
		return 100;
	}
	else if (result == MATCH_RACE_RESULT_LOSE)
	{
		return 50;
	}
	else if (result == MATCH_RACE_RESULT_DRAW)
	{
		return 75;
	}
	return 0;
}

UInt32 Pk3V3ScoreWarMatchRace::_GetContriScore(MatchRaceResult result, ObjID_t roleid, UInt32 damagePercent)
{
	if (result == MATCH_RACE_RESULT_ERROR)
	{
		return 0;
	}

	if (damagePercent > 30000)
	{
		ErrorLogStream << "race(" << GetID() << ") gamer(" << roleid << ") too big damage(" << damagePercent << ")." << LogStream::eos;
		damagePercent = 30000;

	}

	UInt32 totalContriScore = 50;
	return (UInt32)(totalContriScore * damagePercent / (10000.0f) / 3.0f);
}