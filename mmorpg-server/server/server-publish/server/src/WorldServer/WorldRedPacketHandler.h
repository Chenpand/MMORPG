#ifndef __WORLD_RED_PACKET_HANDLER_H__
#define __WORLD_RED_PACKET_HANDLER_H__

#include <CLRedPacketProtocol.h>

class WSPlayer;

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendRedPacketReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOpenRedPacketReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendCustomRedPacketReq& req);

void HandleProtocol(CLProtocol::SceneAddRedPacketReq& req);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetGuildRedPacketInfoReq& req);

#endif