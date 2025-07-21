#ifndef _SCENE_LOST_DUNGEON_HANDLE_H_
#define _SCENE_LOST_DUNGEON_HANDLE_H_

#include <CLLostDungeonProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::LostDungeonChallengeReq& req);

void HandleProtocol(Player* player, CLProtocol::LostDungeonOpenReq& req);

void HandleProtocol(Player* player, CLProtocol::LostDungeonOpenBoxReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonBoxChoiceItemReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonGetRewardReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneLostDungeonGiveUpReq& req);

void HandleProtocol(CLProtocol::SceneLostDungeonSettleFloor& req);

void HandleProtocol(CLProtocol::BattleLostDungSyncDataToSelfScene& req);

#endif