#ifndef __WORLD_OPERATE_HANDLER_H__
#define __WORLD_OPERATE_HANDLER_H__

#include "CLOpActiveProtocol.h"
class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::CWCDKVerifyReq& req);

void HandleProtocol(CLProtocol::CSCDKVerifyRes& res);
#endif