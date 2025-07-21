#ifndef _WORLD_TASK_HANDLER_H_
#define _WORLD_TASK_HANDLER_H_

#include <CLTaskProtocol.h>
class WSPlayer;


void HandleProtocol(CLProtocol::WorldAccountTaskCheckPakcageEnoughRes& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSubmitAccountTask& res);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetTaskItemReq& protocol);
#endif