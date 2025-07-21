#include "WorldPremiumLeagueHandler.h"
#include "WSPlayer.h"
#include "PremiumLeagueMgr.h"


void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueRewardPoolReq &req)
{
	PremiumLeagueMgr::Instance()->SyncRewardPool(player);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueEnrollReq &req)
{
	CLProtocol::WorldPremiumLeagueEnrollRes res;
	if (PremiumLeagueMgr::Instance()->GetStatus() == PLS_ENROLL)
	{
		res.result = PremiumLeagueMgr::Instance()->OnPlayerEnroll(player);
	}
	else
	{
		res.result = PremiumLeagueMgr::Instance()->OnPlayerReEnroll(player);
	}
	
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPremiumLeagueBattleRecordReq &req)
{
	CLProtocol::WorldPremiumLeagueBattleRecordRes res;
	std::vector<BattleRecordEntry*> entrys;
	if (req.isSelf)
	{
		entrys = PremiumLeagueMgr::Instance()->GetBattleRecordMgr().GetSelfRecord(player->GetID());
		res.totalCount = entrys.size();
	}
	else
	{
		entrys = PremiumLeagueMgr::Instance()->GetBattleRecordMgr().GetRecord(req.startIndex, req.count);
		res.totalCount = PremiumLeagueMgr::Instance()->GetBattleRecordMgr().GetSize();
	}

	for (auto entry : entrys)
	{
		PremiumLeagueRecordEntry* tmpEntry = (PremiumLeagueRecordEntry*)entry;
		if (!tmpEntry)
		{
			continue;
		}

		res.records.push_back(*tmpEntry);
	}

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::MatchServerPremiumLeagueBattleRes& res)
{
	auto battle = PremiumLeagueMgr::Instance()->FindBattleByGuid(res.battleId);
	if (!battle)
	{
		ErrorLogStream << "can't find premium battle(" << res.battleId << ") raceid(" << res.sessionId << ")." << LogStream::eos;
		return;
	}

	battle->raceId = res.sessionId;
	battle->relayAddr = res.addr;
	battle->UpdateToDB();
	battle->SyncToPlayer();
}