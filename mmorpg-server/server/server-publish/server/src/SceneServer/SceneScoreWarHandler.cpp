#include "SceneScoreWarHandler.h"
#include "ScoreWarMgr.h"
#include "SSRouter.h"
#include "Player.h"
#include <CLErrorCode.h>

void HandleProtocol(Player* player, CLProtocol::SceneScoreWarRewardReq& sync)
{
	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), sync);
		return;
	}

	CLProtocol::SceneScoreWarRewardRes res;
	res.result = player->GiveScoreWarReward(sync.rewardId);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneCrossSyncScoreWarInfo& sync)
{
	ScoreWarMgr::Instance()->OnCrossSyncScoreWarInfo((ScoreWarStatus)sync.status, sync.statusEndTime, sync.startTime);

	CLProtocol::SceneCrossSyncScoreWarInfoRes res;
	res.result = ErrorCode::SUCCESS;
	Router::SendToCross(res);

}
