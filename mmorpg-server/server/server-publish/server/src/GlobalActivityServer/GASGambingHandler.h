#ifndef _GAS_GAMBING_HANDLER_H_
#define _GAS_GAMBING_HANDLER_H_

#include <CLGambingProtocol.h>
#include <CLItemProtocol.h>

#include "GASPlayer.h"


void HandleProtocol(GASPlayer* player, CLProtocol::PayingGambleReq &req);

void HandleProtocol(GASPlayer* player, CLProtocol::GambingItemQueryReq &req);

void HandleProtocol(GASPlayer* player, CLProtocol::GambingMineQueryReq &req);

void HandleProtocol(GASPlayer* player, CLProtocol::GambingRecordQueryReq &req);

void HandleProtocol(GASPlayer* player, CLProtocol::SceneCheckItemNumRet& protocol);

#endif