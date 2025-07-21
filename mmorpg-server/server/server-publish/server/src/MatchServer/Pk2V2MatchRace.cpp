#include "Pk2V2MatchRace.h"
#include "MSRouter.h"


Pk2V2MatchRace::Pk2V2MatchRace(PkType pkType) 
{
}


Pk2V2MatchRace::~Pk2V2MatchRace()
{
}

void Pk2V2MatchRace::CountMatchScoreChanges()
{
	VMatchTeamMember members = GetAllMember();
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		member->SetScoreChange(MATCH_RACE_RESULT_WIN, 100);
		member->SetScoreChange(MATCH_RACE_RESULT_LOSE, 50);
		member->SetScoreChange(MATCH_RACE_RESULT_DRAW, 50);
		member->SetScoreChange(MATCH_RACE_RESULT_ERROR, 0);
	}
}

bool Pk2V2MatchRace::OnRaceEnd(EGameSessionDismissType ret, const void * raceEnd)
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
	endReq.type = PK_2V2_ACTIVITY;
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
		result.info = info;
		result.matchScore = member->GetScore() + member->GetScoreChange(GetRaceResult(info.resultFlag));

		InfoLogStream << "race (" << GetID() << ") player(" << info.roleId << "), flag(" << info.resultFlag << ") damagePercent(" << info.damagePercent
			 << ")" << LogStream::eos;

		endReq.results.push_back(result);
	}

	for (auto id : wsIds)
	{
		MSRouter::Instance()->SendToWorld(id, endReq);
	}
	return true;
}
