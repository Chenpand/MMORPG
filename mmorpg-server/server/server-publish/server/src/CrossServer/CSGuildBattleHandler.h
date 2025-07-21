#ifndef _CS_GUILD_BATTLE_HANDLER_H_
#define _CS_GUILD_BATTLE_HANDLER_H_

#include <CLNetwork.h>
#include <CLGuildProtocol.h>
#include <CLGameSessionProtocol.h>

void HandleProtocol(CLProtocol::WorldCrossGuildBattleEnrollInfoRes& res);

void HandleProtocol(CLProtocol::CrossGuildBattleEnrollReq& req);

void HandleProtocol(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);

void HandleProtocol(CLProtocol::CrossGuildBattleRecordReq& req);

void HandleProtocol(CLProtocol::CrossGuildBattleSortListSync& req);

void HandleProtocol(CLProtocol::CrossGuildBattleSortListReq& req);

void HandleProtocol(CLProtocol::CrossGuildBattleSelfSortListReq& req);

void HandleProtocol(CLProtocol::CrossGuildBattleGMSync& sync);

#endif  //_CS_GUILD_BATTLE_HANDLER_H_