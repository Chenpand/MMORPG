#include "PremiumLeaguePreliminayMatchAlgorithm.h"
#include <CLSeasonDataEntry.h>

PremiumLeaguePreliminayMatchAlgorithm::PremiumLeaguePreliminayMatchAlgorithm()
{
	m_lastUpdateTime = 0;

}

PremiumLeaguePreliminayMatchAlgorithm::~PremiumLeaguePreliminayMatchAlgorithm()
{

}

bool PremiumLeaguePreliminayMatchAlgorithm::Initialize()
{
	return true;
}

void PremiumLeaguePreliminayMatchAlgorithm::OnTick(const Avalon::Time& now)
{

}

bool PremiumLeaguePreliminayMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	if (m_matchUnits.find(teamId) != m_matchUnits.end())
	{
		return false;
	}

	Match::PremiumLeaguePreliminayMatchKey& key2 = static_cast<Match::PremiumLeaguePreliminayMatchKey&>(key);

	PremiumLeaguePreliminayMatchUnit* unit = new PremiumLeaguePreliminayMatchUnit();
	unit->teamId = teamId;
	unit->winSteak = key2.winSteak;
	unit->startTime = (UInt32)CURRENT_TIME.Sec();
	auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(key2.seasonLevelId);
	if (seasonLevelData)
	{
		unit->mainLevel = (SeasonMainLevel)seasonLevelData->mainLevel;
	}

	m_matchUnits[teamId] = unit;

	return true;
}

bool PremiumLeaguePreliminayMatchAlgorithm::DelTeam(ObjID_t teamId)
{
	auto itr = m_matchUnits.find(teamId);
	if (itr == m_matchUnits.end())
	{
		return false;
	}

	auto unit = itr->second;
	m_matchUnits.erase(itr);
	_DelFromPool(unit);
	if (unit)
	{
		delete unit;
	}

	return true;
}

std::vector<Match::MatchTeamResult> PremiumLeaguePreliminayMatchAlgorithm::GetMatchTeamResult()
{
	std::vector<Match::MatchTeamResult> results;
	std::vector<ObjID_t> matchedTeams;

	for (auto itr : m_matchUnits)
	{
		auto unit = itr.second;
		if (!unit || unit->dirty)
		{
			continue;
		}

		auto opponent = _FindOpponent(unit);
		if (!opponent)
		{
			continue;
		}

		unit->dirty = true;
		opponent->dirty = true;

		Match::MatchTeamResult result;
		result.teams.push_back(unit->teamId);
		result.teams.push_back(opponent->teamId);
		results.push_back(result);

		matchedTeams.push_back(unit->teamId);
		matchedTeams.push_back(opponent->teamId);

		// 先从匹配池中去掉
		_DelFromPool(unit);
		_DelFromPool(opponent);
	}

	// 删除所有匹配到的人
	for (auto teamId : matchedTeams)
	{
		auto itr = m_matchUnits.find(teamId);
		if (itr != m_matchUnits.end())
		{
			delete itr->second;
			m_matchUnits.erase(itr);
		}
	}

	return results;
}

PremiumLeaguePreliminayMatchUnit* PremiumLeaguePreliminayMatchAlgorithm::_FindOpponent(PremiumLeaguePreliminayMatchUnit* unit)
{
	if (!unit)
	{
		return NULL;
	}

	std::vector<PremiumLeaguePreliminayMatchUnit*> matchpool;
	for (auto itr : m_matchUnits)
	{
		auto other = itr.second;
		if (IsMatch(unit, other))
		{
			matchpool.push_back(other);
		}
	}

	if (matchpool.empty())
	{
		return NULL;
	}

	int index = Avalon::Random::RandBetween(0, matchpool.size() - 1);
	return matchpool[index];
}

bool PremiumLeaguePreliminayMatchAlgorithm::IsMatch(PremiumLeaguePreliminayMatchUnit* a, PremiumLeaguePreliminayMatchUnit* b)
{
	if (!a || !b || a == b)
	{
		return false;
	}

	// 有一方已经匹到对手了
	if (a->dirty || b->dirty)
	{
		return false;
	}

	UInt32 passSecA = (UInt32)CURRENT_TIME.Sec() - a->startTime;
	UInt32 passSecB = (UInt32)CURRENT_TIME.Sec() - b->startTime;

	// 判断双方连胜数是否满足条件
	{
		if (!IsMatchWinSteak(passSecA, a->winSteak, b->winSteak) || !IsMatchWinSteak(passSecB, b->winSteak, a->winSteak))
		{
			return false;
		}
	}

	// 判断双方段位数是否满足条件
	{
		if (!IsMatchSeasonLevel(passSecA, a->mainLevel, b->mainLevel) || !IsMatchSeasonLevel(passSecB, b->mainLevel, a->mainLevel))
		{
			return false;
		}
	}
	return true;
}

void PremiumLeaguePreliminayMatchAlgorithm::_DelFromPool(PremiumLeaguePreliminayMatchUnit* unit)
{
	if (!unit)
	{
		return;
	}
}

bool PremiumLeaguePreliminayMatchAlgorithm::IsMatchSeasonLevel(UInt32 sec, SeasonMainLevel level1, SeasonMainLevel level2)
{
	if (sec <= 10)
	{
		return level1 == level2;
	}
	else if (sec <= 20)
	{
		return true;
	}
	else if (sec <= 30)
	{
		return abs((int)level1 - (int)level2) <= 1;
	}
	else if (sec <= 40)
	{
		return true;
	}

	return true;
}

bool PremiumLeaguePreliminayMatchAlgorithm::IsMatchWinSteak(UInt32 sec, UInt32 num1, UInt32 num2)
{
	if (sec <= 10)
	{
		return true;
	}
	else if (sec <= 20)
	{
		return num1 == num2;
	}
	else if (sec <= 30)
	{
		return true;
	}
	else if (sec <= 40)
	{
		return abs((int)num1 - (int)num2) <= 2;
	}

	return true;
}