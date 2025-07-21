#ifndef _SCENE_STORY_HANDLER_H_
#define _SCENE_STORY_HANDLER_H_

#include <CLStoryProtocol.h>

class Player;

void HandleProtocol(Player *pPlayer, CLProtocol::SceneNotifyStopStory &protocol);

void HandleProtocol(Player *player, CLProtocol::SceneNotifyNewBoot &protocol);

void HandleProtocol(Player *player, CLProtocol::SceneNotifyBootFlag &protocol);

#endif
