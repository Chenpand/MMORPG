#include "ChampionMatchRace.h"
#include "MSRouter.h"

bool ChampionMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void * raceEnd)
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

	CLProtocol::MatchServerChampionRaceEnd endReq;
	auto team = GetAllTeam()[0];
	endReq.groupID = team->GetGroupID();
	endReq.raceId = GetID();
	endReq.result = resultBitmask_win;
	endReq.dungeonID = GetDungeonID();
	endReq.order = team->GetOrderID();
	for (auto& result : raceEndInfo.results)
	{
		auto member = FindMember(result.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << result.roleId << ")." << LogStream::eos;
			continue;
		}

		CLProtocol::MatchServerRaceEnd onPvpReq;
		onPvpReq.type = PK_CHAMPION_BATTLE;
		onPvpReq.opponent = GetOpponentInfo(result.roleId);
		onPvpReq.result = result;
		onPvpReq.dungeonID = GetDungeonID();
		onPvpReq.usedTime = GetUsedTime();
		MSRouter::Instance()->SendToUnion(onPvpReq);

		auto resultType = GetRaceResult(result.resultFlag);
		if (resultType == MATCH_RACE_RESULT_WIN)
		{
			endReq.winner.id = member->GetID();
			endReq.winner.name = member->GetName();
		}
		else if (resultType == MATCH_RACE_RESULT_LOSE)
		{
			//如果loserid已经有了，意味着Relay双输的情况
			if (endReq.loser.id != 0)
			{
				endReq.winner.id = member->GetID();
				endReq.winner.name = member->GetName();
				endReq.result = resultBitmask_draw;
			}
			else
			{
				endReq.loser.id = member->GetID();
				endReq.loser.name = member->GetName();
			}
		}
		else
		{
			//非法结算按平局处理
			endReq.result = resultBitmask_draw;

			if (endReq.winner.id == 0)
			{
				endReq.winner.id = member->GetID();
				endReq.winner.name = member->GetName();
			}
			else
			{
				endReq.loser.id = member->GetID();
				endReq.loser.name = member->GetName();
			}
		}
	}

	MSRouter::Instance()->SendToUnion(endReq);

	return true;
}

void ChampionMatchRace::CountMatchScoreChanges()
{
}
