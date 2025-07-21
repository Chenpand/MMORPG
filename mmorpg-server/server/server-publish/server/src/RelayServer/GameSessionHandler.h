#ifndef _RL_GAMESESSIONHANDLER_H_
#define _RL_GAMESESSIONHANDLER_H_

#include <CLGameSessionProtocol.h>
#include <CLRelayServerProtocol.h>
#include <CLReplayProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLSysProtocol.h>

//relay server & match server
//处理协议match -> relay 的MATCHSERVER_START_GAME_REQ
void HandleProtocol(CLProtocol::MatchSvrStartGameReq& protocol);
void HandleProtocol(CLProtocol::MatchSvrNotifyRacePlayerInfo& protocol);
void HandleProtocol(CLProtocol::MatchSvrTerminateGame& protocol);
void HandleProtocol(CLProtocol::SceneServerPlayerLeaveRace& protocol);
void HandleProtocol(CLProtocol::SceneServerSyncPlayerEvent& protocol);
void HandleProtocol(CLProtocol::SceneServerGiveupReconn& protocol);
void HandleProtocol(CLProtocol::RelaySvrRaceInitPacket& protocol);

//client & relay server
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrLoginReq& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrReconnectReq& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrPlayerInputReq& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrEndGameReq& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrDungeonRaceEndReq& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrDisconnectNotify& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrFrameChecksumRequest& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrPingNotify& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrReportLoadProgress& protocol);
void HandleProtocol(ObjID_t roleId, CLProtocol::RelaySvrObserveReq& protocol);

//replay server
void HandleProtocol(CLProtocol::ReplayFileReq& protocol);

//verify server
void HandleProtocol(CLProtocol::VerifyCreateTaskRes& res);

void HandleProtocol(CLProtocol::VerifyReportTaskResult& res);

void HandleProtocol(ObjID_t roleId, CLProtocol::SysHeartbeat& protocol);

#endif
