#ifndef _WORLD_MALL_HANDLER_H_
#define _WORLD_MALL_HANDLER_H_

#include <CLMallProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallBuy &protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQueryItemReq &protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQueryItemDetailReq& protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::CWMallBatchBuyReq& protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallGiftPackActivateReq& protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldGetMallItemByItemIdReq &protocol);
void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQuerySingleItemReq &protocol);

void HandleProtocol(CLProtocol::WorldMallCheckBuyRet &protocol);
void HandleProtocol(CLProtocol::SWRefreshLimiteBuyNum &protocol);
void HandleProtocol(CLProtocol::MallGiftPackActivateReq &protocol);
void HandleProtocol(CLProtocol::MallActivateCondReq &protocol);

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldGetRechargePushItemsReq& protocol);
void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldBuyRechargePushItemReq& protocol);
void HandleProtocol(CLProtocol::WorldCheckRechargePushRes& protocol);
void HandleProtocol(CLProtocol::WorldSceneBuyRechargePushItemRes& protocol);

#endif

