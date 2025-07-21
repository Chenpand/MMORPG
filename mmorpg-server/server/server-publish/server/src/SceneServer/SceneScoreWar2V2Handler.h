#ifndef _SCENE_2V2_SCORE_WAR_HANDLER_H_
#define _SCENE_2V2_SCORE_WAR_HANDLER_H_

#include <CLNetwork.h>
#include <CL2V2ScoreWarProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::Scene2V2ScoreWarRewardReq& sync);

void HandleProtocol(CLProtocol::Scene2V2CrossSyncScoreWarInfo& sync);


#endif  //_SCENE_SCORE_WAR_HANDLER_H_