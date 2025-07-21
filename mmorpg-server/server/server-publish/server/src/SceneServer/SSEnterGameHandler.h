#ifndef _SS_ENTERGAME_HANDLER_H_
#define _SS_ENTERGAME_HANDLER_H_

#include <CLEnterGameProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SceneEnterGameReq& protocol);

void HandleProtocol(CLProtocol::WorldRegisterPlayerRet& protocol);

void HandleProtocol(CLProtocol::SceneGameReconnectReq& protocol);

void HandleProtocol(CLProtocol::SceneGameReconnectRet& protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneUpdateBulletinRecord& protocol);

#endif
