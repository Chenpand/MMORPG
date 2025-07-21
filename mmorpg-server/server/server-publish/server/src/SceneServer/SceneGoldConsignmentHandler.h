#ifndef _SCENE_GOLD_CONSIGNMENT_H_
#define _SCENE_GOLD_CONSIGNMENT_H_

#include <CLGoldConsignmentProtocol.h>
#include <CLUnionServerProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneGoldConsignmentSubmitOrderReq& req);

void HandleProtocol(CLProtocol::SceneGoldConsignmentDealNotify& req);

void HandleProtocol(Player* player, CLProtocol::UnionPlayerGetItem& req);

#endif // !_SCENE_GOLD_CONSIGNMENT_H_
