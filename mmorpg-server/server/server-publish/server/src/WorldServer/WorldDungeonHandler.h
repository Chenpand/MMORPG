#ifndef __WORLD_LEVEL_HANDLER_H__
#define __WORLD_LEVEL_HANDLER_H__

#include <CLDungeonProtocol.h>

class WSPlayer;

void HandleProtocol(ObjID_t id, CLProtocol::SceneRareItemReq& req);

void HandleProtocol(CLProtocol::WorldDungeonHardUpdate& req);

void HandleProtocol(CLProtocol::SceneGotRaceItem& req);

void HandleProtocol(CLProtocol::SceneDungeonChestReq& req);

void HandleProtocol(CLProtocol::WorldDungeonStartRaceReq& req);

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonRaceEndReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonEnterNextAreaReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonRewardReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonOpenChestReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonEndDropReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonReviveReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonKillMonsterReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonClearAreaMonsters& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonEnterRaceReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonReportFrameReq& req);


void HandleProtocol(CLProtocol::WorldDungeonNotifyResetAreaIndex& notify);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonGetAreaIndexReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonRollItemReq& req);

#endif