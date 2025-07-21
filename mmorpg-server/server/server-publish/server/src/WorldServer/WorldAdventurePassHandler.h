#ifndef _WORLD_ADVENTURE_PASS_H_
#define _WORLD_ADVENTURE_PASS_H_
#include <CLWorldAdventurePassProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassStatusReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassBuyReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassBuyLvReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassExpPackReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldAventurePassRewardReq &req);


#endif