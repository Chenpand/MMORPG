#include "SeasonMatchAlgorithm.h"

int SeasonMatchBaseAlorithm::GetMatchScoreDiff(Match::SeMatchUnit* pkUnit)
{
	int initDiff = 50;
	if (!pkUnit)
	{
		return initDiff;
	}

	int sec = pkUnit->GetLoop();
	if (sec <= 5)
	{
		return initDiff;
	}
	else if (sec <= 30) 
	{
		return initDiff + 14 * (sec - 5);
	}
	else if (sec <= 50)
	{
		return 400 + (sec - 30) * 5;
	}
	else
	{
		int diff = 500 + sec - 50;
		return diff <= 600 ? diff : 600;
	}
}

SeasonMatchAlgorithm::SeasonMatchAlgorithm()
{
    m_lastUpdateTime = 0;
    
}

SeasonMatchAlgorithm::~SeasonMatchAlgorithm()
{

}

bool SeasonMatchAlgorithm::Initialize()
{
	return m_algorithm.Initialize(1, 2, 100, 500, 5, 15, 1);
}

void SeasonMatchAlgorithm::OnTick(const Avalon::Time& now)
{
    m_algorithm.Update(now.MSec());
}

bool SeasonMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	Match::SeasonMatchKey& key2 = static_cast<Match::SeasonMatchKey&>(key);

	std::vector<ObjID_t> members;
	members.push_back(key2.id);
	std::vector<ObjID_t> blackList;
	for (auto id : key2.lastOpponentList)
	{
		blackList.push_back(id);
	}
	
	return m_algorithm.AddMatchTeam(teamId, 1, key2.score, members, blackList);
}

bool SeasonMatchAlgorithm::DelTeam(ObjID_t teamId)
{
    return m_algorithm.DelMatchTeam(teamId);
}

std::vector<Match::MatchTeamResult> SeasonMatchAlgorithm::GetMatchTeamResult()
{
    std::vector<Match::MatchTeamResult> results;
    auto& allResult = m_algorithm.GetAllMatcheResult();
    for (auto& result : allResult)
    {
        Match::MatchTeamResult tmpResult;

        for (auto& unit : result)
        {
            for (auto& team : unit->GetAllMatchTeams())
            {
                tmpResult.teams.push_back(team);
            }
        }

        results.push_back(tmpResult);
    }

    m_algorithm.DelAllMatchResult();

    return results;
}