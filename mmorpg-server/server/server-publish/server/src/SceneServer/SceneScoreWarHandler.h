#ifndef _SCENE_SCORE_WAR_HANDLER_H_
#define _SCENE_SCORE_WAR_HANDLER_H_

#include <CLNetwork.h>
#include <CLScoreWarProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneScoreWarRewardReq& sync);

void HandleProtocol(CLProtocol::SceneCrossSyncScoreWarInfo& sync);


#endif  //_SCENE_SCORE_WAR_HANDLER_H_