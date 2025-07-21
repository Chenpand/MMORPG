#include "PremiumLeaguePreliminayMatchService.h"
#include "PremiumLeaguePreliminayMatchAlgorithm.h"

PremiumLeaguePreliminayMatchService::PremiumLeaguePreliminayMatchService()
:PvPMatchService(false)
{
	SetType(MST_PREMIUM_LEAGUE_PRELIMINAY);
}

bool PremiumLeaguePreliminayMatchService::Init()
{
	auto algorithm = new PremiumLeaguePreliminayMatchAlgorithm();
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

	return true;
}

bool PremiumLeaguePreliminayMatchService::OnTick(const Avalon::Time& now)
{
	PvPMatchService::OnTick(now);
	return true;
}

bool PremiumLeaguePreliminayMatchService::OnJoinMatchPool(MatchTeam* team)
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

	Match::PremiumLeaguePreliminayMatchKey key;
	key.winSteak = leader->GetWinStreak();
	key.seasonLevelId = leader->GetSeasonLevel();
	if (m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		team->SetMatching(true);
		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec());
	}

	return true;
}