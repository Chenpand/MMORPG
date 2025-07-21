#ifndef _SCENE_BATTLE_HANDLER_H_
#define _SCENE_BATTLE_HANDLER_H_

#include <CLNetwork.h>
#include <CLBattleProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLGameSessionProtocol.h>

class Player;

class BattleEnterSceneFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol);

void HandleProtocol(CLProtocol::BattleExitSceneReq& protocol);

void HandleProtocol(CLProtocol::BattleExitSceneRes& protocol);

void HandleProtocol(CLProtocol::SceneBattleCreateBattleReq& protocol);

void HandleProtocol(CLProtocol::SceneBattleDestroyBattle& protocol);

void HandleProtocol(CLProtocol::SceneBattleKillSomeOne& protocol);

void HandleProtocol(CLProtocol::SceneBattleNotifyWaveInfo& protocol);

void HandleProtocol(CLProtocol::SceneBattleBalanceLog& protocol);

void HandleProtocol(CLProtocol::BattleCheckNoWarReq& protocol);

void HandleProtocol(CLProtocol::BattleShopNotify& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattlePickUpItemReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleChangeSkillsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneItemSync& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattlePickUpSpoilsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleSelectOccuReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleBirthTransfer& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleThrowSomeoneItemReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleDelItemReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleEnterBattleReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleNpcTradeReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattlePlaceTrapsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleOpenBoxReq& protocol);

void HandleProtocol(Player* player, CLProtocol::BattleChoiceSkillReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleNoWarChoiceReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneBattleOccuListReq& protocol);

void HandleProtocol(Player* player, CLProtocol::BattleChoiceEquipReq& protocol);

#endif
