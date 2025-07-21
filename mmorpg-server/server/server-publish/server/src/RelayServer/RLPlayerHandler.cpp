#include "GameSessionHandler.h"
#include "RLPlayerMgr.h"
#include "RLPlayer.h"
#include "RLPlayerMgr.h"
#include "CLDefine.h"
#include "CLErrorCode.h"
#include "RaceMgr.h"
#include "Race.h"
#include "PvPRace.h"
#include "DungeonRace.h"
#include "RelaySvrRouter.h"

//
//#define SEND_LOGIN_RESPONSE()

//
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrLoginReq& protocol)
{
#ifndef _NEW_RACE_MODE_
	DebugLogStream << "HandleProtocol RelaySvrLoginReq(protocol id:" << protocol.GetProtocolID() << ", accId:" << protocol.m_unAccId << ", roleId:" << protocol.m_unRoleId << ", sessionId:" << protocol.m_unGameSessionId << ", pos:" << protocol.m_ucPos << LogStream::eos;

	ObjID_t sessionId	= protocol.m_unGameSessionId;
	//ObjID_t roleId = protocol.m_unRoleId;
	UInt32	accId = protocol.m_unAccId;
	UInt8	pos = protocol.m_ucPos;

	CLProtocol::RelaySvrLoginRes response;
	response.result = success;
	//response.currentTime = 

	//noted：注意处理断线重连

	//so many people
	if (RLPlayerMgr::Instance()->get_ablity() <= 0)
	{
		response.result = ErrorCode::RELAY_LOGIN_SYSTEMERROR;

		return;
	}

	//
	RLPlayer* player = new RLPlayer(accId);
	if (NULL == player)
	{
		ErrorLogStream << "in HandleProtocol(CLProtocol::RelaySvrLoginReq& protocol), new player(accId:" << accId << ", roleId:" << roleId << ")  failed, just return " << LogStream::eos;
		return;
	}

	player->SetID(roleId);

	InfoLogStream << "in HandleProtocol(CLProtocol::RelaySvrLoginReq& protocol), new player(accId:" << accId << ", roleId:" << roleId << ")  success!" << LogStream::eos;

	//
	RLGameSession* session = RLGameSessionMgr::Instance()->find_gamesession(sessionId);
	if (NULL == session)
	{
		ErrorLogStream << "HandleProtocol RelaySvrLoginReq, can't find game session(id:" << sessionId << ")" << LogStream::eos;
		//也是验证过程
		response.result = ErrorCode::RELAY_LOGIN_INVALIDSESSION;
		player->send_protocol(response, true); //通知udp conn server断开链接

		//delete the player
		CL_SAFE_DELETE(player);
		return;
	}

	if (!session->add_fighter(pos, player))
	{
		ErrorLogStream << "HandleProtocol RelaySvrLoginReq, can't add_fighter to game session(id:" << sessionId <<") fighter(pos:" << pos << ", roleid:" << roleId << ")" << LogStream::eos;
		response.result = ErrorCode::RELAY_LOGIN_INVALIDFIGHTER;
		player->send_protocol(response, true);//通知udp conn server断开链接

		//delete the player
		DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrLoginReq& protocol), to delete player(accId:" << accId << ", roleId:" << player->GetID() << "), and to dismiss game session(id:" << sessionId << ")" << LogStream::eos;
		CL_SAFE_DELETE(player);



		session->Dismiss(gamesessionDismissType_offline);
		return;
	}

	//
	player->set_gamesession(session);

	RLPlayerMgr::Instance()->add_player(player);

	//
	response.currentTime = Avalon::Time::CurrentTime().MSec();

	//
	player->send_protocol(response);

	//to check the game session 
	//session->OnFighterLogin();
	session->OnFighterLogin(pos, player);
#else
    ObjID_t sessionId = protocol.m_unGameSessionId;
    UInt32	accId = protocol.m_unAccId;
    //UInt8	pos = protocol.m_ucPos;

    CLProtocol::RelaySvrLoginRes res;
    res.result = ErrorCode::SUCCESS;

	ErrorLogStream << "player(accId:" << accId << ", roleId:" << roleId << ")  begin login" << LogStream::eos;

    /*if (RLPlayerMgr::Instance()->get_ablity() <= 0)
    {
        res.result = ErrorCode::RELAY_LOGIN_SYSTEMERROR;
        return;
    }*/

    RLPlayer* player = new RLPlayer(accId);
    if (NULL == player)
    {
        ErrorLogStream << "new player(accId:" << accId << ", roleId:" << roleId << ")  failed, just return " << LogStream::eos;
        return;
    }

    player->SetID(roleId);
    player->SetRaceID(sessionId);

    auto race = RaceMgr::Instance()->FindRace(sessionId);
    if (NULL == race)
    {
        ErrorLogStream << "player(" << roleId << "want login unexist race(" << sessionId << ")" << LogStream::eos;

        res.result = ErrorCode::RELAY_LOGIN_INVALIDSESSION;
        player->send_protocol(res, true);
        CL_SAFE_DELETE(player);

        return;
    }

    if (!race->OnGamerLogin(player))
    {
        res.result = ErrorCode::RELAY_LOGIN_INVALIDFIGHTER;
        player->send_protocol(res, true);
        CL_SAFE_DELETE(player);
		InfoLogStream << "player(accId:" << accId << ", roleId:" << roleId << ")  OnGamerLogin fail " << LogStream::eos;
        return;
    }

    res.currentTime = Avalon::Time::CurrentTime().MSec();
    player->send_protocol(res);

    RLPlayerMgr::Instance()->add_player(player);
	InfoLogStream << "player(accId:" << accId << ", roleId:" << roleId << ")  Logined " << LogStream::eos;
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrReconnectReq& protocol)
{
	ObjID_t sessionId = protocol.gameSessionId;
	UInt32	accId = protocol.accid;
	//UInt8	pos = protocol.pos;
	roleId = protocol.roleId;

	CLProtocol::RelaySvrReconnectRes res;
	res.result = ErrorCode::SUCCESS;

	if (RLPlayerMgr::Instance()->get_ablity() <= 0)
	{
		res.result = ErrorCode::RELAY_RECONNECT_SYSTEMERROR;
		return;
	}

	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (player)
	{
		ErrorLogStream << "player(" << roleId << "want reconnect race(" << sessionId << ") player is online." << LogStream::eos;
		res.result = ErrorCode::RELAY_RECONNECT_PLAYER_ONLINE;
		return;
	}

	player = new RLPlayer(accId);
	if (NULL == player)
	{
		ErrorLogStream << "new player(accId:" << accId << ", roleId:" << roleId << ")  failed, just return " << LogStream::eos;
		return;
	}

	player->SetID(roleId);
	player->SetRaceID(sessionId);

	auto race = RaceMgr::Instance()->FindRace(sessionId);
	if (NULL == race)
	{
		ErrorLogStream << "player(" << roleId << "want reconnect unexist race(" << sessionId << ")" << LogStream::eos;

		res.result = ErrorCode::RELAY_RECONNECT_INVALIDSESSION;
		player->send_protocol(res, true);
		CL_SAFE_DELETE(player);
		return;
	}

	if (race->GetGamerByRoleID(roleId))
	{
		if (!race->OnGamerReconnect(player, protocol.lastFrame))
		{
			res.result = ErrorCode::RELAY_RECONNECT_INVALIDFIGHTER;
			player->send_protocol(res, true);
			CL_SAFE_DELETE(player);
			return;
		}

		InfoLogStream << "race(" << race->GetID() << ") gamer(" << player->GetID() << ") start to reconnect..." << LogStream::eos;
	}
	else
	{
		// 观战
		res.result = race->OnObserverLogin(player, protocol.lastFrame);
		if (res.result != ErrorCode::SUCCESS)
		{
			player->send_protocol(res, true);
			CL_SAFE_DELETE(player);
			return;
		}

		InfoLogStream << "race(" << race->GetID() << ") observer(" << player->GetID() << ") start to reconnect..." << LogStream::eos;
	}

	

	res.result = ErrorCode::SUCCESS;
	player->send_protocol(res);

	RLPlayerMgr::Instance()->add_player(player);
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrPlayerInputReq& protocol)
{
#ifndef _NEW_RACE_MODE_
	ObjID_t sessionId = protocol.gamesessionId;
	UInt8 pos = protocol.pos;

	RLGameSession* session = RLGameSessionMgr::Instance()->find_gamesession(sessionId);
	if (NULL == session)
	{
		ErrorLogStream << "in  HandleProtocol(CLProtocol::RelaySvrPlayerInputReq& protocol), can't find game session(" << sessionId << "), just return" << LogStream::eos;
		return;
	}

	//
	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		ErrorLogStream << "in  HandleProtocol(CLProtocol::RelaySvrPlayerInputReq& protocol), can't find player(pos:" << (UInt16)pos << ", roleId:" << roleId << ")"  << "in game session(" << sessionId << "), just return" << LogStream::eos;
		return;
	}

	session->OnFighterInputGameData(pos, player, protocol.input);
#else
	InfoLogStream << "rev input data1 = " << protocol.input.data1 << LogStream::eos; 
    RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
        ErrorLogStream << "recv input data msg, can't find player(" << roleId << ")." << LogStream::eos;
        return;
    }

    auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
    if (!race)
    {
        ErrorLogStream << "recv input data msg from player(" << roleId << ") can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
        return;
    }

    race->OnRecvGamerInputData(player, protocol.input);
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrEndGameReq& protocol)
{
#ifndef _NEW_RACE_MODE_
	ObjID_t sessionId = protocol.gamesessionId;
	UInt8 pos = protocol.pos;
	UInt8 gameResult = protocol.gameResult;

	DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrEndGameReq& protocol), player(pos:" << pos << "), roleId(" << roleId << ") want to end session(" << sessionId << "), and result:" << gameResult  << ")" << LogStream::eos;

	RLGameSession* session = RLGameSessionMgr::Instance()->find_gamesession(sessionId);
	if (NULL == session)
	{
		ErrorLogStream << "in HandleProtocol(CLProtocol::RelaySvrEndGameReq& protocol), can't find game session(" << sessionId << "), for player(roleId:" << roleId << ")" << LogStream::eos;
		return;
	}

	//
	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		ErrorLogStream << "in HandleProtocol(CLProtocol::RelaySvrEndGameReq& protocol), can't find player(pos:" << (UInt16)pos << ",roleId:" << roleId  << ") in game session(" << sessionId << "), for player(roleId:" << roleId << ")" << LogStream::eos;
		return;
	}

	session->OnFighterRequestEndGame(pos, player, gameResult);
#else
    RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
		ErrorLogStream << "recv player(" << roleId << ")'s race end msg, but can't find player." << LogStream::eos;
        return;
    }

    ObjID_t raceId = player->GetRaceID();
    PvPRace* race = RaceMgr::Instance()->FindRaceWithType<PvPRace>(raceId);

    if (!race)
    {
        ErrorLogStream << "recv player(" << player->GetID() << ")'s race end msg, but can't find pvp race(" << raceId << ")." << LogStream::eos;
        return;
    }

    race->OnRecvRaceEndMsg(player, protocol);
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrDungeonRaceEndReq& protocol)
{
#ifndef _NEW_RACE_MODE_
    ObjID_t sessionId = protocol.raceEndInfo.gamesessionId;

    DebugLogStream << "race(" << sessionId << ") recv player(" << roleId << ")'s dungeon race end request." << LogStream::eos;

    RLGameSession* session = RLGameSessionMgr::Instance()->find_gamesession(sessionId);
    if (NULL == session)
    {
        ErrorLogStream << "race(" << sessionId << ") recv player(" << roleId << ")'s dungeon race end request, but can't find this race." << LogStream::eos;
        return;
    }

    auto player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
        ErrorLogStream << "can't find player" << roleId << ") in game session(" << sessionId << ") when request to dungeon race end." << LogStream::eos;
        return;
    }

    session->OnFighterRequestDungeonEndGame(player, protocol.raceEndInfo);
#else
    DebugLogStream << "recv player(" << roleId << ")'s dungeon race end request." << LogStream::eos;

	std::ostringstream logstr;
	logstr << "recv player(" << roleId << ")'s dungeon race end, session id : " << protocol.raceEndInfo.gamesessionId;
	for (auto info : protocol.raceEndInfo.infoes)
	{
		logstr << "roleId : " << info.roleId << ", pos : " << (UInt32)info.pos << ", score : " << (UInt32)info.score;
	}

	DebugLogStream << logstr.str() << LogStream::eos;

    auto player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
        ErrorLogStream << "can't find player" << roleId << ") when request to dungeon race end." << LogStream::eos;
        return;
    }

    auto race = RaceMgr::Instance()->FindRaceWithType<DungeonRace>(player->GetRaceID());
    if (!race)
    {
        ErrorLogStream << "race(" << player->GetRaceID() << ") recv player(" << roleId << ")'s dungeon race end request, but can't find this race." << LogStream::eos;
        return;
    }

    race->OnRecvRaceEndMsg(player, protocol);
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrDisconnectNotify& protocol)
{
#ifndef _NEW_RACE_MODE_
	DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrDisconnectNotify& protocol), player(roleId:" << protocol.roleId <<  ", accId:" << protocol.accId << ") notified offline" << LogStream::eos;

	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrDisconnectNotify& protocol), can't find player(roleId:" << roleId << ", accId:" << protocol.accId << "), so just return" << LogStream::eos;
		return;
	}

	RLGameSession* session = player->get_gamesession();
	if (NULL == session)
	{
		DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrDisconnectNotify& protocol),  player(roleId:" << roleId << ", accId:" << player->get_accId() << ")'s session is null, so just return" << LogStream::eos;

		return;
	}

	player->set_offline();

	UInt8 pos = session->get_player_pos(player);
	if (pos == RLGameSession::gamesession_invalid_position)
	{
		DebugLogStream << "in HandleProtocol(CLProtocol::RelaySvrDisconnectNotify& protocol),  player(roleId:" << roleId << ", accId:" << player->get_accId() << ")'s position is valid, so just return" << LogStream::eos;

		return;
	}

	session->OnFighterNotifyOffline(pos, player);
#else
    RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
        DebugLogStream << "player(" << roleId << ") offline, can't find player instance." << LogStream::eos;
        return;
    }

    player->set_offline();

    auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
    if (!race)
    {
        ErrorLogStream << "player(" << player->GetID() << ") offline, but can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
		RLPlayerMgr::Instance()->remove_player(player);
		RLPlayerMgr::Instance()->FreePlayer(player);
        return;
    }

	if (race->GetGamerByRoleID(roleId))
	{
    race->OnGamerLeaveRace(player->GetID(), GAMER_LEAVE_DISCONNECT);
	}
	else
	{
		race->OnObserverDisconnect(player->GetID());
	}
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrFrameChecksumRequest& protocol)
{
#ifndef _NEW_RACE_MODE_
	InfoLogStream << "in HandleProtocol(CLProtocol::RelaySvrFrameChecksumRequest& protocol), player(rolid:" << protocol.roleId << ") request check frames: " << LogStream::eos;
	
// 	std::vector<CLProtocol::FrameChecksum>::iterator iter = protocol.frameschecksum.begin();
// 	for (; iter != protocol.frameschecksum.end(); ++iter)
// 	{
// 		InfoLogStream << "sequence:" << iter->framesequence << ", checksum:" << iter->checksum << LogStream::eos;
// 	}
	InfoLogStream << "sequence:" << protocol.frameschecksum.framesequence << ", checksum:" << protocol.frameschecksum.checksum << LogStream::eos;

	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		ErrorLogStream << "in HandleProtocol(CLProtocol::RelaySvrFrameChecksumRequest& protocol), can't find player  by roleid(" << roleId << ")" << LogStream::eos;
		return;
	}

	RLGameSession* session = player->get_gamesession();
	if (NULL == session)
	{
		ErrorLogStream << "in HandleProtocol(CLProtocol::RelaySvrFrameChecksumRequest& protocol), player(accId:" << player->get_accId() << ",roleId:" << player->GetID() << ")'s game session is null! just discard the packet" << LogStream::eos;
		return;
	}

	session->OnFighterRequestCheckFrame(player, protocol.frameschecksum);
#else
    RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
    if (NULL == player)
    {
        DebugLogStream << "player(" << roleId << ") report frame checksum, can't find player instance." << LogStream::eos;
        return;
    }

    auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
    if (!race)
    {
        ErrorLogStream << "player(" << player->GetID() << ") report frame checksum, but can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
        return;
    }

    race->OnRecvGamerFrameChecksum(player, protocol.frameschecksum);
#endif
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrPingNotify& protocol)
{
	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		DebugLogStream << "player(" << roleId << ") notify ping, can't find player instance." << LogStream::eos;
		return;
	}

	auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") notify ping, but can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
		return;
	}

	race->OnRecvGamerPing(player, protocol.ping);
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrReportLoadProgress& protocol)
{
	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		DebugLogStream << "player(" << roleId << ") report load progress, can't find player instance." << LogStream::eos;
		return;
	}

	auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") report load progress, but can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
		return;
	}

	race->OnGamerReportLoadProgress(player, protocol.progress);
}

void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrObserveReq& protocol)
{
	ObjID_t sessionId = protocol.raceId;
	UInt32	accId = protocol.accid;

	CLProtocol::RelaySvrObserveRes res;

	if (RLPlayerMgr::Instance()->find_player(roleId))
	{
		ErrorLogStream << "player(accId:" << accId << ", roleId:" << roleId << ")  repeat login." << LogStream::eos;
		return;
	}

	RLPlayer* player = new RLPlayer(accId);
	if (NULL == player)
	{
		ErrorLogStream << "new player(accId:" << accId << ", roleId:" << roleId << ")  failed, just return " << LogStream::eos;
		return;
	}

	player->SetID(roleId);
	player->SetRaceID(sessionId);

	auto race = RaceMgr::Instance()->FindRace(sessionId);
	if (NULL == race)
	{
		ErrorLogStream << "player(" << roleId << "want observe unexist race(" << sessionId << ")" << LogStream::eos;

		res.result = ErrorCode::RELAY_OBSERVE_RACE_NOT_EXIST;
		player->send_protocol(res, true);
		CL_SAFE_DELETE(player);
		return;
	}

	res.result = race->OnObserverLogin(player, protocol.startFrame);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << roleId << "want observe race(" << sessionId << ") failed, error(" << res.result << ")" << LogStream::eos;

		player->send_protocol(res, true);
		CL_SAFE_DELETE(player);
		return;
	}

	player->send_protocol(res);

	// 发送开始协议
	if (race->GetInitPacket())
	{
		player->send_packet(*race->GetInitPacket(), false);
	}

	RLPlayerMgr::Instance()->add_player(player);
}

void HandleProtocol(ObjID_t roleId, CLProtocol::SysHeartbeat& protocol)
{
	RLPlayer* player = RLPlayerMgr::Instance()->find_player(roleId);
	if (NULL == player)
	{
		DebugLogStream << "player(" << roleId << ") recv heartbeat, can't find player instance." << LogStream::eos;
		return;
	}

	auto race = RaceMgr::Instance()->FindRace(player->GetRaceID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") recv heartbeat, but can't find race(" << player->GetRaceID() << ")." << LogStream::eos;
		return;
	}

	auto gamer = race->GetGamerByRoleID(roleId);
	if (gamer)
	{
		gamer->SetLastRecvHeartbeatTime((UInt32)CURRENT_TIME.Sec());
	}
}