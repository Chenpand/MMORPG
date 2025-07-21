#include "PracticeMatchRace.h"
#include "MSRouter.h"

PracticeMatchRace::PracticeMatchRace()
{

}

PracticeMatchRace::~PracticeMatchRace()
{

}

bool PracticeMatchRace::OnCreate(VMatchTeam teams)
{
	CopyMatchTeam(teams);
    return true;
}

bool PracticeMatchRace::OnRelayServerCreateRaceRet()
{
    return true;
}

bool PracticeMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
    InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

	PkRaceEndInfo raceEndInfo = *(PkRaceEndInfo*)raceEnd;

	if (GetState() != MRS_RACE)
	{
		ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
		return false;
	}

	for (auto& result : raceEndInfo.results)
	{
		auto member = FindMember(result.roleId);
		if (!member)
		{
			InfoLogStream << "race (" << GetID() << ") can't find member(" << result.roleId << ")." << LogStream::eos;
			continue;
		}

		CLProtocol::MatchServerRaceEnd endReq;
		endReq.type = PK_PRACTICE;
		endReq.result = result;
		endReq.dungeonID = GetDungeonID();
		endReq.usedTime = GetUsedTime();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), endReq);
	}
    return true;
}
