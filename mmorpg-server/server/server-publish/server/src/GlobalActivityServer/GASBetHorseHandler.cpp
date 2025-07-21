#include "GASBetHorseHandler.h"
#include "BetHorseMgr.h"

void HandleProtocol(GASPlayer* player, CLProtocol::BetHorseReq& req)
{
	BetHorseMgr::Instance()->HandlerBetHorseData(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterOddsReq& req)
{
	BetHorseMgr::Instance()->HandlerRealTimeOdds(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterHistoryRecordReq& req)
{
	BetHorseMgr::Instance()->HandlerShooterHistory(player, req.shooterId);
}

void HandleProtocol(GASPlayer* player, CLProtocol::StakeReq& req)
{
	BetHorseMgr::Instance()->HandlerStake(player, req.id, req.num);
}

void HandleProtocol(GASPlayer* player, CLProtocol::StakeRecordReq& req)
{
	BetHorseMgr::Instance()->HandlerPlayerStakeRecord(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::BattleRecordReq& req)
{
	BetHorseMgr::Instance()->HandlerBattleRecord(player);
}

void HandleProtocol(GASPlayer* player, CLProtocol::ShooterRankReq& req)
{
	BetHorseMgr::Instance()->HandlerShooterRank(player);
}

void HandleProtocol(CLProtocol::GASBetHorseMailRewardRes& protocol)
{
	BetHorseMgr::Instance()->HandlerWorldBetHorseMailRewardRes(protocol.playerUid);
}