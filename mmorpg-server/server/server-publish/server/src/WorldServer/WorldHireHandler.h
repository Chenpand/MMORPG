#ifndef _WORLD_HIRE_HANDLE_H_
#define _WORLD_HIRE_HANDLE_H_
#include <CLRelationProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireInfoReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldUseHireCodeReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryTaskStatusReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireTaskAccidListReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireListReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldSubmitHireTaskReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireCoinReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHirePushReq &req);
void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryHireAlreadyBindReq &req);



void HandleProtocol(CLProtocol::WorldQueryHireAlreadyBindRes &req);

void HandleProtocol(CLProtocol::WorldOnSceneTrigeTask &req);

void HandleProtocol(CLProtocol::GASCheckHireBindRet &req);




#endif