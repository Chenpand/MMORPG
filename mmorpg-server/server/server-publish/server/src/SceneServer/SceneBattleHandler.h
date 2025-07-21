#ifndef _SCENE_BATTLE_HANDLER_H_
#define _SCENE_BATTLE_HANDLER_H_

#include <CLBattleProtocol.h>

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol);

void HandleProtocol(CLProtocol::BattleExitSceneReq& protocol);

void HandleProtocol(CLProtocol::SceneBattleBalanceEnd& protocol);

void HandleProtocol(CLProtocol::BattleNotifyRankAward& protocol);

void HandleProtocol(CLProtocol::BattleOpenNotify& protocol);

#endif
