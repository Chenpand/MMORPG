#ifndef _SCENE_RELATION_HANDLER_H_
#define _SCENE_RELATION_HANDLER_H_

#include <CLRelationProtocol.h>

class Player;

void HandleProtocol(CLProtocol::SceneRelationSyncList& req);

void HandleProtocol(CLProtocol::SceneRelationNotifyNew& req);

void HandleProtocol(CLProtocol::SceneRelationNotifyDel& req);

void HandleProtocol(CLProtocol::SceneRelationSyncIntimacy& req);

void HandleProtocol(CLProtocol::SceneRelationFindPlayerReq& req);

void HandleProtocol(CLProtocol::SceneQueryPlayerReq& req);

void HandleProtocol(CLProtocol::SceneQueryPlayerDetailsReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentInfoReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentDraw& req);

void HandleProtocol(Player* player, CLProtocol::SceneRelationPresentGive& req);

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendReq& req);

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendRet& req);
void HandleProtocol(Player* player, CLProtocol::SceneMasterGiveEquip& req);
void HandleProtocol(Player* player, CLProtocol::SceneAddonPayReq& req);
void HandleProtocol(CLProtocol::WorldCheckMasterGiveRes& res);

#endif
