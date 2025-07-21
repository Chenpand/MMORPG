#include "GuildBattleMatchAlgorithm.h"

GuildBattleMatchAlgorithm::GuildBattleMatchAlgorithm()
{
	m_lastUpdateTime = 0;

}

GuildBattleMatchAlgorithm::~GuildBattleMatchAlgorithm()
{

}

bool GuildBattleMatchAlgorithm::Initialize()
{
	return true;
}

void GuildBattleMatchAlgorithm::OnTick(const Avalon::Time& now)
{
	
}

bool GuildBattleMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	if (m_matchUnits.find(teamId) != m_matchUnits.end())
	{
		return false;
	}

	Match::GuildBattleMatchKey& key2 = static_cast<Match::GuildBattleMatchKey&>(key);

	GuildBattleMatchUnit* unit = new GuildBattleMatchUnit();
	unit->teamId = teamId;
	unit->guildId = key2.guildId;
	unit->terrId = key2.terrId;
	unit->winSteak = key2.winSteak > GUILD_BATTLE_MATCH_POOL_MAX_WINSTEAK ? GUILD_BATTLE_MATCH_POOL_MAX_WINSTEAK : key2.winSteak;
	unit->startTime = (UInt32)CURRENT_TIME.Sec();

	m_matchUnits[teamId] = unit;
	
	auto itr = m_matchPool.find(unit->terrId);
	if (itr == m_matchPool.end())
	{
		m_matchPool[unit->terrId].push_back(unit);
	}
	else
	{
		itr->second.push_back(unit);
	}

	return true;
}

bool GuildBattleMatchAlgorithm::DelTeam(ObjID_t teamId)
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

std::vector<Match::MatchTeamResult> GuildBattleMatchAlgorithm::GetMatchTeamResult()
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

GuildBattleMatchUnit* GuildBattleMatchAlgorithm::_FindOpponent(GuildBattleMatchUnit* unit)
{
	if (!unit)
	{
		return NULL;
	}

	std::vector<GuildBattleMatchUnit*> matchpool;
	

	auto itr = m_matchPool.find(unit->terrId);
	if (itr == m_matchPool.end())
	{
		return NULL;
	}

	for (auto other : itr->second)
	{
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

void GuildBattleMatchAlgorithm::GetMatchRange(GuildBattleMatchUnit* unit, UInt32& begin, UInt32& end)
{
	if (!unit)
	{
		begin = 0;
		end = 0;
		return;
	}

	begin = unit->winSteak;
	end = unit->winSteak;

	UInt32 passSec = (UInt32)CURRENT_TIME.Sec() - unit->startTime;
	UInt32 rangeBySec[] = { 0, 5, 10, 15, 20, 25 };
	for (UInt32 i = sizeof(rangeBySec) / sizeof(rangeBySec[0]) - 1; i >= 0; i--)
	{
		if (passSec < rangeBySec[i])
		{
			continue;
		}

		if (i > unit->winSteak)
		{
			begin = 0;
		}
		else
		{
			begin = unit->winSteak - i;
		}

		if (i + unit->winSteak > GUILD_BATTLE_MATCH_POOL_MAX_WINSTEAK)
		{
			end = GUILD_BATTLE_MATCH_POOL_MAX_WINSTEAK;
		}
		else
		{
			end = unit->winSteak + i;
		}

		return;
	}
}

bool GuildBattleMatchAlgorithm::IsMatch(GuildBattleMatchUnit* a, GuildBattleMatchUnit* b)
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

	// 同一公会的不能匹在一起
	if (a->guildId == b->guildId)
	{
		return false;
	}

	// 判断双方连胜数是否满足条件
	{
		UInt32 minWinSteak = 0;
		UInt32 maxWinSteak = 0;
		GetMatchRange(a, minWinSteak, maxWinSteak);
		if (b->winSteak < minWinSteak || b->winSteak > maxWinSteak)
		{
			return false;
		}
	}

	{
		UInt32 minWinSteak = 0;
		UInt32 maxWinSteak = 0;
		GetMatchRange(b, minWinSteak, maxWinSteak);
		if (a->winSteak < minWinSteak || a->winSteak > maxWinSteak)
		{
			return false;
		}
	}

	return true;
}

void GuildBattleMatchAlgorithm::_DelFromPool(GuildBattleMatchUnit* unit)
{
	if (!unit)
	{
		return;
	}

	auto itr = m_matchPool.find(unit->terrId);
	if (itr == m_matchPool.end())
	{
		return;
	}

	auto& terrPool = itr->second;
	for (UInt32 i = 0; i < terrPool.size(); i++)
	{
		if (terrPool[i] == unit)
		{
			terrPool.erase(terrPool.begin() + i);
			break;;
		}
	}

	if (terrPool.empty())
	{
		m_matchPool.erase(itr);
	}
}