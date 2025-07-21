#include "TraditionMatchAlgorithm.h"

TraditionMatchAlgorithm::TraditionMatchAlgorithm()
{
    m_lastUpdateTime = 0;
    for (auto& algorithm : m_algorithms)
    {
        algorithm.Initialize(1, 2, 1, 1);
    }
}

TraditionMatchAlgorithm::~TraditionMatchAlgorithm()
{

}

bool TraditionMatchAlgorithm::Initialize()
{
    return true;
}

bool TraditionMatchAlgorithm::AddPlayerToPool(ObjID_t roleId, UInt32 pkValue)
{
    PkLevelInfo info = _GetPkLevelInfo(pkValue);
    if (info.type >= PKLV_NUM)
    {
        return false;
    }

    return m_algorithms[info.type].AddMatchTeam(roleId, 1, info.level);
}

bool TraditionMatchAlgorithm::RemovePlayer(ObjID_t roleId)
{
    for (auto& algorithm : m_algorithms)
    {
        algorithm.DelMatchTeam(roleId);
    }
    
    return true;
}

void TraditionMatchAlgorithm::Update()
{
    UInt64 curTime = Avalon::Time::CurrentTime().MSec();
    for (auto& algorithm : m_algorithms)
    {
        algorithm.Update(curTime);
    }

    m_lastUpdateTime = curTime;
}

VTraditionMatchResult TraditionMatchAlgorithm::GetMatchResult()
{
    VTraditionMatchResult matchResults;

    for (auto& algorithm : m_algorithms)
    {
        Match::SeMatchAlorithm::VMatchResult& allResult = algorithm.GetAllMatcheResult();
        for (auto& result : allResult)
        {
            TraditionMatchResult matchResult;
            for (auto& unit : result)
            {
                for (auto& roldId : unit->GetAllMatchTeams())
                {
                    matchResult.roleIds.push_back(roldId);
                }
            }

            matchResults.push_back(matchResult);
        }

        algorithm.DelAllMatchResult();
    }

    return matchResults;
}

void TraditionMatchAlgorithm::OnTick(const Avalon::Time& now)
{
    Update();
}

bool TraditionMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
    return AddPlayerToPool(teamId, key.score);
}

bool TraditionMatchAlgorithm::DelTeam(ObjID_t teamId)
{
    return RemovePlayer(teamId);
}

std::vector<Match::MatchTeamResult> TraditionMatchAlgorithm::GetMatchTeamResult()
{
    VTraditionMatchResult tmp = GetMatchResult();

    std::vector<Match::MatchTeamResult> results;
    for (auto& tmpResult : tmp)
    {
        Match::MatchTeamResult result;
        for (auto roleId : tmpResult.roleIds)
        {
            result.teams.push_back(roleId);
        }
        results.push_back(result);
    }

    return results;
}

PkLevelInfo TraditionMatchAlgorithm::_GetPkLevelInfo(UInt32 pkValue)
{
    PkLevelInfo info;
    info.type = PKLV_BRONZE;
    info.level = 0;

    /*for (auto itr = m_pkLevelInfoes.rbegin(); itr != m_pkLevelInfoes.rend(); ++itr)
    {
        if (pkValue >= itr->first)
        {
            return itr->second;
        }
    }*/

    return info;
}
