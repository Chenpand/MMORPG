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


	// 检测匹配超时
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
			// 超时
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
		// 前n场强制匹配机器人，不加入匹配池
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
			// 铂金以下（青铜-黄金），连输3场之后，5秒之内，匹配不到人，则直接匹配到机器人
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
		胜负情况	修正分值
		连胜1		0
		连胜2		100
		连胜3		150
		连胜4		350
		连胜5		500
		连胜6以上	600
		连负1		0
		连负2		-100
		连负3		-150
		连负4		-350
		连负5		-500
		连负6以上	-600

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