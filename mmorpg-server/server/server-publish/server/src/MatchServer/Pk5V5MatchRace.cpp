#include "Pk5V5MatchRace.h"
#include "MSRouter.h"


Pk5V5MatchRace::Pk5V5MatchRace(PkType pkType) 
{
}


Pk5V5MatchRace::~Pk5V5MatchRace()
{
}


bool Pk5V5MatchRace::OnRaceEnd(EGameSessionDismissType ret, const void * raceEnd)
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
	endReq.type = PK_5V5_MATCH;
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
