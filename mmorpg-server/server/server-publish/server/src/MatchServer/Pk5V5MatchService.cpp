#include "Pk5V5MatchService.h"

#include <CLRelayServerProtocol.h>

#include "Pk5V5MatchAlgorithm.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"
#include "RelaySessionMgr.h"
#include "MSApplication.h"

Pk5V5MatchService::Pk5V5MatchService():PvPMatchService(true)
{
	SetType(MST_PK_5V5_MATCH);
}


Pk5V5MatchService::~Pk5V5MatchService()
{
}

bool Pk5V5MatchService::Init()
{
	auto algorithm = new Pk5V5MatchAlgorithm();
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

void Pk5V5MatchService::OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
	auto race = FindRace(sessionId);
	if (!race)
	{
		return;
	}

	InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

	// 获取玩家信息失败
	if (result != 0)
	{
		ErrorLogStream << "player(" << roleId << ") is offline, match result (" << sessionId << ")." << LogStream::eos;

		DismissMatchRace(race, true);
		MatchServiceMgr::Instance()->OnPlayerCancelMatch(roleId);
		return;
	}

	auto member = race->FindMember(roleId);
	if (!member)
	{
		ErrorLogStream << "race(" << sessionId << ") can't find member(" << roleId << ")." << LogStream::eos;
		return;
	}

	member->SetRaceInfo(info);

	if (!race->IsAllPlayerHasRaceInfo())
	{
		return;
	}

	// 到这里，说明所有玩家的比赛信息都已经取过来了
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
		// 没有有用的RelayServer
		ErrorLogStream << "no useable relayserver." << LogStream::eos;

		// 重新开始匹配
		DismissMatchRace(race, false);
		return;
	}

	InfoLogStream << "race (" << sessionId << ") waiting relay server(" << relaySession->GetNodeID() << ") return." << LogStream::eos;

	relaySession->SendProtocol(startReq);

	race->SetRelayServerID(relaySession->GetNodeID());
	race->ChangeState(MRS_WAITING_RELAY_SERVER_RES);
}

void Pk5V5MatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address)
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

	// 更新RelayServer负载
	RelaySessionMgr::Instance()->UpdateRelayServerAbality(race->GetRelayServerID(), abality);

	CLProtocol::WorldNotifyRaceStart raceStartInfo;
	raceStartInfo.raceType = GetRaceType();
	raceStartInfo.session = gamesessionID;
	raceStartInfo.addr = address;
	raceStartInfo.dungeonID = GenPkDungeon();
	raceStartInfo.battleFlag = MSApplication::Instance()->GetBattleFlag();

	race->SetDungeonID(raceStartInfo.dungeonID);

	CLProtocol::MatchSvrNotifyRacePlayerInfo notify;
	notify.gamesessionID = gamesessionID;
	notify.pkDungeonId = raceStartInfo.dungeonID;
	notify.battleFlag = raceStartInfo.battleFlag;

	auto members = race->GetAllMember();
	if (members.empty())
	{
		ErrorLogStream << "match(" << gamesessionID << ") members is empty." << LogStream::eos;
		DismissMatchRace(race, true);
		return;
	}

	auto leader = members[0];
	for (auto member : race->GetAllMember())
	{
		if (!member)
		{
			continue;
		}

		// 这里要检查玩家是否存在
		raceStartInfo.players.push_back(member->GetRaceInfo());

		notify.pkType = member->GetPkType();
		notify.players.push_back(member->GetRaceInfo());
	}

	UInt8 posSeat = race->GetAllMember().size();
	for (auto i = race->GetAllMember().size(); i < 10; ++i)
	{
		Robot* robot = RobotMgr::Instance()->RandOccuRobot();
		if (robot)
		{
			RobotMgr::Instance()->GenRobotEquipStrengthen(robot);
			auto raceInfo = robot->raceInfo;
			raceInfo.robotAIType = robot->hardType;
			raceInfo.pos = posSeat++;
			raceInfo.matchScore = race->GetRobotMatchScore();
			raceInfo.pkValue = _GenRobotPkValue(leader->GetRaceInfo().pkValue);
			raceInfo.seasonLevel = race->GetRobotSeasonLevel();

			raceStartInfo.players.push_back(raceInfo);
			notify.players.push_back(raceInfo);

			InfoLogStream << "race (" << gamesessionID << ") add robot(" << raceInfo.roleId << ")." << LogStream::eos;
		}
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

	OnRaceStarted(gamesessionID);

	MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), notify);

	// 战斗初始包
	{
		CLProtocol::RelaySvrRaceInitPacket init;
		init.raceId = gamesessionID;
		if (raceStartInfo.Encode(init.packet))
		{
			MSRouter::Instance()->SendToRelay(race->GetRelayServerID(), init);
		}
	}
}
