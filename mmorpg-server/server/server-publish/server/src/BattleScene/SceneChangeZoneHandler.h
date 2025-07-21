#ifndef _SCENE_CHANGEZONE_HANDLER_H_
#define _SCENE_CHANGEZONE_HANDLER_H_

#include <CLChangeZoneProtocol.h>

class Player;

void HandleProtocol(CLProtocol::GateChangeZoneCheckRet& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneChangeZoneReturn &protocol);

#endif
