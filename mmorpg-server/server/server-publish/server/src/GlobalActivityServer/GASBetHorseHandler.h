#ifndef _BET_HORSE_HANDLER_H_
#define _BET_HORSE_HANDLER_H_

#include <CLBetHorseProtocol.h>
#include <CLGlobalActivityProtocol.h>

class GASPlayer;

void HandleProtocol(GASPlayer* player, CLProtocol::BetHorseReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterOddsReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterHistoryRecordReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::StakeReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::StakeRecordReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::BattleRecordReq& req);

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterRankReq& req);

void HandleProtocol(CLProtocol::GASBetHorseMailRewardRes& protocol);

#endif