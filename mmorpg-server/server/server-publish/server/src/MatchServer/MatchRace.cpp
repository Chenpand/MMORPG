#include "MatchRace.h"

MatchRace::MatchRace()
{
    ChangeState(MRS_WAIT_PLAYER_INFO);
    m_relayServerId = 0;
	m_robot = NULL;
	m_robotAI = ROBOT_AI_INVALID;
	m_robotMatchScore = 0;
	m_robotSeasonLevel = 0;
	m_dungeonID = 0;
	m_createTime = CURRENT_TIME.Sec();
}

MatchRace::~MatchRace()
{
	DebugLogStream << "race(" << GetID() << ") destory." << LogStream::eos;
	for (auto team : m_teams)
	{
		delete team;
	}

	m_teams.clear();
}

void MatchRace::SetRelayServerID(UInt32 id)
{
	m_relayServerId = id;

	DebugLogStream << "race(" << GetID() << ") alloc relayserver(" << id << ")." << LogStream::eos;
}

VMatchTeamMember MatchRace::GetAllMember() const
{
    VMatchTeamMember members;
    for (auto team : m_teams)
    {
        if (!team)
        {
            continue;
        }

        for (auto member : team->GetTeamMembers())
        {
            if (!member)
            {
                continue;
            }

            members.push_back(member);
        }
    }

    return members;
}

MatchTeamMember* MatchRace::FindMember(ObjID_t id) const
{
    for (auto team : m_teams)
    {
        if (!team)
        {
            continue;
        }

        auto member = team->FindMember(id);
        if (member)
        {
            return member;
        }
    }

    return NULL;
}

bool MatchRace::IsAllPlayerHasRaceInfo() const
{
    VMatchTeamMember members = GetAllMember();
    for (auto member : members)
    {
        if(!member)
        {
            continue;
        }

        if (!member->HasRaceInfo())
        {
            return false;
        }
    }

    return true;
}

void MatchRace::CopyMatchTeam(VMatchTeam teams)
{
	for (auto oteam : teams)
	{
		auto team = new MatchTeam(*oteam);
		m_teams.push_back(team);
	}
}
