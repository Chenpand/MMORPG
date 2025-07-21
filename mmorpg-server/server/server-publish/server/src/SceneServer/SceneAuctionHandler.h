#ifndef _SCENE_AUCTION_HANDLER_H_
#define _SCENE_AUCTION_HANDLER_H_

#include <CLAuctionProtocol.h>
class Player;

void HandleProtocol(CLProtocol::WorldAuctionCheck &req);

void HandleProtocol(Player* player, CLProtocol::SceneAuctionRefreshReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneAuctionBuyBoothReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneAbnormalTransactionRecordReq& req);

void HandleProtocol(CLProtocol::SceneAbnormalTransRemoveMailItemRes& res);

void HandleProtocol(CLProtocol::WorldNotifyQueryWaitHandleAbnormalTrans& protocol);

void HandleProtocol(CLProtocol::SceneAuctionCheckPackageReq &req);

#endif

