#include "GameSessionHandler.h"
#include <AvalonCompress.h>
#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"
#include "RLApplication.h"
#include "RaceMgr.h"
#include "Race.h"
#include "PvPRace.h"

//
void HandleProtocol(CLProtocol::MatchSvrStartGameReq& protocol)
{
#ifndef _NEW_RACE_MODE_
	InfoLogStream << "In Handle MatchSvrStartGameReq, game sessionId(" << protocol.gamesessionID << "), Fighters: " << LogStream::eos;
	for (std::vector<CLProtocol::Fighter>::iterator iter = protocol.gamesessionFighter.begin(); iter != protocol.gamesessionFighter.end(); ++iter)
	{
		InfoLogStream << "Fighter[" << iter->pos << "]: accId(" << iter->accId << ") roleId(" << iter->roleId << ")" << LogStream::eos;
	}

	//
	ObjID_t sessionId = protocol.gamesessionID;
	CLProtocol::MatchSvrStartGameRes response;

	response.gamesessionID = sessionId;

	//检查参战者是否可以加入对战
	RLPlayer* fighter = NULL;
	bool alreadHere = false;
	for (auto iter : protocol.gamesessionFighter)
	{
		fighter = RLPlayerMgr::Instance()->find_player(iter.roleId);
		if (fighter) //fighter was already here
		{
			ErrorLogStream << "in HandleProtocol(CLProtocol::MatchSvrStartGameReq& protocol), fighter(pos:" << (UInt16)iter.pos
				<< ", accId:" << iter.accId << ",roleId:" << iter.roleId << ") was already here, just return match server false!" << LogStream::eos;
			alreadHere = true;
		}
	}

	if (alreadHere)
	{
		response.result = CLProtocol::MatchSvrRlaySvrResultCode_fighter_alreadyhere;
		protocol.SendBack(response);
		//Router::SendToMatchSvr(response);
		return;
	}

// 	for (std::vector<CLProtocol::Fighter>::iterator iter = protocol.gamesessionFighter.begin(); iter != protocol.gamesessionFighter.end(); ++iter)
// 	{
// 		fighter = RLPlayerMgr::Instance()->find_player(iter->roleId);
// 	}


	RLGameSession* session = RLGameSessionMgr::Instance()->find_gamesession(sessionId);
	if (NULL != session) //已经存在了game session
	{
		response.result = CLProtocol::MatchSvrRlaySvrResultCode_duplicatedSessionId;
		ErrorLogStream << "In Handle MatchSvrStartGameReq received duplicated game sessionId(" << sessionId << "), just response error code(" << response.result << ") to match server" << LogStream::eos;
		protocol.SendBack(response);
		//Router::SendToMatchSvr(response);
		return;
	}

	//create game session
	session = new RLGameSession();
	if (NULL == session)
	{
		ErrorLogStream << "In Handle MatchSvrStartGameReq, can't new RLGameSession" << LogStream::eos;
		return;
	}

    session->set_race_type((RaceType)protocol.raceType);
	session->SetID(sessionId);

	//
	//add fighters
	for (std::vector<CLProtocol::Fighter>::iterator iter = protocol.gamesessionFighter.begin(); iter != protocol.gamesessionFighter.end(); ++iter)
	{
		//InfoLogStream << "Fighter[" << iter->pos << "]: accId(" << iter->accId << ") roleId(" << iter->roleId << ")" << LogStream::eos;
		if (!session->add_fighter(iter->pos, iter->accId, iter->roleId))
		{
			//
			ErrorLogStream << "add_fighter failed ! Fighter[" << iter->pos << "]: accId(" << iter->accId << ") roleId(" << iter->roleId << ")" << LogStream::eos;
			CL_SAFE_DELETE(session);
			return;
		}
	}

	
	RLGameSessionMgr::Instance()->add_gamesession(session);

	session->set_status(RLGameSession::gamesessionStatus_waitstart);
	session->set_waitStartTimer(RLGameSession::max_gamesession_waitstart);

	//send response to match server
	response.result = CLProtocol::MatchSvrRlaySvrResultCode_success;
	response.ability = RLPlayerMgr::Instance()->get_ablity();
	//response.address.
	std::string ip = RLApplication::Instance()->GetProperties()->GetProperty("Server", "ip").GetString();
	UInt16 port = RLApplication::Instance()->GetProperties()->GetProperty("Server", "port");
	response.address.SetIP(ip);
	response.address.SetPort(port);

	protocol.SendBack(response);
	//Router::SendToMatchSvr(response);

	return;
#else
    for (std::vector<CLProtocol::Fighter>::iterator iter = protocol.gamesessionFighter.begin(); iter != protocol.gamesessionFighter.end(); ++iter)
    {
        InfoLogStream << "create race(" << protocol.gamesessionID << ") gamer(" << iter->accId << ", " << iter->roleId << ", " << iter->pos << ")" << LogStream::eos;
    }

    ObjID_t raceId = protocol.gamesessionID;
    CLProtocol::MatchSvrStartGameRes response;

    response.gamesessionID = raceId;
	response.raceType = protocol.raceType;

    //检查参战者是否可以加入对战
    RLPlayer* fighter = NULL;
    bool alreadHere = false;
    for (auto iter : protocol.gamesessionFighter)
    {
		fighter = RLPlayerMgr::Instance()->find_player(iter.roleId);
		if (fighter)
        {
            ErrorLogStream << "fighter(pos:" << (UInt16)iter.pos << ", accId:" << iter.accId << ",roleId:" 
                            << iter.roleId << ") was already in racing, just return match server false!" << LogStream::eos;
            alreadHere = true;
        }
    }

    if (alreadHere)
    {
        response.result = CLProtocol::MatchSvrRlaySvrResultCode_fighter_alreadyhere;
		protocol.SendBack(response);
		//Router::SendToMatchSvr(response);
        return;
    }

    auto* race = RaceMgr::Instance()->FindRace(raceId);
    if (race)
    {
        response.result = CLProtocol::MatchSvrRlaySvrResultCode_duplicatedSessionId;
        ErrorLogStream << " received duplicated race(" << raceId << "), just response error code(" << response.result << ") to match server" << LogStream::eos;
		protocol.SendBack(response);
		//Router::SendToMatchSvr(response);
        return;
    }

    race = RaceMgr::Instance()->CreateRace((RaceType)protocol.raceType);
    if (!race)
    {
        ErrorLogStream << "race(" << raceId << ") create instance failed." << LogStream::eos;

		response.result = CLProtocol::MatchSvrRlaySvrResultCode_invalid_race_type;
		protocol.SendBack(response);
		//Router::SendToMatchSvr(response);
        return;
    }

    race->SetID(raceId);

    for (auto& fighter : protocol.gamesessionFighter)
    {
        if (!race->AddGamer(fighter.accId, fighter.roleId, fighter.pos, fighter.wsId))
        {
            ErrorLogStream << "race(" << raceId << ") add gamer failed, gamer(" << fighter.accId << ", " << fighter.roleId << ", " << fighter.pos << ")." << LogStream::eos;
            CL_SAFE_DELETE(race);

			response.result = CLProtocol::MatchSvrRlaySvrResultCode_add_gamer_failed;
			protocol.SendBack(response);
			//Router::SendToMatchSvr(response);
            return;
        }
    }
    
    race->ChangeStatus(RACE_STATUS_WAIT_LOGIN);
    RaceMgr::Instance()->AddRace(race);

    //send response to match server
    response.result = CLProtocol::MatchSvrRlaySvrResultCode_success;
    response.ability = RLPlayerMgr::Instance()->get_ablity();
    std::string ip = RLApplication::Instance()->GetProperties()->GetProperty("Server", "ip").GetString();
    UInt16 port = RLApplication::Instance()->GetProperties()->GetProperty("Server", "port");
    response.address.SetIP(ip);
    response.address.SetPort(port);

	protocol.SendBack(response);
    //Router::SendToMatchSvr(response);

	// 创建比赛成功，说明放弃了之前的比赛
	// 1. 删除重连信息
	// 2. 调用放弃比赛流程
	for (auto& fighter : protocol.gamesessionFighter)
	{
		auto reconInfo = RaceMgr::Instance()->GetReconnectInfo(fighter.roleId);
		if (!reconInfo)
		{
			continue;
		}

		// 先删除重连信息
		//auto raceId = reconInfo->raceId;
		RaceMgr::Instance()->DelReconnectInfo(fighter.roleId);

		auto race = RaceMgr::Instance()->FindRace(reconInfo->raceId);
		if (race)
		{
			race->OnGamerReconnectTimeout(fighter.roleId);
			continue;
		}
	}
#endif
}

void HandleProtocol(CLProtocol::MatchSvrTerminateGame& protocol)
{
	auto race = RaceMgr::Instance()->FindRace(protocol.gamesessionID);
	if (!race)
	{
		ErrorLogStream << "match server want stop unexist race(" << protocol.gamesessionID << ")." << LogStream::eos;
		return;
	}

	race->Dismiss(gamesessionDismissType_ms_terminate);
}

void HandleProtocol(CLProtocol::MatchSvrNotifyRacePlayerInfo& protocol)
{
	auto race = RaceMgr::Instance()->FindRaceWithType<PvPRace>(protocol.gamesessionID);
	if (!race)
	{
		ErrorLogStream << "match server notify race player info to unexist race(" << protocol.gamesessionID << ")." << LogStream::eos;
		return;
	}

	race->SetPkType((PkType)protocol.pkType);
	race->SetPkDungeonID(protocol.pkDungeonId);
	race->SetRacePlayerInfo(protocol.players);
	race->SetBattleFlag(protocol.battleFlag);

	InfoLogStream << "race(" << protocol.gamesessionID << ") set pktype(" << protocol.pkType << ") dungeon(" << protocol.pkDungeonId << ")." << LogStream::eos;
}

void HandleProtocol(CLProtocol::SceneServerPlayerLeaveRace& protocol)
{
    DebugLogStream << "recv race(" << protocol.raceId << ")player(" << protocol.roleId << ") leave race notfiy from scene server." << LogStream::eos;

	auto roleid = protocol.roleId;
	auto raceid = protocol.raceId;
	auto player = RLPlayerMgr::Instance()->find_player(roleid);
	if (player && raceid != player->GetRaceID())
    {
		InfoLogStream << "recv race(" << raceid << ")player(" << roleid << ") leave race notfiy from scene server, player not in this race." << LogStream::eos;
        return;
    }

	auto race = RaceMgr::Instance()->FindRace(raceid);
    if (!race)
    {
		ErrorLogStream << "recv player(" << roleid << ") leave race notfiy from scene server, can't find race("
			<< raceid << ")." << LogStream::eos;
        return;
    }

	if (race->GetGamerByRoleID(roleid))
	{
		race->OnGamerLeaveRace(roleid, GAMER_LEAVE_INITIATIVE);
	}
	else
	{
		race->OnObserverDisconnect(roleid);
	}
	
    //player->disconnect();
}

void HandleProtocol(CLProtocol::SceneServerSyncPlayerEvent& protocol)
{
	DebugLogStream << "recv player(" << protocol.roleId << ")'s sync event(" << protocol.type << "), session(" << protocol.raceId << ")." << LogStream::eos;

	auto race = RaceMgr::Instance()->FindRace(protocol.raceId);
	if (!race)
	{
		ErrorLogStream << "race(" << protocol.raceId << ") recv player(" << protocol.roleId << ")'s sync event, but can't find this race." << LogStream::eos;
		return;
	}

	if (protocol.type == SYNC_PLAYER_EVENT_LEVELUP)
	{
		race->OnGamerLevelUp(protocol.roleId, (UInt16)protocol.param1);
	}
	else if (protocol.type == SYNC_PLAYER_EVENT_REVIVE)
	{
		race->OnGamerReviveOther(protocol.roleId, protocol.param1, protocol.param2);
	}
	else if (protocol.type == SYNC_DESTORY_DUNGEON)
	{
		race->OnNotifyDestoryDungeon(protocol.param1);
	}
	else if (protocol.type == SYNC_END_RACE)
	{
		race->OnLogicEndDungeon(protocol.param1);
	}
	else if (protocol.type == SYNC_TEAMCOPY_BIMS_ENERY_PROGRESS)
	{
		race->OnTeamCopyNotifyBimsEnergyProgress(protocol.param1, protocol.param2);
	}
	else if (protocol.type == SYNC_USE_ITEM)
	{
		race->OnUseItem(protocol.roleId, protocol.param1, protocol.param2);
	}
	else if (protocol.type == SYNC_ADD_REVIVE_CNT)
	{
		race->OnAddReviveCnt(protocol.param1);
	}
}

void HandleProtocol(CLProtocol::SceneServerGiveupReconn& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") giveup reconnect." << LogStream::eos;

	// 先从在线玩家中找，这是正在进行中的比赛
	auto raceId1 = (ObjID_t)0;
	auto player = RLPlayerMgr::Instance()->find_player(protocol.roleId);
	if (player)
	{ 
		raceId1 = player->GetRaceID();
		auto race = RaceMgr::Instance()->FindRace(raceId1);
		if (race)
		{
			InfoLogStream << "player(" << protocol.roleId << ") leave race(" << raceId1 << ")." << LogStream::eos;

			if (race->GetGamerByRoleID(protocol.roleId))
			{
				race->OnGamerLeaveRace(protocol.roleId, GAMER_LEAVE_INITIATIVE);
			}
			else
			{
				race->OnObserverDisconnect(protocol.roleId);
			}

			
			race->OnGamerReconnectTimeout(protocol.roleId);
		}
	}

	// 再从已经离线的比赛中找
	auto reconn = RaceMgr::Instance()->GetReconnectInfo(protocol.roleId);
	if (!reconn)
	{
		return;
	}

	auto raceId2 = reconn->raceId;
	RaceMgr::Instance()->DelReconnectInfo(protocol.roleId);

	if (raceId1 == raceId2)
	{
		return;
	}

	auto race = RaceMgr::Instance()->FindRace(raceId2);
	if (race)
	{
		race->OnGamerReconnectTimeout(protocol.roleId);
	}
}

void HandleProtocol(CLProtocol::RelaySvrRaceInitPacket& protocol)
{
	DevLogStream << "recv race(" << protocol.raceId << ") init packet." << LogStream::eos;

	auto race = RaceMgr::Instance()->FindRace(protocol.raceId);
	if (!race)
	{
		ErrorLogStream << "can't find race(" << protocol.raceId << ")." << LogStream::eos;
		return;
	}

	if (race->GetInitPacket())
	{
		ErrorLogStream << "repeat get race(" << protocol.raceId << ")'s init packet." << LogStream::eos;
		return;
	}

	race->SetInitPacket(protocol.packet);
	race->OnRecvInitPacket();
	protocol.Detach();
}

void HandleProtocol(CLProtocol::ReplayFileReq& protocol)
{
	CLProtocol::ReplayFileRes res;
	res.raceId = protocol.raceId;
	res.error = 0;

	auto race = RaceMgr::Instance()->FindRaceWithType<PvPRace>(protocol.raceId);
	if (!race)
	{
		res.error = 1;
		DebugLogStream << "replay server need race(" << protocol.raceId << ")'s replay file, can't find this race..." << LogStream::eos;

		Router::SendToReplaySvr(res);
		return;
	}

	auto file = race->GenReplayFile();

	static UInt8 s_EncodeBuffer[1024 * 1024];
	Avalon::NetOutputStream stream(s_EncodeBuffer, sizeof(s_EncodeBuffer));
	try
	{
		stream & file;
	}
	catch (Avalon::NetSerializeException& e)
	{
		res.error = 1;
		FatalLogStream << "encode race(" << protocol.raceId << ")'s replay file failed..." << e.what() << LogStream::eos;

		Router::SendToReplaySvr(res);
		return;
	}


	static UInt8 dstBuffer[1024 * 1024];
	size_t dstLen = sizeof(dstBuffer);
	Avalon::snappy_compress((char*)dstBuffer, &dstLen, (const char*)s_EncodeBuffer, stream.GetPos());

	auto packet = Avalon::Packet::Create();
	UInt32 maxSizePerPacket = Avalon::Packet::MAX_BODY_SIZE;
	int index = 0;
	while (dstLen > (size_t)index)
	{
		auto len = dstLen - index;
		if (len > maxSizePerPacket)
		{
			len = maxSizePerPacket;
		}

		res.totalLen = dstLen;
		res.index = index;
		res.Encode(packet);

		UInt32 protocolSize = packet->GetSize();
		Avalon::NetOutputStream encodeStream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - protocolSize);
		try
		{
			encodeStream & (UInt16)len;
			encodeStream.SeriaArray(dstBuffer + index, len);
			index += len;
		}
		catch (Avalon::NetSerializeException& e)
		{
			res.error = 1;
			FatalLogStream << "encode race(" << protocol.raceId << ")'s replay res..." << e.what() <<  LogStream::eos;

			Router::SendToReplaySvr(res);
			Avalon::Packet::Destroy(packet);
			return;
		}

		packet->SetSize(protocolSize + encodeStream.GetPos());

		Router::SendToReplaySvr(packet);
	}

	Avalon::Packet::Destroy(packet);

	/*auto packet = Avalon::Packet::Create();
	res.Encode(packet);

	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());

	// 现在先一次性通过，正常情况下其实要根据录像文件的大小分包
	// ToDO...
	{
		UInt8 replayBuffer[65536];
		Avalon::NetOutputStream replayStream(replayBuffer, sizeof(replayBuffer));

		try
		{
			replayStream & file;
			stream.SeriaArray(replayBuffer, replayStream.GetPos());
			packet->SetSize((UInt16)stream.GetPos());
			Router::SendToReplaySvr(packet);
		}
		catch (Avalon::NetException& e)
		{
			FatalLogStream << "encode race(" << protocol.raceId << ")'s replay failed " << e.what() << LogStream::eos;

			Avalon::Packet::Destroy(packet);
			res.error = 1;
			Router::SendToReplaySvr(res);
		}


	}

	Avalon::Packet::Destroy(packet);*/
}

void HandleProtocol(CLProtocol::VerifyCreateTaskRes& res)
{
	auto race = RaceMgr::Instance()->FindRace(res.raceId);
	if (!race)
	{
		ErrorLogStream << "can't find race(" << res.raceId << ")." << LogStream::eos;

		// 通知放弃比赛
		if (RLApplication::Instance()->NotifyGiveupVerifyWhenRaceUnexist())
		{
			InfoLogStream << "notify verify server give up unexist race(" << res.raceId << ")" << LogStream::eos;
			CLProtocol::VerifyReportRaceDismiss report;
			report.raceId = res.raceId;
			res.SendBack(res);
		}

		return;
	}

	if (res.result != 0)
	{
		// 验证任务创建失败
		ErrorLogStream << "race(" << res.raceId << ") create verify task failed, reason(" << res.result << ")." << LogStream::eos;
		return;
	}

	InfoLogStream << "race(" << res.raceId << ") create verify task success, node(" << res.GetConnID() << ")." << LogStream::eos;
	race->OnCreateVerifyReturn(res.GetConnID());
}

void HandleProtocol(CLProtocol::VerifyReportTaskResult& res)
{
	auto race = RaceMgr::Instance()->FindRace(res.raceId);
	if (!race)
	{
		ErrorLogStream << "can't find race(" << res.raceId << ")." << LogStream::eos;
		return;
	}

	auto gamer = race->GetGamerByRoleID(res.roleId);
	if (!gamer)
	{
		ErrorLogStream << "race(" << res.raceId << ") can't find gamer(" << res.roleId << ")." << LogStream::eos;
		return;
	}

	//race->OnGamerLeaveRace(res.roleId, GAMER_LEAVE_UNSYNC);
	if (RT_DUNGEON_CROSS == race->GetRaceType())
	{
		Router::SendToDungeonSvr(res);
	}
	else
	{
		Router::SendToWorldSvr(gamer->GetWsID(), res);
	}
}