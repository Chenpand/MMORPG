#pragma once
#include <CLLostDungeonProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonSwitchChageModeReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonPkReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonTaskListReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonChoiceTaskReq& req);

void HandleProtocol(Player* player, CLProtocol::BattleLostDungSeeIntellReq& req);

void HandleProtocol(CLProtocol::BattleSynLostDungTeamSt& req);

void HandleProtocol(Player* player, CLProtocol::SceneQueryKillTargetPosReq& req);

void HandleProtocol(CLProtocol::BattleQuerylayerDetailOne& req);

void HandleProtocol(CLProtocol::BattleNotifyTeamRaceEnd& req);

void HandleProtocol(CLProtocol::BattleSyncRacePlayerInfo& req);
