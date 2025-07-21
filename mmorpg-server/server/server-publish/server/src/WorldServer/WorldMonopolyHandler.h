#ifndef _WORLD_MONOPOLY_HANDLER_H_
#define _WORLD_MONOPOLY_HANDLER_H_
#include <CLWorldMonopolyProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyCoinReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyStatusReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyRollReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyCardExchangeReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyCardListReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolySellCardReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldMonopolyCardExchangedListReq &req);

void HandleProtocol(CLProtocol::WorldMonopolyGMJoinDungeon &req);



#endif