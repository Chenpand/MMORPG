#include "MatchService.h"
#include "MatchServiceMgr.h"

MatchService::MatchService(MatchServiceType type)
    : m_type(type)
{
    m_matchAlgorithm = NULL;
	m_nodeid = 0;
}

MatchService::~MatchService()
{
    for (auto itr : m_mapTeams)
    {
        if (itr.second)
        {
            delete itr.second;
        }
    }

    m_mapTeams.clear();

    if (m_matchAlgorithm)
    {
        delete m_matchAlgorithm;
        m_matchAlgorithm = NULL;
    }
}

bool MatchService::OnTick(const Avalon::Time& now)
{
    if (m_matchAlgorithm)
    {
        m_matchAlgorithm->OnTick(now);

        std::vector<Match::MatchTeamResult> matchResults = m_matchAlgorithm->GetMatchTeamResult();
        for (auto& result : matchResults)
        {
            std::vector<MatchTeam*> teams;

            for (auto teamId : result.teams)
            {
                MatchTeam* team = FindTeam(teamId);
                if (team)
                {
                    teams.push_back(team);
					team->SetMatching(false);
                }
            }

            if (!OnMatchSuccess(teams))
            {
                // 开始战斗失败，重新放回匹配池
                for (auto team : teams)
                {
                    if (!team)
                    {
                        continue;
                    }

                    OnJoinMatchPool(team);
                }
            }
        }
    }

	std::vector<MatchRace*> dismissRaces;
	for (auto itr : m_mapRaces)
	{
		auto race = itr.second;
		if (!race)
		{
			continue;
		}

		if (race->GetState() == MRS_WAITING_RELAY_SERVER_RES && 
			now.Sec() >= WAIT_RELAY_SERVER_MAX_TIME + race->GetStatusTime().Sec())
		{
			DebugLogStream << "race(" << race->GetID() << ") wait relay server timeout." << LogStream::eos;
			dismissRaces.push_back(race);
		}
		else if (race->GetState() == MRS_WAIT_PLAYER_INFO &&
			now.Sec() >= WAIT_PLAYER_INFO_MAX_TIME + race->GetStatusTime().Sec())
		{
			DebugLogStream << "race(" << race->GetID() << ") wait player info timeout." << LogStream::eos;
			dismissRaces.push_back(race);
		}
	}

	for (auto race : dismissRaces)
	{
		DismissMatchRace(race, true);
	}

    return true;
}

void MatchService::OnRaceEnd(ObjID_t gamesessionID, EGameSessionDismissType ret, const void* raceEnd)
{
    auto race = FindRace(gamesessionID);
    if (!race)
    {
        return;
    }

    if (race->OnRaceEnd(ret, raceEnd))
    {
        // 解散队伍，把队伍中的玩家都删掉
        DismissMatchRace(race, false);
    }
}

UInt32 MatchService::CountTeamMatchScore(MatchTeam* team)
{
    if (!team)
    {
        return 0;
    }

    auto members = team->GetTeamMembers();
    if (members.empty())
    {
        return 0;
    }

    UInt32 totalScore = 0;
    for (auto member : members)
    {
        if (member)
        {
            totalScore += member->GetScore();
        }
    }

    team->SetScore(totalScore / members.size());
    return team->GetScore();
}

bool MatchService::IsPkService()
{
	return GetType() >= MST_RACE_BEGIN && GetType() <= MST_RACE_END;
}

bool MatchService::IsDungeonService()
{
	return GetType() >= MST_DUNGEON_BEGIN && GetType() <= MST_DUNGEON_END;
}

bool MatchService::AddTeam(MatchTeam* team)
{
    if (!team)
    {
        return false;
    }

    if (FindTeam(team->GetID()))
    {
        ErrorLogStream << "repeat add team(" << team->GetID() << ")." << LogStream::eos;
        return false;
    }

    m_mapTeams[team->GetID()] = team;
    return true;
}

MatchTeam* MatchService::FindTeam(ObjID_t id)
{
    auto itr = m_mapTeams.find(id);
    return itr == m_mapTeams.end() ? NULL : itr->second;
}

void MatchService::DelTeam(ObjID_t id)
{
    if (m_matchAlgorithm)
    {
        m_matchAlgorithm->DelTeam(id);
    }

    auto team = FindTeam(id);
    if (!team)
    {
        return;
    }

    // 删除MatchServiceMgr中的玩家信息，如果这个玩家还在这个队伍中
    for (auto member : team->GetTeamMembers())
    {
        if (!member)
        {
            continue;
        }

        auto player = MatchServiceMgr::Instance()->FindPlayer(member->GetID());
        if (player && player->GetTeamID() == id)
        {
            MatchServiceMgr::Instance()->DelPlayer(member->GetID());
        }
    }

    m_mapTeams.erase(id);
    delete team;
}

bool MatchService::AddRace(MatchRace* race)
{
    if (!race)
    {
        return false;
    }

    if (FindRace(race->GetID()))
    {
        ErrorLogStream << "repeat add race(" << race->GetID() << ")." << LogStream::eos;
        return false;
    }

    m_mapRaces[race->GetID()] = race;
    return true;
}

MatchRace* MatchService::FindRace(ObjID_t id)
{
    auto itr = m_mapRaces.find(id);
    return itr == m_mapRaces.end() ? NULL : itr->second;
}

void MatchService::DelRace(ObjID_t id)
{
    auto race = FindRace(id);
    if (race)
    {
        m_mapRaces.erase(id);
        delete race;
    }
}

void MatchService::Report()
{
	InfoLogStream << "service(" << GetNodeID() << ", " << (UInt8)GetType() << ") current race(" << (UInt32)m_mapRaces.size() << ") team(" << (UInt32)m_mapTeams.size() << ")." << LogStream::eos;
}

void MatchService::DismissMatchRace(const MatchRace* race, bool rematch)
{
    if (!race)
    {
        return;
    }

    for (auto team : race->GetAllTeam())
    {
        if (!team)
        {
            continue;
        }

        if (!rematch)
        {
            InfoLogStream << "race (" << race->GetID() << ") dismissed, team(" << team->GetID() << ") deleted." << LogStream::eos;

            DelTeam(team->GetID());
            continue;
        }

        InfoLogStream << "race (" << race->GetID() << ") dismissed, team(" << team->GetID() << ") restart match." << LogStream::eos;

        OnJoinMatchPool(team);
    }

    DelRace(race->GetID());
}