#ifndef _SCENE_GIFTBAG_HANDLER_H_
#define _SCENE_GIFTBAG_HANDLER_H_

#include <CLGiftBagProtocol.h>
#include <CLGSActivityProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneGetGiftbag& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneGetActiveGiftbag &protocol);

void HandleProtocol(CLProtocol::WorldNotifyGameStartTimeToS &protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneFestivalCounterReq& protocol);

void HandleProtocol(Player *pPlayer, CLProtocol::SceneGSActivityStoreGold &protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneBuyGiftBag& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneGiftPackInfoReq& req);

#endif
