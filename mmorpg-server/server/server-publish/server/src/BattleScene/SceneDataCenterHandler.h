#ifndef _SCENE_DATACENTER_HANDLER_H_
#define _SCENE_DATACENTER_HANDLER_H_

#include <CLDataCenterProtocol.h>

class Player;

void HandleProtocol(Player *pPlayer, CLProtocol::SceneDataCenterUdpLog &protocol);
void HandleProtocol(Player *pPlayer, CLProtocol::SceneCustomLogReport &protocol);

#endif

