#include "DungeonMatchRace.h"
#include "MSRouter.h"

DungeonMatchRace::DungeonMatchRace()
: m_dungeonId(0), m_wsTeamId(0), m_wsId(0), m_isHell(0)
{

}

DungeonMatchRace::~DungeonMatchRace()
{

}

bool DungeonMatchRace::OnCreate(VMatchTeam teams)
{
	CopyMatchTeam(teams);

    if (teams.empty())
    {
        return false;
    }

    auto team = teams[0];
    if (!team)
    {
        return false;
    }

    SetWsID(team->GetWsID());
    SetWsTeamID(team->GetWsTeamID());
    SetDungeonID(team->GetDungeonID());
    SetHell(team->IsHell());
	SetCityMonster(team->GetCityMonster());

    return true;
}

bool DungeonMatchRace::OnRelayServerCreateRaceRet()
{
    return true;
}

bool DungeonMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
{
    InfoLogStream << "race (" << GetID() << ") recv race end, ret(" << ret << ")" << LogStream::eos;

    if (GetState() != MRS_RACE)
    {
        ErrorLogStream << "race (" << GetID() << ") not racing, state(" << GetState() << ")." << LogStream::eos;
        return false;
    }

    DungeonRaceEndInfo* info = (DungeonRaceEndInfo*)raceEnd;
    if (!info)
    {
        return false;
    }

    CLProtocol::MatchServerDungeonRaceEnd req;
    req.teamId = GetWsTeamID();
    req.result = ret;
    req.endInfo = *info;
    MSRouter::Instance()->SendToWorld(GetWsID(), req);

    return true;
}