#include "Pk2V2MatchAlgorithm.h"
#include <CLSystemValueDataEntry.h>

const UInt32 MAX_RANDOM_UNIT_SIZE = 100;

Pk2V2MatchAlgorithm::Pk2V2MatchAlgorithm()
{
}


Pk2V2MatchAlgorithm::~Pk2V2MatchAlgorithm()
{
}

bool Pk2V2MatchAlgorithm::Initialize()
{
	waitTimer.SetInterValue(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_MATCH_READY_TIME) * Match::SeTimer::ONE_SECOND, CURRENT_TIME.MSec());
	return true;
}

void Pk2V2MatchAlgorithm::OnTick(const Avalon::Time & now)
{
	waitTimer.Update(CURRENT_TIME.MSec());
	if (waitTimer.IsPassed())
	{
		for (auto &i : m_FindTeammateMap)
		{
			auto &unit = i.second;
			if (CURRENT_TIME.MSec() > unit.lastRangeTime)
			{
				if (unit.rangeScore >= unit.score * 0.5)
				{
					unit.rangeScore = unit.score;
					continue;
				}
				unit.rangeScore += unit.score * 0.1;
				unit.lastRangeTime = CURRENT_TIME.MSec() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_INITIAL_MATCH_TIME) * Match::SeTimer::ONE_SECOND;
				if (unit.rangeScore >= unit.score * 0.5)
				{
					unit.lastRangeTime = CURRENT_TIME.MSec() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_2V2_SCORE_WAR_MIDDLE_MATCH_TIME) * Match::SeTimer::ONE_SECOND;
				}
			}
		}

		for (auto &i : m_FindTeammateMap)
		{
			auto &unitI = i.second;
			std::vector<MatchUnit*> teamVec;
			if (unitI.GetTeamNum() == 1)
			{
				for (auto &j : m_FindTeammateMap)
				{
					auto &unitJ = j.second;
					if (teamVec.size() < 3)
					{
						if (unitJ.GetTeamNum() == 1)
						{
							if (i.first != j.first && unitI.IsInRange(unitJ))
							{
								teamVec.push_back(&unitJ);
							}
						}
					}
					else
					{
						break;
					}
				}
			}
			if (teamVec.size() == 3)
			{
				for (auto &unitJ : teamVec)
				{
					unitI.teamVec.push_back(unitJ->teamVec[0]);
					unitJ->lastRangeTime = CURRENT_TIME.MSec();
					unitJ->teamVec.clear();
				}
			}
		}


		for (auto i = m_FindTeammateMap.begin(); i != m_FindTeammateMap.end(); )
		{
			auto &unit = i->second;
			if (unit.GetTeamNum() == 4)
			{
				Team* maxTeam = nullptr;
				Team* minTeam = nullptr;
				for (auto &team : unit.teamVec)
				{
					if (maxTeam == nullptr)
					{
						maxTeam = &team;
					}
					if (team.score >= maxTeam->score)
					{
						maxTeam = &team;
					}
					if (minTeam == nullptr)
					{
						minTeam = &team;
					}
					if (team.score <= minTeam->score)
					{
						minTeam = &team;
					}
				}
				std::swap(unit.teamVec[0],*maxTeam);
				std::swap(unit.teamVec[1], *minTeam);
				
				Match::MatchTeamResult result;
				for (auto &team : unit.teamVec)
				{
					result.teams.push_back(team.id);
				}
				m_MatchResults.push_back(result);
				i = m_FindTeammateMap.erase(i);
			}
			else if (unit.GetTeamNum() == 0)
			{
				i = m_FindTeammateMap.erase(i);
			}
			else
			{
				++i;
			}
			if (i == m_FindTeammateMap.end())
			{
				break;
			}
		}
	}
}

bool Pk2V2MatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey & key)
{
	auto data = m_FindTeammateMap.find(teamId);
	if (data != m_FindTeammateMap.end())
	{
		return false;
	}
	Team team;
	team.id = teamId;
	team.score = key.score;
	MatchUnit unit(team.score, static_cast<Match::PK2V2MatchKey&>(key).lv);
	unit.teamVec.push_back(team);
	unit.lastRangeTime = CURRENT_TIME.MSec();
	m_FindTeammateMap[teamId] = unit;
	return true;
}

bool Pk2V2MatchAlgorithm::DelTeam(ObjID_t teamId)
{
	DelFromMateMap(teamId);
	return true;
}

std::vector<Match::MatchTeamResult> Pk2V2MatchAlgorithm::GetMatchTeamResult()
{
	auto temp = m_MatchResults;
	m_MatchResults.clear();
	return temp;
}

void Pk2V2MatchAlgorithm::DelFromMateMap(ObjID_t teamId)
{
	auto mateMapIter = m_FindTeammateMap.find(teamId);
	if (mateMapIter != m_FindTeammateMap.end())
	{
		m_FindTeammateMap.erase(mateMapIter);
	}
	for (auto &i : m_FindTeammateMap)
	{
		auto &unit = i.second;
		for (auto j = unit.teamVec.begin(); j != unit.teamVec.end(); ++j)
		{
			if (j->id == teamId)
			{
				unit.teamVec.erase(j);
				break;
			}
		}
	}
}

void Pk2V2MatchAlgorithm::DelFromEnemyMap(ObjID_t teamId)
{
	auto enemyMapIter = m_FindEnemyMap.find(teamId);
	if (enemyMapIter != m_FindEnemyMap.end())
	{
		m_FindEnemyMap.erase(enemyMapIter);
	}
}

MatchUnit::MatchUnit(UInt32 score, UInt32 lv)
{
	this->score = score;
	this->lv = lv;
	UInt32 s_levelRange[] = { 15, 20, 25, 30, 35, 40, 45, 55, 60, 65};
	UInt32 s_maxLevel[] = { 24, 29, 34, 39, 44, 49, 54, 60, 65, 65};
	this->maxLvRange = 0;
	for (UInt32 i = 0; i < sizeof(s_levelRange) / sizeof(s_levelRange[0]); i++)
	{
		if (s_levelRange[i] >= lv)
		{
			this->maxLvRange = s_maxLevel[i];
			break;
		}
	}
}

bool MatchUnit::IsInRange(MatchUnit &unit)
{
	UInt32 lowScore = 0;
	if (this->score <= rangeScore)
	{
		lowScore = 0;
	}
	else
	{
		lowScore = this->score - rangeScore;
	}
	bool boScoreSuit = false;
	if (unit.score >= lowScore && unit.score <= this->score + rangeScore)
	{
		boScoreSuit = true;
	}

	if (!boScoreSuit)
	{
		return false;
	}

	if (this->lv <= unit.maxLvRange && unit.lv <= this->maxLvRange)
	{
		return true;
	}

	return false;
}

UInt32 MatchUnit::GetTeamNum()
{
	return teamVec.size();
}
