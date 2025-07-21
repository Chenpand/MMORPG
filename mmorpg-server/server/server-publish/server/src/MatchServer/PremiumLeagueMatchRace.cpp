#include "PremiumLeagueMatchRace.h"
#include "MSRouter.h"

bool PremiumLeagueMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
	InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

	PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

	if (GetState() != MRS_RACE)
	{
		ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
		return false;
	}

	// 结果为空
	if (raceEndInfo.empty())
	{
		ErrorLogStream << "race (" << GetID() << ") result is empty." << LogStream::eos;
		return false;
	}

	CLProtocol::MatchServerPremiumLeagueRaceEnd endReq;
	endReq.raceId = GetID();
	endReq.result = 0;
	endReq.dungeonID = GetDungeonID();
	endReq.usedTime = GetUsedTime();
	UInt32 wsId = 0;
	for (auto& result : raceEndInfo.results)
	{
		auto member = FindMember(result.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << result.roleId << ")." << LogStream::eos;
			continue;
		}

		wsId = member->GetWsID();

		auto resultType = GetRaceResult(result.resultFlag);
		if (resultType == MATCH_RACE_RESULT_WIN)
		{
			endReq.winner.id = member->GetID();
			endReq.winner.name = member->GetName();
			endReq.winner.remainHp = result.remainHp;
			endReq.winner.remainMp = result.remainMp;
		}
		else if (resultType == MATCH_RACE_RESULT_LOSE)
		{
			endReq.loser.id = member->GetID();
			endReq.loser.name = member->GetName();
		}
		else
		{
			endReq.result = resultType == MATCH_RACE_RESULT_DRAW ? resultBitmask_draw : resultBitmask_invalid;

			if (endReq.winner.id == 0)
			{
				endReq.winner.id = member->GetID(); 
				endReq.winner.name = member->GetName();
				endReq.winner.remainHp = result.remainHp;
				endReq.winner.remainMp = result.remainMp;
			}
			else
			{
				endReq.loser.id = member->GetID(); 
				endReq.loser.name = member->GetName();
				endReq.loser.remainHp = result.remainHp;
				endReq.loser.remainMp = result.remainMp;
			}
		}
	}

	// 使用了机器人
	if (m_robot)
	{
		if (endReq.winner.id == 0)
		{
			endReq.winner.name = m_robot->raceInfo.name;
		}
		else
		{
			endReq.loser.name = m_robot->raceInfo.name;
		}
	}

	MSRouter::Instance()->SendToWorld(wsId, endReq);

	return true;
}