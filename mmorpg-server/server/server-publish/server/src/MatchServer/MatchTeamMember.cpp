#include "MatchTeamMember.h"

MatchTeamMember::MatchTeamMember()
{
    m_score = 0;
    m_wsId = 0;
    m_hasRaceInfo = false;
    m_seat = 0;
    memset(m_scoreChanges, 0, sizeof(m_scoreChanges));
	m_fightCount = 0;
	m_dailyCount = 0;
	m_winStreak = 0;
	m_loseStreak = 0;
	m_guildId = 0;
	m_terrId = 0;
	m_pkType = PK_NORMAL_1V1;
	m_seasonLevel = 0;
	m_level = 0;
	m_seasonStar = 0;
	SetStartTime(CURRENT_TIME.Sec());
	m_teamId = 0;
	m_seasonExp = 0;
	m_serverCreateTime = 0;
}

MatchTeamMember::~MatchTeamMember()
{

}

MatchTeamMember::MatchTeamMember(const MatchTeamMember& other)
{
	*this = other;
}

MatchTeamMember& MatchTeamMember::operator=(const MatchTeamMember& other)
{
	SetID(other.GetID());
	SetName(other.GetName());
	m_pkType = other.m_pkType;
	m_score = other.m_score;
	m_wsId = other.m_wsId;
	m_guildId = other.m_guildId;
	m_terrId = other.m_terrId;
	m_fightCount = other.m_fightCount;
	m_seat = other.m_seat;
	m_hasRaceInfo = other.m_hasRaceInfo;
	m_raceInfo = other.m_raceInfo;
	m_dungeonInfo = other.m_dungeonInfo;
	memcpy(m_scoreChanges, other.m_scoreChanges, sizeof(m_scoreChanges));
	m_seasonLevel = other.m_seasonLevel;
	m_seasonStar = other.m_seasonStar;
	m_level = other.m_level;
	m_startTime = other.m_startTime;
	m_teamId = other.m_teamId;
	m_seasonExp = other.m_seasonExp;
	m_serverCreateTime = other.m_serverCreateTime;

	return *this;
}

void MatchTeamMember::SetByFigher(CLProtocol::Fighter fighter)
{
    SetID(fighter.roleId);
    m_raceInfo.accid = fighter.accId;
    m_raceInfo.pos = fighter.pos;
}

CLProtocol::Fighter MatchTeamMember::GetFighter() const
{
    CLProtocol::Fighter fighter;
    fighter.roleId = GetID();
    fighter.accId = m_raceInfo.accid;
    fighter.pos = m_raceInfo.pos;
    fighter.wsId = GetWsID();

    return fighter;
}