#include "ChampionMatchService.h"
#include "MatchServiceMgr.h"
#include "MSRouter.h"
#include "RelaySessionMgr.h"
#include <CLRelayServerProtocol.h>
#include "MSApplication.h"

ChampionMatchService::ChampionMatchService()
{

}

void ChampionMatchService::OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr & address)
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

		// 这里要检查玩家是否存在
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
		MSRouter::Instance()->SendToUnion(raceStartInfo);
	}

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


	CLProtocol::MatchServerChampionBattleRes res;
	auto team = race->GetAllTeam()[0];
	res.groupID = team->GetGroupID();
	res.order = team->GetOrderID();
	res.sessionId = gamesessionID;
	res.addr = address;
	MSRouter::Instance()->SendToUnion(res);
}

bool ChampionMatchService::OnMatchSuccess(VMatchTeam allTeams)
{
	auto* race = CreateRace();
	race->SetID(MatchServiceMgr::Instance()->GenRaceID());

	race->OnCreate(allTeams);

	AddRace(race);

	auto members = race->GetAllMember();

	CLProtocol::UnionMatchServerQueryPlayerInfoReq req;
	req.msg.sessionId = race->GetID();
	req.msg.type = GetMatchQueryType();
	UInt8 seat = 0;
	for (auto member : members)
	{
		if (!member)
		{
			continue;
		}

		member->SetSeat(seat++);
		req.msg.roleId = member->GetID();
		MSRouter::Instance()->SendToUnion(req);
	}

	return true;
}

void ChampionMatchService::OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info)
{
	auto race = FindRace(sessionId);
	if (!race)
	{
		ErrorLogStream << "can't find race id is " << sessionId << "role id is " << roleId << " result is " << result << LogStream::eos;
		return;
	}

	InfoLogStream << "race (" << sessionId << ") recv player( " << roleId << ") race info result(" << result << ")." << LogStream::eos;

	// 获取玩家信息失败
	if (result != 0)
	{

		CLProtocol::MatchServerChampionRaceEnd req;
		auto team = race->GetAllTeam()[0];
		req.groupID = team->GetGroupID();
		req.result = _resultBitmask::resultBitmask_invalid;
		req.raceId = race->GetID();
		req.loser.id = roleId;
		req.order = team->GetOrderID();
		auto members = team->GetTeamMembers();
		for (auto& i : members)
		{
			if (i->GetID() != roleId)
			{
				req.winner.id = i->GetID();
				break;
			}
		}
		req.dungeonID = 0;
		MSRouter::Instance()->SendToUnion(req);

		ErrorLogStream << "query player info faild(" << req.groupID << "," << req.raceId << "," << req.order << ") winner id is " << req.winner.id << " loser id is " << req.loser.id << LogStream::eos;

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

void ChampionMatchService::DismissMatchRace(const MatchRace* race, bool notice)
{
	if (!race)
	{
		return;
	}

	if (notice)
	{
		//直接解散按平局处理
		CLProtocol::MatchServerChampionRaceEnd req;
		auto team = race->GetAllTeam()[0];
		req.groupID = team->GetGroupID();
		req.result = _resultBitmask::resultBitmask_draw;
		req.raceId = race->GetID();
		req.order = team->GetOrderID();
		req.dungeonID = 0;
		auto members = team->GetTeamMembers();
		for (auto& i : members)
		{
			if (req.winner.id == 0)
			{
				req.winner.id = i->GetID();
			}
			else
			{
				req.loser.id = i->GetID();
			}
		}
		InfoLogStream << "DismissMatchRace("<< req.groupID << "," << req.raceId  << ","<< req.order << ") winner id is " << req.winner.id << " loser id is " << req.loser.id << LogStream::eos;
		MSRouter::Instance()->SendToUnion(req);
	}

	for (auto team : race->GetAllTeam())
	{
		if (!team)
		{
			continue;
		}

		InfoLogStream << "race (" << race->GetID() << ") dismissed, team(" << team->GetID() << ") deleted." << LogStream::eos;

		DelTeam(team->GetID());
		continue;
	}

	DelRace(race->GetID());
}