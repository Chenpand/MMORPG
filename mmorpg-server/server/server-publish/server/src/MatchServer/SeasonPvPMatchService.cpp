#include "SeasonPvPMatchService.h"
#include "SeasonLevelMatchAlgorithm.h"
#include "MSRouter.h"

#include <CLErrorCode.h>
#include <CLSeasonDataEntry.h>

SeasonPvPMatchService::SeasonPvPMatchService()
	:PvPMatchService(true)
{
    SetType(MST_SEASON_PVP);
}

bool SeasonPvPMatchService::Init()
{
	auto algorithm = new SeasonLevelMatchAlgorithm();
    if (!algorithm->Initialize())
    {
        return false;
    }

    SetAlgorithm(algorithm);

	class DungeonVistor : public Avalon::DataEntryVisitor<DungeonDataEntry>
	{
	public:
		bool operator()(DungeonDataEntry *dataEntry)
		{
			if (dataEntry && dataEntry->subType == DST_PK && dataEntry->threeType != DTT_CHIJI_PK)
			{
				pkDungeonIdList.push_back(dataEntry->id);
			}
			return true;
		}

		std::vector<UInt32> pkDungeonIdList;
	};

	DungeonVistor vistor;
	DungeonDataEntryMgr::Instance()->Visit(vistor);
	m_pkDungeonIdList = vistor.pkDungeonIdList;

	if (m_pkDungeonIdList.empty())
	{
		ErrorLogStream << "pk dungeon list is empty." << LogStream::eos;
		return false;
	}

	DebugLogStream << "pk dungeon size(" << (UInt32)m_pkDungeonIdList.size() << ")." << LogStream::eos;

    return true;
}

bool SeasonPvPMatchService::OnTick(const Avalon::Time& now)
{
	PvPMatchService::OnTick(now);


	// ���ƥ�䳬ʱ
	std::vector<MatchTeam*> timeoutTeams;
	for (auto itr : m_mapTeams)
	{
		auto team = itr.second;
		if (!team)
		{
			continue;
		}

		if (!team->IsMatching())
		{
			continue;
		}

		/*if (team->GetCreateTime() + MATCH_MAX_WAIT_TIME < (UInt32)now.Sec())
		{
			// ��ʱ
			timeoutTeams.push_back(team);
		}*/
	}

	for (auto team : timeoutTeams)
	{
		for (auto member : team->GetTeamMembers())
		{
			if (!member)
			{
				continue;
			}

			CLProtocol::WorldMatchStartRes res;
			res.result = ErrorCode::MATCH_TIMEOUT;
			MSRouter::Instance()->SendToPlayer(member->GetWsID(), member->GetID(), res);
		}
		

		OnTeamCancelMatch(team->GetID());
	}

	return true;
}

bool SeasonPvPMatchService::OnJoinMatchPool(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	auto members = team->GetTeamMembers();
	if (members.empty())
	{
		return false;
	}

	auto leader = members[0];
	if (!leader)
	{
		return false;
	}

	if (leader && leader->GetFightCount() < 3 && m_useRobot)
	{
		// ǰn��ǿ��ƥ������ˣ�������ƥ���
		UInt32 waitRobotTime = Avalon::Random::RandBetween(2, 5);
		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + waitRobotTime);
		team->SetMatching(true);
		return true;
	}

	Match::SeasonMatchKey key;
	key.wsId = leader->GetWsID();
	key.serverCreateTime = leader->GetServerCreateTime();
	key.id = leader->GetID();
	key.score = AdjuestMatchScore(leader);
	key.seasonLevelId = leader->GetSeasonLevel();
	key.level = leader->GetLevel();
	key.lastOpponentList = GetLastOpponent(leader->GetID());
	key.isWudaodahui = leader->GetPkType() == PK_WUDAODAHUI;

	if (!m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		return false;
	}

	team->SetMatching(true);

	auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(leader->GetSeasonLevel());
	if (seasonLevelData && !seasonLevelData->isRobot)
	{
		team->SetWaitMatchTime(0);
	}
	else
	{
		if (leader->GetloseStreak() >= 3)
		{
			// �������£���ͭ-�ƽ𣩣�����3��֮��5��֮�ڣ�ƥ�䲻���ˣ���ֱ��ƥ�䵽������
			team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + 5);
		}
		else
		{
			team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec(seasonLevelData ? (SeasonMainLevel)seasonLevelData->mainLevel : SML_BRONZE));
		}
	}

	return true;
}

void SeasonPvPMatchService::OnRaceStarted(ObjID_t sessionId)
{
	auto race = FindRace(sessionId);
	if (!race)
	{
		return;
	}

	auto members = race->GetAllMember();
	if (members.size() == 2)
	{
		AddHistory(members[0]->GetID(), members[1]->GetID());
	}
	else if(members.size() == 1)
	{
		AddHistory(members[0]->GetID(), 0);
	}
}

UInt32 SeasonPvPMatchService::AdjuestMatchScore(MatchTeamMember* member)
{
	if (!member)
	{
		return SEASON_MIN_MATCH_SCORE;
	}

#ifdef _ADJUEST_MATCH_ALGORITHM
	/*
		ʤ�����	������ֵ
		��ʤ1		0
		��ʤ2		100
		��ʤ3		150
		��ʤ4		350
		��ʤ5		500
		��ʤ6����	600
		����1		0
		����2		-100
		����3		-150
		����4		-350
		����5		-500
		����6����	-600

	*/

	int score = member->GetScore();

	if (member->GetWinStreak() == 2)
	{
		score += 100;
	}
	else if (member->GetWinStreak() == 3)
	{
		score += 150;
	}
	else if (member->GetWinStreak() == 4)
	{
		score += 350;
	}
	else if (member->GetWinStreak() == 5)
	{
		score += 500;
	}
	else if (member->GetWinStreak() >= 6)
	{
		score += 600;
	}
	else if (member->GetloseStreak() == 2)
	{
		score -= 100;
	}
	else if (member->GetloseStreak() == 3)
	{
		score -= 150;
	}
	else if (member->GetloseStreak() == 4)
	{
		score -= 350;
	}
	else if (member->GetloseStreak() == 5)
	{
		score -= 500;
	}
	else if (member->GetloseStreak() >= 6)
	{
		score -= 600;
	}

	score = (score < (int)SEASON_MIN_MATCH_SCORE) ? (int)SEASON_MIN_MATCH_SCORE : score;
	member->SetScore(score);
	return score;
#else
	return member->GetScore();
#endif
}