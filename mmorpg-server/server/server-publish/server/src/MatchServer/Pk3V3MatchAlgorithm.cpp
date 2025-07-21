#include "Pk3V3MatchAlgorithm.h"
#include <sstream>

Pk3V3MatchAlgorithm::Pk3V3MatchAlgorithm()
{
	m_lastUpdateTime = 0;
#ifdef _DEBUG
	m_algorithms.Initialize(3, 2);
#else
	m_algorithms.Initialize(3, 2);
#endif
}

Pk3V3MatchAlgorithm::~Pk3V3MatchAlgorithm()
{

}

bool Pk3V3MatchAlgorithm::Initialize()
{
	return true;
}

VPk3V3MatchResult Pk3V3MatchAlgorithm::GetMatchResult()
{
	
	VPk3V3MatchResult matchResults;
	auto& allResult = m_algorithms.GetAllMatcheResult();
	for (auto& result : allResult)
	{
		std::stringstream ss;
		Pk3V3MatchResult matchResult;
		for (auto& unit : result)
		{
			ss << " " << unit->GetMatchUnitID() << "(";
			for (auto& roomId : unit->GetAllMatchTeams())
			{
				matchResult.roomIds.push_back(roomId);
			}

			for (auto& roomId : unit->GetAllMatchTeams())
			{
				ss << roomId << ", ";
			}

			ss << ")";
		}

		DebugLogStream << "match result:" << ss.str() << "." << LogStream::eos;
		matchResults.push_back(matchResult);
	}

	m_algorithms.DelAllMatchResult();
	return matchResults;
}

void Pk3V3MatchAlgorithm::OnTick(const Avalon::Time& now)
{
	m_algorithms.Update(CURRENT_TIME.MSec());
}

bool Pk3V3MatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	auto& key2 = static_cast<Match::PK3V3MatchKey&>(key);
	for (auto member : key2.members)
	{
		DebugLogStream << "[3v3] team(" << teamId << ") member(" << member.roleId << ") add to match." << LogStream::eos;
	}

	return m_algorithms.AddMatchTeam(key2.wsId, key2.serverCreateTime, teamId, playerNum, key2.members);
}

bool Pk3V3MatchAlgorithm::DelTeam(ObjID_t teamId)
{
	return m_algorithms.DelMatchTeam(teamId);
}

std::vector<Match::MatchTeamResult> Pk3V3MatchAlgorithm::GetMatchTeamResult()
{
	VPk3V3MatchResult tmp = GetMatchResult();

	std::vector<Match::MatchTeamResult> results;
	for (auto& tmpResult : tmp)
	{
		Match::MatchTeamResult result;
		for (auto roomId : tmpResult.roomIds)
		{
			result.teams.push_back(roomId);
		}
		results.push_back(result);
	}

	return results;
}