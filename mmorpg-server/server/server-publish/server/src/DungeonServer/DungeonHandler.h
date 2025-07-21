#ifndef _DUNGEON_HANDLER_H_
#define _DUNGEON_HANDLER_H_

#include <CLGameSessionProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLItemProtocol.h>
#include <CLQuickBuyProtocol.h>
#include <CLVerifyProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLDungeonProtocol.h>

class DSPlayer;

//void HandleProtocol(Protocol::DungenServerTeamCreateRaceReq& req);

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceReq& req);

void HandleProtocol(CLProtocol::MatchSvrStartGameRes& res);

void HandleProtocol(CLProtocol::MatchSvrDungeonRaceEnd& res);

void HandleProtocol(CLProtocol::MatchSvrRelayRegister& reg);

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol);

void HandleProtocol(CLProtocol::RelayDungeonBossPhaseChange& protocol);

void HandleProtocol(CLProtocol::TeamCopyNotifyRaceDestroyDungeon& protocol);

void HandleProtocol(CLProtocol::TeamCopyAddRaceReviveCnt& protocol);

void HandleProtocol(CLProtocol::DungeonSvrEndRace& protocol);

void HandleProtocol(CLProtocol::DungeonSvrReviveReq& protocol);

void HandleProtocol(CLProtocol::SceneToDungeonQuickBuyTransactionReq& req);

void HandleProtocol(CLProtocol::VerifyReportTaskResult& protocol);

void HandleProtocol(CLProtocol::SceneSyncEvent& sync);

void HandleProtocol(CLProtocol::TeamCopyNotifyBimsEnergyAccuProgress& protocol);

void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonEnterNextAreaReq& protocol);
void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonRewardReq& protocol);
void HandleProtocol(DSPlayer* player, CLProtocol::SceneDungeonKillMonsterReq& protocol);

void HandleProtocol(CLProtocol::RelayAbalitySync& sync);

#endif