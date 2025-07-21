#include "EuqalPvPMatchRace.h"
#include "MSRouter.h"


EuqalPvPMatchRace::EuqalPvPMatchRace()
{
}


EuqalPvPMatchRace::~EuqalPvPMatchRace()
{
}


bool EuqalPvPMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
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
		endReq.type = PK_EQUAL_1V1;
		endReq.opponent = GetOpponentInfo(result.roleId);
		endReq.result = result;
		endReq.dungeonID = GetDungeonID();
		endReq.usedTime = GetUsedTime();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), endReq);
	}
	return true;
}

EuqalPracticeMatchRace::EuqalPracticeMatchRace()
{

}

EuqalPracticeMatchRace::~EuqalPracticeMatchRace()
{

}

bool EuqalPracticeMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
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
		endReq.type = PK_EQUAL_PRACTICE;
		endReq.result = result;
		endReq.dungeonID = GetDungeonID();
		endReq.usedTime = GetUsedTime();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), endReq);
	}
	return true;
}