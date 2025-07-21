#ifndef _TCS_DUNGEON_HANDLER_H_
#define _TCS_DUNGEON_HANDLER_H_

#include <CLGameSessionProtocol.h>
#include <CLMatchProtocol.h>
#include <CLTeamCopyProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLItemProtocol.h>
#include <CLSyncObjectProtocol.h>

class TCSPlayer;

void HandleProtocol(CLProtocol::DungenServerTeamStartRaceRes& protocol);

void HandleProtocol(CLProtocol::DungeonSvrDungeonRaceEnd& protocol);

void HandleProtocol(CLProtocol::RelayDungeonBossPhaseChange& protocol);

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol);

void HandleProtocol(CLProtocol::TeamCopyDungeonRevive& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol);

void HandleProtocol(CLProtocol::SceneToDungeonQuickBuyTransactionReq& protocol);

void HandleProtocol(CLProtocol::TeamCopyPhaseBossInfo& protocol, TCSPlayer* player);

void HandleProtocol(CLProtocol::SceneSyncEvent& protocol);

#endif
