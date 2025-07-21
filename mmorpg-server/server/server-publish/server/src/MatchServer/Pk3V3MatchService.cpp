#include "PK3V3MatchService.h"
#include "Pk3V3MatchAlgorithm.h"

Pk3V3MatchService::Pk3V3MatchService()
:PvPMatchService(false)
{
	SetType(MST_PK_3V3_MATCH);
}

bool Pk3V3MatchService::Init()
{
	auto algorithm = new Pk3V3MatchAlgorithm();
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

bool Pk3V3MatchService::OnTick(const Avalon::Time& now)
{
	return PvPMatchService::OnTick(now);
}

bool Pk3V3MatchService::OnJoinMatchPool(MatchTeam* team)
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

	Match::PK3V3MatchKey key;

	for (auto member : members)
	{
		Match::PK3V3MatchMember matchMember;
		key.wsId = member->GetWsID();
		key.serverCreateTime = member->GetServerCreateTime();
		matchMember.roleId = member->GetID();
		matchMember.level = member->GetLevel();
		matchMember.score = AdjuestMatchScore(member);
		matchMember.seasonLevel = member->GetSeasonLevel();
		auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(member->GetSeasonLevel());
		if (seasonLevelData)
		{
			matchMember.mainLevel = seasonLevelData->mainLevel;
			matchMember.smallLevel = seasonLevelData->smallLevel;
			matchMember.isPromotion = seasonLevelData->isPromotion;
		}

		key.members.push_back(matchMember);
	}

	if (!m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		return false;
	}

	team->SetMatching(true);
	return true;
}

UInt32 Pk3V3MatchService::AdjuestMatchScore(MatchTeamMember* member)
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