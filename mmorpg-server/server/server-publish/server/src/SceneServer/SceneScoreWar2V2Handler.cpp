#include "SceneScoreWar2V2Handler.h"
#include "ScoreWar2V2Mgr.h"
#include "SSRouter.h"
#include "Player.h"
#include <CLErrorCode.h>

void HandleProtocol(Player* player, CLProtocol::Scene2V2ScoreWarRewardReq& sync)
{
	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), sync);
		return;
	}

	CLProtocol::Scene2V2coreWarRewardRes res;
	res.result = player->Give2V2ScoreWarReward(sync.rewardId);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::Scene2V2CrossSyncScoreWarInfo& sync)
{
	ScoreWar2V2Mgr::Instance()->OnCrossSyncScoreWarInfo((ScoreWar2V2Status)sync.status, sync.statusEndTime, sync.startTime);

	CLProtocol::Scene2V2CrossSyncScoreWarInfoRes res;
	res.result = ErrorCode::SUCCESS;
	Router::SendToCross(res);

}
