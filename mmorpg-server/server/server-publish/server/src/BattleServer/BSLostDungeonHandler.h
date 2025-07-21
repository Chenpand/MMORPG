#ifndef _BS_LOST_DUNGEON_HANDLER_H_
#define _BS_LOST_DUNGEON_HANDLER_H_
#include <CLLostDungeonProtocol.h>
#include <CLBattleProtocol.h>

void HandleProtocol(CLProtocol::LostDungeonMathBattleReq& protocol);

void HandleProtocol(CLProtocol::BattleLostDungeonPkReq& protocol);

void HandleProtocol(CLProtocol::BattleEnterSceneRes& protocol);

void HandleProtocol(CLProtocol::BattleTeamReportVoteChoice& protocol);

void HandleProtocol(CLProtocol::BattleExitNotify& protocol);

void HandleProtocol(CLProtocol::BattleDestoryRes& protocol);

void HandleProtocol(CLProtocol::BattleQuerylayerDetailOne& protocol);

void HandleProtocol(CLProtocol::BattleSyncRacePlayerInfo& protocol);

#endif