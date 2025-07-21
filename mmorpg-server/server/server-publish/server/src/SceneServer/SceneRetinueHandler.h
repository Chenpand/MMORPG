#ifndef _SCENE_RETINUE_HANDLER_H_
#define _SCENE_RETINUE_HANDLER_H_

#include <CLRetinueProtocol.h>

class Player;


void HandleProtocol(Player* player, CLProtocol::SceneChanageRetinueReq req);

void HandleProtocol(Player* player, CLProtocol::SceneRetinueChangeSkillReq req);

void HandleProtocol(Player* player, CLProtocol::SceneRetinueUnlockReq req);

void HandleProtocol(Player* player, CLProtocol::SceneRetinueUpLevelReq req);

#endif	//_SCENE_RETINUE_HANDLER_H_
