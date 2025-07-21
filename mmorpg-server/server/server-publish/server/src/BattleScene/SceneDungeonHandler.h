#ifndef _SCENE_DUNGEON_HANDLER_H_
#define _SCENE_DUNGEON_HANDLER_H_

#include <CLDungeonProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneDungeonStartReq &req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonRaceEndReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonEnterNextAreaReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonRewardReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonOpenChestReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonEndDropReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonReviveReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonKillMonsterReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonClearAreaMonsters& req);

void HandleProtocol(Player* player, CLProtocol::WorldDungeonEnterRaceReq& req);

void HandleProtocol(Player* player, CLProtocol::WorldDungeonReportFrameReq& req);

void HandleProtocol(CLProtocol::SceneDungeonChestRes& res);

void HandleProtocol(CLProtocol::WorldDungeonStartRaceRes& res);

void HandleProtocol(CLProtocol::WorldDungeonNotifyRaceEnd& notify);

void HandleProtocol(CLProtocol::WorldDungeonNotifyClearAreaMonster& notify);

void HandleProtocol(CLProtocol::WorldDungeonNotifyConsumeFatigue& notify);

void HandleProtocol(CLProtocol::WorldDungeonNotifyKillMonster& notify);

void HandleProtocol(CLProtocol::WorldDungeonNotifyGotItems& notify);

// ËÀÍöÖ®Ëþ
void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutResultReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutQueryResultReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTowerWipeoutQuickFinishReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTowerResetReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneTowerFloorAwardReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonBuyTimesReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDungeonResetAreaIndexReq& req);

#endif
