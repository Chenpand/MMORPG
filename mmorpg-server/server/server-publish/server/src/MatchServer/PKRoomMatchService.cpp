#include "PKRoomMatchService.h"
#include <CLErrorCode.h>
#include <CLRelayServerProtocol.h>

#include "MSRouter.h"
#include "MatchServiceMgr.h"
#include "RelaySessionMgr.h"
#include "SeasonLevelMatchAlgorithm.h"
#include "MSApplication.h"

PkRoomMatchService::PkRoomMatchService()
:PvPMatchService(false)
{
	SetType(MST_PK_ROOM);
}

PkRoomMatchService::~PkRoomMatchService()
{

}

bool PkRoomMatchService::Init()
{
	SetAlgorithm(NULL);

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

bool PkRoomMatchService::OnTeamStartMatch(MatchTeam* team)
{
	if (!AddTeam(team))
	{
		return false;
	}

	DebugLogStream << "team(" << team->GetID() << ") start dungeon match." << LogStream::eos;

	VMatchTeam teams;
	teams.push_back(team);
	return OnMatchSuccess(teams);
}

UInt32 PkRoomMatchService::OnTeamCancelMatch(ObjID_t teamId)
{
	auto team = FindTeam(teamId);
	if (!team)
	{
		return ErrorCode::MATCH_CANCLE_NOT_MATCHING;
	}

	if (team->GetRaceID())
	{
		return ErrorCode::MATCH_CANCLE_RACING;
	}

	InfoLogStream << "team(" << teamId << ") cancel match." << LogStream::eos;

	DelTeam(teamId);
	return ErrorCode::SUCCESS;
}

bool PkRoomMatchService::OnMatchSuccess(VMatchTeam allTeams)
{
	auto* race = CreateRace();
	race->SetID(MatchServiceMgr::Instance()->GenRaceID());

	race->OnCreate(allTeams);

	AddRace(race);

	auto members = race->GetAllMember();
	std::random_shuffle(members.begin(), members.end());

	CLProtocol::MatchServerQueryPlayerInfoReq req;
	req.sessionId = race->GetID();
	req.type = GetMatchQueryType();
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		req.roleId = member->GetID();
		MSRouter::Instance()->SendToWorld(member->GetWsID(), req);
	}
	return true;
}

void PkRoomMatchService::OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
	auto race = FindRace(sessionId);
	if (!race)
	{
		return;
	}

	InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

	// ��ȡ�����Ϣʧ��
	if (result != 0)
	{
		ErrorLogStream << "player(" << roleId << ") is offline, match result (" << sessionId << ")." << LogStream::eos;

		DismissMatchRace(race, false);
		return;
	}

	auto member = race->FindMember(roleId);
	if (!member)
	{
		ErrorLogStream << "race(" << sessionId << ") can't find member(" << roleId << ")." << LogStream::eos;
		DismissMatchRace(race, false);
		return;
	}

	member->SetName(info.name);
	member->SetRaceInfo(info);

	if (!race->IsAllPlayerHasRaceInfo())
	{
		return;
	}

	// �����˵��������ҵı�����Ϣ���Ѿ�ȡ������
	CLProtocol::MatchSvrStartGameReq startReq;
	startReq.raceType = GetRaceType();
	startReq.gamesessionID = race->GetID();
	for (auto member : race->GetAllMember())
	{
		if (!member)
		{
			continue;
		}

		startReq.gamesessionFighter.push_back(member->GetFighter());
	}

	RelaySession* relaySession = RelaySessionMgr::Instance()->SelectRelayServer();
	if (!relaySession)
	{
		// û�����õ�RelayServer
		ErrorLogStream << "no useable relayserver." << LogStream::eos;

		// ���¿�ʼƥ��
		DismissMatchRace(race, false);
		return;
	}

	InfoLogStream << "race (" << sessionId << ") waiting relay server(" << relaySession->GetNodeID() << ") return." << LogStream::eos;

	relaySession->SendProtocol(startReq);

	race->SetRelayServerID(relaySession->GetNodeID());
	race->ChangeState(MRS_WAITING_RELAY_SERVER_RES);
}

bool PkRoomMatchService::OnJoinMatchPool(MatchTeam* team)
{
	std::vector<MatchTeam*> allTeams;
	allTeams.push_back(team);
	return OnMatchSuccess(allTeams);
}

void PkRoomMatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
{
	auto race = FindRace(gamesessionID);
	if (!race)
	{
		return;
	}

	InfoLogStream << "race (" << gamesessionID << ") recv start response, result(" << result << ") abality(" << abality << ")." << LogStream::eos;

	if (result != 0)
	{
		ErrorLogStream << "relay server return error(" << result << "), match(" << gamesessionID << ")." << LogStream::eos;
		DismissMatchRace(race, true);
		return;
	}

	race->ChangeState(MRS_RACE);

	// ����RelayServer����
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

	CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = GetRaceType();
	raceStartInfo.session = gamesessionID;
	raceStartInfo.addr = address;
	// ToDo...
	raceStartInfo.dungeonID = GenPkDungeon();
	raceStartInfo.battleFlag = MSApplication::Instance()->GetBattleFlag();

	race->SetDungeonID(raceStartInfo.dungeonID);

	CLProtocol::MatchSvrNotifyRacePlayerInfo notify;
	notify.gamesessionID = gamesessionID;
	notify.pkDungeonId = raceStartInfo.dungeonID;
	notify.battleFlag = raceStartInfo.battleFlag;

	for (auto member : race->GetAllMember())
	{
		if (!member)
		{
			continue;
		}

		// ����Ҫ�������Ƿ����
		raceStartInfo.players.push_back(member->GetRaceInfo());

		notify.pkType = member->GetPkType();
		notify.players.push_back(member->GetRaceInfo());
	}

	for (auto member : race->GetAllMember())
	{
		if (!member)
		{
			continue;
		}

		raceStartInfo.roleId = member->GetID();
		InfoLogStream << "race (" << gamesessionID << ") send raceinfo to player(" << member->GetID() << ")." << LogStream::eos;
		MSRouter::Instance()->SendToWorld(member->GetWsID(), raceStartInfo);
	}

	MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), notify);

	// ս����ʼ��
	{
		CLProtocol::RelaySvrRaceInitPacket init;
		init.raceId = gamesessionID;
		if (raceStartInfo.Encode(init.packet))
		{
			MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), init);
		}
	}
}