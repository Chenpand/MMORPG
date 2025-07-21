#ifndef _WS_ITEM_HANDLER_H_
#define _WS_ITEM_HANDLER_H_

#include <CLItemProtocol.h>
#include <CLQuickBuyProtocol.h>
#include <CLPacketExecutor.h>

class WSPlayer;

void HandleProtocol(CLProtocol::SceneCheckItemNumRet& protocol);

void HandleProtocol(CLProtocol::SceneQuickBuyTransactionReq& req);
void HandleProtocol(CLProtocol::SWEnlargeStorageReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOpenJarRecordReq& req);
void HandleProtocol(CLProtocol::WorldAddJarRecordReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldEquipRecoOpenJarsRecordReq& req);

void HandleProtocol(CLProtocol::WorldSyncResistMagic& req);

class ItemCheckRetFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

void HandleProtocol(CLProtocol::WorldGotAccountMoneyReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetItemFriendPresentInfosReq& req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldItemFriendPresentReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetSysRecordReq& req);

void HandleProtocol(CLProtocol::SceneGetCreditPoint& req);

#endif
