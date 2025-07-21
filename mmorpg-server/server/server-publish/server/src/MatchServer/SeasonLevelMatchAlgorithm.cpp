#include "SeasonLevelMatchAlgorithm.h"
#include <CLSeasonDataEntry.h>

SeasonLevelMatchAlgorithm::SeasonLevelMatchAlgorithm()
{
	m_lastUpdateTime = 0;

}

SeasonLevelMatchAlgorithm::~SeasonLevelMatchAlgorithm()
{

}

bool SeasonLevelMatchAlgorithm::Initialize()
{
	return true;
}

void SeasonLevelMatchAlgorithm::OnTick(const Avalon::Time& now)
{

}

bool SeasonLevelMatchAlgorithm::AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key)
{
	Match::SeasonMatchKey& key2 = static_cast<Match::SeasonMatchKey&>(key);

	if (m_matchUnits.find(teamId) != m_matchUnits.end())
	{
		return false;
	}

	GuildLevelMatchUnit* unit = new GuildLevelMatchUnit();
	unit->wsId = key2.wsId;
	unit->serverCreateTime = key2.serverCreateTime;
	unit->roleId = key2.id;
	unit->teamId = teamId;
	unit->level = key2.level;
	unit->blackList = key2.lastOpponentList;
	unit->isWudaodahui = key2.isWudaodahui;
	
	auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(key2.seasonLevelId);
	if (seasonLevelData)
	{
		unit->mainLevel = (SeasonMainLevel)seasonLevelData->mainLevel;
		unit->smallLevel = seasonLevelData->smallLevel;
		unit->isPromotion = seasonLevelData->isPromotion;
	}
	
	unit->matchScore = key2.score;
	unit->startTime = (UInt32)CURRENT_TIME.Sec();

	m_matchUnits[teamId] = unit;

	return true;
}

bool SeasonLevelMatchAlgorithm::DelTeam(ObjID_t teamId)
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

std::vector<Match::MatchTeamResult> SeasonLevelMatchAlgorithm::GetMatchTeamResult()
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

		// �ȴ�ƥ�����ȥ��
		_DelFromPool(unit);
		_DelFromPool(opponent);
	}

	// ɾ������ƥ�䵽����
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

UInt16 SeasonLevelMatchAlgorithm::_GetMatchMaxLevel(UInt16 level)
{
	struct LevelRange
	{
		LevelRange(UInt16 a, UInt16 b) : level(a), maxLevel(b) {}
		UInt16 level;
		UInt16 maxLevel;
	};

	LevelRange s_range[] = {
		{ 15, 24 },
		{ 20, 29 },
		{ 25, 34 },
		{ 30, 39 },
		{ 35, 44 },
		{ 40, 49 },
		{ 45, 54 },
		{ 50, 60 },
		{ 55, 60 },
		{ 60, 65 },
		{ 65, 65 }
	};

	UInt32 size = sizeof(s_range) / sizeof(s_range[0]);
	for (UInt32 i = 0; i < size; i++)
	{
		if (level <= s_range[i].level || i == size - 1)
		{
			return s_range[i].maxLevel;
		}
	}

	return MAX_LEVEL;
}

GuildLevelMatchUnit* SeasonLevelMatchAlgorithm::_FindOpponent(GuildLevelMatchUnit* unit)
{
	if (!unit)
	{
		return NULL;
	}

	std::vector<GuildLevelMatchUnit*> matchpool;

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

	
	GuildLevelMatchUnit* opponent = NULL;
	if (!unit->isPromotion)
	{
		// ��������վ�������һ������
		int index = Avalon::Random::RandBetween(0, matchpool.size() - 1);
		opponent = matchpool[index];
	}
	else
	{
		// ���������վ���Ҷ�λ��ߵ�Ϊ����
		for (auto target : matchpool)
		{
			if (!target)
			{
				continue;
			}

			if (opponent == NULL || target->mainLevel > opponent->mainLevel || (target->mainLevel == opponent->mainLevel && target->smallLevel < opponent->smallLevel))
			{
				opponent = target;
			}
		}
	}
	
	return opponent;
}

bool SeasonLevelMatchAlgorithm::IsMatch(GuildLevelMatchUnit* a, GuildLevelMatchUnit* b)
{
	if (!a || !b || a == b)
	{
		return false;
	}

	// ��һ���Ѿ�ƥ��������
	if (a->dirty || b->dirty)
	{
		return false;
	}

	/*
		ƥ������

		����ƥ�䣺
			1. ƥ������Ƿ��ڿ�ƥ�䷶Χ��
			2. ��λ����(��ͬ��λ��ƥ�䵽��Ŀ���λ��һ��)
			3. ��ɫ�ȼ�����20�����ڣ�
			3. ÿ�첻����ͬ1�����ƥ�䵽N��

		����ս����ƥ�䣺
			1. ����ƥ����Լ���ǰ��λ��1�����λ�������ң�Ѱ��ʱ��Ϊ30�룬��30��������û�к��ʵ�ƥ�����ʱ����ʼѰ��ƥ����Լ���ͬ���λ�Ķ���

		���ƥ��������
			1. 20����ֻƥ���Լ��������
			2. 20-40����ֻƥ�俪��ʱ�����15���ڵ����
			3. 40���ƥ�����з������
	*/

	{
		// ƥ�����
		{
			// �Է������Ƿ����Լ��Ŀ�ƥ�䷶Χ��
			UInt32 diff = _GetMatchScoreDiff(a);
			if (b->matchScore < a->matchScore - diff || b->matchScore > a->matchScore + diff)
			{
				return false;
			}
		}

		{
			// �Լ��Ļ����Ƿ��ڱ��˵Ŀ�ƥ�䷶Χ��
			UInt32 diff = _GetMatchScoreDiff(b);
			if (a->matchScore < b->matchScore - diff || a->matchScore > b->matchScore + diff)
			{
				return false;
			}
		}
	}

	{
		// �ȼ�
		if (b->level > _GetMatchMaxLevel(a->level) || a->level > _GetMatchMaxLevel(b->level))
		{
			return false;
		}
	}

	{
		// ��λ����
		{
			// �Է���λ�Ƿ����Լ��Ŀ�ƥ�䷶Χ��
			SeasonMainLevel minMainLevel, maxMainLevel;
			_GetMatchLevelRange(a, minMainLevel, maxMainLevel);
			if (minMainLevel > b->mainLevel || maxMainLevel < b->mainLevel)
			{
				return false;
			}
		}

		{
			// �Լ���λ�Ƿ��ڶԷ��Ŀ�ƥ�䷶Χ��
			SeasonMainLevel minMainLevel, maxMainLevel;
			_GetMatchLevelRange(b, minMainLevel, maxMainLevel);
			if (minMainLevel > a->mainLevel || maxMainLevel < a->mainLevel)
			{
				return false;
			}
		}
	}

	{
		// ������
		if (std::find(a->blackList.begin(), a->blackList.end(), b->roleId) != a->blackList.end() ||
			std::find(b->blackList.begin(), b->blackList.end(), a->roleId) != b->blackList.end())
		{
			return false;
		}
	}

	{
		// ���ƥ��
		UInt32 sec1 = (UInt32)CURRENT_TIME.Sec() - (UInt32)a->startTime;
		UInt32 sec2 = (UInt32)CURRENT_TIME.Sec() - (UInt32)b->startTime;
		if (sec1 <= 20 || sec2 <= 20)
		{
			if (a->wsId != b->wsId)
			{
				return false;
			}
		}
		else if (sec1 <= 40 || sec2 <= 40)
		{
			UInt32 diffTime = DIFF_TIME(a->serverCreateTime, b->serverCreateTime);
			UInt32 maxDiffTime = 15 * Avalon::Time::SECS_OF_DAY;
			if (diffTime >= maxDiffTime)
			{
				return false;
			}
		}
	}

	return true;
}

UInt32 SeasonLevelMatchAlgorithm::_GetMatchScoreDiff(GuildLevelMatchUnit* unit)
{
	UInt32 initDiff = 50;
	if (!unit)
	{
		return initDiff;
	}

	int sec = (UInt32)CURRENT_TIME.Sec() - unit->startTime;
	if (sec <= 5)
	{
		return initDiff;
	}
	else if (sec <= 30)
	{
		return initDiff + 14 * (sec - 5);
	}
	else if (sec <= 50)
	{
		return 400 + (sec - 30) * 5;
	}
	else
	{
		UInt32 diff = 500 + sec - 50;
		return diff <= 600 ? diff : 600;
	}
}

void SeasonLevelMatchAlgorithm::_GetMatchLevelRange(GuildLevelMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel)
{
	minMainLevel = maxMainLevel = (SeasonMainLevel)0;
	if (!unit)
	{
		return;
	}

	if (!unit->isPromotion)
	{
		// ����ƥ��
		if (unit->mainLevel == SML_BRONZE)
		{
			minMainLevel = SML_BRONZE;
			maxMainLevel = SML_SILVER;
			return;
		}
		else if (unit->mainLevel == SML_SILVER)
		{
			minMainLevel = SML_BRONZE;
			maxMainLevel = SML_GOLD;
			return;
		}
		else if (unit->mainLevel == SML_GOLD)
		{
			minMainLevel = SML_SILVER;
			maxMainLevel = SML_DIAMOND;
			return;
		}
		else
		{
			minMainLevel = SML_GOLD;
			maxMainLevel = SML_KING;
			return;
		}
	}

	/*
		����ս
		30����ֻ��ƥ��һ��λ�ģ�30������ƥͬ��λ���߸�һ��λ
	*/
	UInt32 sec = (UInt32)CURRENT_TIME.Sec() - (UInt32)unit->startTime;
	if (sec <= 30)
	{
		minMainLevel = maxMainLevel = (SeasonMainLevel)(unit->mainLevel+1);
		return;
	}
	else
	{
		minMainLevel = (SeasonMainLevel)unit->mainLevel;
		maxMainLevel = (SeasonMainLevel)(unit->mainLevel + 1);
		return;
	}
	
	return;

}

void SeasonLevelMatchAlgorithm::_DelFromPool(GuildLevelMatchUnit* unit)
{
	/*if (!unit)
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
	}*/
}