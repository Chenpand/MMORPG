#include "Pk2V2ActivityMatchService.h"
#include "Pk2V2MatchAlgorithm.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"



Pk2V2ActivityMatchService::Pk2V2ActivityMatchService()
{
	SetType(MST_PK_2V2_ACTIVITY);
}


Pk2V2ActivityMatchService::~Pk2V2ActivityMatchService()
{
}

//bool Pk2V2ActivityMatchService::OnJoinMatchPool(MatchTeam* team)
//{
//	if (!team)
//	{
//		return false;
//	}
//
//	Match::MatchKey key;
//	key.score = team->GetScore();
//	if (m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
//	{
//		team->SetMatching(true);
//		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec());
//	}
//
//	for (auto member : team->GetTeamMembers())
//	{
//		member->SetStartTime(CURRENT_TIME.Sec());
//	}
//
//	return true;
//}

bool Pk2V2ActivityMatchService::Init()
{
	auto algorithm = new Pk2V2MatchAlgorithm();
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

	return true;
}

UInt32 Pk2V2ActivityMatchService::CountTeamMatchScore(MatchTeam * team)
{
	if (!team)
	{
		return 0;
	}

	auto members = team->GetTeamMembers();
	if (members.empty())
	{
		return 0;
	}

	UInt32 totalScore = 0;
	for (auto member : members)
	{
		if (member)
		{
			team->SetLv(member->GetLevel());
			totalScore += member->GetScore();
		}
	}
	team->SetScore(totalScore);
	return team->GetScore();
}


bool Pk2V2ActivityMatchService::OnMatchSuccess(VMatchTeam allTeams)
{
	auto race = CreateRace();
	race->SetID(MatchServiceMgr::Instance()->GenRaceID());

	race->OnCreate(allTeams);

	AddRace(race);

	auto members = race->GetAllMember();

	if (members.size() != 4)
	{
		return false;
	}

	CLProtocol::MatchServerQueryPlayerInfoReq req;
	req.sessionId = race->GetID();
	req.type = GetMatchQueryType();
	UInt8 seat = 0;

	UInt32 index = 0;
	for (auto member : members)
	{
		if (!member)
		{
			return false;
		}

		index++;
		member->SetSeat(seat++);


		req.roleId = member->GetID();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), req);

		CLProtocol::MatchServerPlayerMatchSuccessNotify notify;
		notify.zoneId = CL_GET_ZONEID_FROM_NODEID(member->GetWsID());
		notify.roleId = member->GetID();
		notify.type = member->GetPkType();
		notify.seasonLevel = member->GetSeasonLevel();
		notify.matchScore = member->GetScore();
		notify.usedTime = (UInt32)(CURRENT_TIME.Sec() - member->GetStartTime());

		MSRouter::Instance()->SendToWorld(member->GetWsID(), notify);
	}

	return true;
}

bool Pk2V2ActivityMatchService::OnJoinMatchPool(MatchTeam * team)
{
	if (!team)
	{
		return false;
	}

	Match::PK2V2MatchKey key;
	key.score = team->GetScore();
	key.lv = team->GetLv();
	if (m_matchAlgorithm->AddTeam(team->GetID(), team->GetMemberNum(), key))
	{
		team->SetMatching(true);
		team->SetWaitMatchTime((UInt32)CURRENT_TIME.Sec() + GetWaitMatchUntilRobotSec());
	}

	for (auto member : team->GetTeamMembers())
	{
		member->SetStartTime(CURRENT_TIME.Sec());
	}

	return true;
}

UInt32 Pk2V2ActivityMatchService::AdjuestMatchScore(MatchTeamMember * member)
{
	return UInt32();
}
