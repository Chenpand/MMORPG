#include "MatchTeam.h"

MatchTeam::MatchTeam()
{
    m_score = 0;
    m_raceId = 0;
    m_wsTeamId = 0;
    m_wsId = 0;
    m_dungeonId = 0;
    m_isHell = 0;
	m_isMatching = false;
	m_guildId = 0;
	m_terrId = 0;
	m_waitMatchTime = 0;
	m_createTime = (UInt32)CURRENT_TIME.Sec();
	m_premiumLeagueBattleId = 0;
	m_OrderID = 0;
	m_GroupID = 0;
}

MatchTeam::MatchTeam(const MatchTeam& other)
{
	*this = other;
}

MatchTeam& MatchTeam::operator=(const MatchTeam& other)
{
	SetID(other.GetID());
	SetName(other.GetName());
	for (auto omember : other.m_members)
	{
		auto member = new MatchTeamMember(*omember);
		m_members.push_back(member);
	}

	m_score = other.m_score;
	m_raceId = other.m_raceId;
	m_wsId = other.m_wsId;
	m_wsTeamId = other.m_wsTeamId;
	m_dungeonId = other.m_dungeonId;
	m_isHell = other.m_isHell;
	m_isMatching = other.m_isMatching;
	m_guildId = other.m_guildId;
	m_terrId = other.m_terrId;
	m_waitMatchTime = other.m_waitMatchTime;
	m_premiumLeagueBattleId = other.m_premiumLeagueBattleId;
	m_cityMonster = other.m_cityMonster;
	m_GroupID = other.m_GroupID;
	m_OrderID = other.m_OrderID;

	return *this;
}

MatchTeam::~MatchTeam()
{
    for (UInt32 i = 0; i < m_members.size(); i++)
    {
        if (m_members[i])
        {
            delete m_members[i];
        }
    }
}

void MatchTeam::AddMember(MatchTeamMember* member)
{
    if (!member)
    {
        return;
    }

    m_members.push_back(member);
}

void MatchTeam::DelMember(ObjID_t id)
{
    for (UInt32 i = 0; i < m_members.size(); i++)
    {
        auto member = m_members[i];
        if (!member)
        {
            continue;
        }

        if (member->GetID() == id)
        {
            m_members.erase(m_members.begin() + i);
        }
    }
}

MatchTeamMember* MatchTeam::FindMember(ObjID_t id) const
{
    for (auto member : m_members)
    {
        if (member && member->GetID() == id)
        {
            return member;
        }
    }

    return NULL;
}
