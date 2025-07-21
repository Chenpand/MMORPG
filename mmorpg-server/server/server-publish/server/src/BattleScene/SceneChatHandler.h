#ifndef _SCENE_CHAT_HANDLER_H_
#define _SCENE_CHAT_HANDLER_H_

#include <CLChatProtocol.h>

class Player;

void HandleProtocol(Player* player,CLProtocol::SceneChat& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneRequest& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneReply& protocol);

void HandleProtocol(Player* player,CLProtocol::SceneChatSendPos& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneChatHornReq& req);

void HandleProtocol(CLProtocol::SceneSyncChat& req);

#endif
