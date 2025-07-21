#ifndef _WORLD_DIG_HANDLER_H_
#define _WORLD_DIG_HANDLER_H_

#include <CLPacketExecutor.h>
#include <CLDigProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapOpenReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapCloseReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigWatchReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigOpenReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigMapInfoReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDigRecordsReq& protocol);

#endif //_WORLD_DIG_HANDLER_H_