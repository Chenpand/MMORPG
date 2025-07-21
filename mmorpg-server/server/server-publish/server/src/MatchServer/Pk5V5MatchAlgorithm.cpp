#include "Pk5V5MatchAlgorithm.h"
#include <CLSystemValueDataEntry.h>

Pk5V5MatchAlgorithm::Pk5V5MatchAlgorithm()
{
	m_WaitingTeamId = 0;
}


Pk5V5MatchAlgorithm::~Pk5V5MatchAlgorithm()
{
}

bool Pk5V5MatchAlgorithm::Initialize()
{
	return true;
}

void Pk5V5MatchAlgorithm::OnTick(const Avalon::Time & now)
{

}

bool Pk5V5MatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey & key)
{
	if (m_WaitingTeamId != 0)
	{
		Match::MatchTeamResult result;
		result.teams.push_back(m_WaitingTeamId);
		result.teams.push_back(teamId);

		m_MatchResults.push_back(result);

		m_WaitingTeamId = 0;
	}
	else
	{
		m_WaitingTeamId = teamId;
	}
	return true;
}

bool Pk5V5MatchAlgorithm::DelTeam(ObjID_t teamId)
{
	return true;
}

std::vector<Match::MatchTeamResult> Pk5V5MatchAlgorithm::GetMatchTeamResult()
{
	auto temp = m_MatchResults;
	m_MatchResults.clear();
	return temp;
}
