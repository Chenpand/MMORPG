#ifndef _SCENE_PET_HANDLER_H_
#define _SCENE_PET_HANDLER_H_

#include <CLPetProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneSetPetSoltReq req);

void HandleProtocol(Player* player, CLProtocol::SceneFeedPetReq req);

void HandleProtocol(Player* player, CLProtocol::SceneSellPetReq req);

void HandleProtocol(Player* player, CLProtocol::SceneChangePetSkillReq req);

void HandleProtocol(Player* player, CLProtocol::SceneSetPetFollowReq req);

void HandleProtocol(Player* player, CLProtocol::SceneDevourPetReq req);

#endif


