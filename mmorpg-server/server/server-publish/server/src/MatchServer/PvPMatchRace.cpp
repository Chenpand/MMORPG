#include "PvPMatchRace.h"
#include "MSRouter.h"

PvPMatchRace::PvPMatchRace()
{
}

PvPMatchRace::~PvPMatchRace()
{

}

bool PvPMatchRace::OnCreate(VMatchTeam teams)
{
	CopyMatchTeam(teams);
    CountMatchScoreChanges();
    return true;
}

bool PvPMatchRace::OnRelayServerCreateRaceRet()
{
    return true;
}

bool PvPMatchRace::OnRaceEnd(EGameSessionDismissType ret, const void* raceEnd)
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
        endReq.type = PK_NORMAL_1V1;
        endReq.opponent = GetOpponentInfo(result.roleId);
        endReq.result = result;
		endReq.dungeonID = GetDungeonID();
		endReq.usedTime = GetUsedTime();
        MSRouter::Instance()->SendToWorld(member->GetWsID(), endReq);
    }
    return true;
}

void PvPMatchRace::CountMatchScoreChanges()
{
    VMatchTeamMember members = GetAllMember();
    for (auto member : members)
    {
        if (!member)
        {
            continue;
        }

        auto opponent = GetOpponent(member->GetID());
        if (!opponent)
        {
            continue;
        }

        member->SetScoreChange(MATCH_RACE_RESULT_WIN, 1000);
        member->SetScoreChange(MATCH_RACE_RESULT_LOSE, -500);
        member->SetScoreChange(MATCH_RACE_RESULT_DRAW, 100);
        member->SetScoreChange(MATCH_RACE_RESULT_ERROR, 0);
    }
}

UInt32 PvPMatchRace::GenRobotMatchScore(UInt32 matchScore)
{
	int minScore = matchScore - 100;
	minScore = minScore < 1000 ? 1000 : minScore;
	minScore = minScore > 1800 ? 1800 : minScore;
	int maxScore = matchScore + 100;
	maxScore = maxScore > 1800 ? 1800 : maxScore;
	minScore = minScore < 1000 ? 1000 : minScore;
	return (UInt32)Avalon::Random::RandBetween(minScore, maxScore);
}

MatchRaceResult PvPMatchRace::GetRaceResult(UInt32 result)
{
    if (result & resultBitmask_win)
    {
        return MATCH_RACE_RESULT_WIN;
    }
    else if (result & resultBitmask_lose)
    {
        return MATCH_RACE_RESULT_LOSE;
    }
    else if (result & resultBitmask_draw)
    {
        return MATCH_RACE_RESULT_DRAW;
    }

    return MATCH_RACE_RESULT_ERROR;
}

MatchTeamMember* PvPMatchRace::GetOpponent(ObjID_t roleId)
{
    VMatchTeamMember members = GetAllMember();
    if (members.size() != 2)
    {
        return NULL;
    }

    for (auto member : members)
    {
        if (member && member->GetID() != roleId)
        {
            return member;
        }
    }

	return NULL;
}

CLProtocol::MatchOpponent PvPMatchRace::GetOpponentInfo(ObjID_t roleId)
{
    CLProtocol::MatchOpponent opponent;
    auto member = GetOpponent(roleId);
    if (!member)
    {
		if (m_robot)
		{
			opponent.name = m_robot->raceInfo.name;
			opponent.level = m_robot->raceInfo.level;
			opponent.occu = m_robot->raceInfo.occu;
			opponent.seasonLevel = m_robotSeasonLevel;
		}
        return opponent;
    }

	opponent.zoneId = member->GetRaceInfo().zoneId;
    opponent.roleId = member->GetID();
	opponent.name = member->GetName();
	opponent.level = member->GetLevel();
    opponent.occu = member->GetRaceInfo().occu;
	opponent.seasonLevel = member->GetSeasonLevel();
	opponent.seasonStars = member->GetRaceInfo().seasonStar;
    
    return opponent;
}