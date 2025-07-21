#include "GuildBattleMatchService.h"
#include "GuildBattleMatchAlgorithm.h"
#include <CLSeasonDataEntry.h>

GuildBattleMatchService::GuildBattleMatchService()
:PvPMatchService(true)
{
	SetType(MST_GUILD_BATTLE);
}

bool GuildBattleMatchService::Init()
{
	auto algorithm = new GuildBattleMatchAlgorithm();
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

bool GuildBattleMatchService::OnTick(const Avalon::Time& now)
{
	PvPMatchService::OnTick(now);
	return true;
}

bool GuildBattleMatchService::OnJoinMatchPool(MatchTeam* team)
{
	if (!team)
	{
		return false;
	}

	Match::GuildBattleMatchKey key;
	key.guildId = team->GetGuildID();
	key.terrId = team->GetTerrID();
	key.winSteak = team->GetScore();
	if (m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		team->SetMatching(true);

		auto leader = team->GetTeamMembers()[0];
		auto seasonLevelData = SeasonLevelDataEntryMgr::Instance()->FindEntry(leader->GetSeasonLevel());
		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec(seasonLevelData ? (SeasonMainLevel)seasonLevelData->mainLevel : SML_BRONZE));
	}

	return true;
}