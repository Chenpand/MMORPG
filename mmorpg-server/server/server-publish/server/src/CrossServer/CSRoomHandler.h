#ifndef _CS_ROOM_HANDLER_H_
#define _CS_ROOM_HANDLER_H_

#include <CLPacketExecutor.h>
#include "CLRoomProtocol.h"
#include <CLGameSessionProtocol.h>
#include <CLMatchProtocol.h>
#include <CLChatProtocol.h>

void HandleProtocol(CLProtocol::CrossSyncRoomStartRace& protocol);

void HandleProtocol(CLProtocol::MatchServerPkRoomRes& protocol);

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerOnline& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerOffline& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerDisconnect& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerReconnect& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerChangeScene& protocol);

void HandleProtocol(CLProtocol::CrossWorldRoomListReq& req);

void HandleProtocol(CLProtocol::CrossUpdateRoomReq& req);

void HandleProtocol(CLProtocol::CrossJoinRoomReq& req);

void HandleProtocol(CLProtocol::CrossQuitRoomReq& req);

void HandleProtocol(CLProtocol::CrossKickOutRoomReq& req);

void HandleProtocol(CLProtocol::CrossDismissRoomReq& req);

void HandleProtocol(CLProtocol::CrossInviteJoinRoomReq& req);

void HandleProtocol(CLProtocol::CrossChangeRoomOwnerReq& req);

void HandleProtocol(CLProtocol::CrossBeInviteRoomReq& req);

void HandleProtocol(CLProtocol::CrossRoomCloseSlotReq& req);

void HandleProtocol(CLProtocol::CrossRoomSwapSlotReq& req);

void HandleProtocol(CLProtocol::CrossRoomResponseSwapSlotReq& req);

void HandleProtocol(CLProtocol::CrossRoomBattleStartReq& req);

void HandleProtocol(CLProtocol::CrossRoomBattleCancelReq& req);

void HandleProtocol(CLProtocol::CrossRoomBattleReadyReq& req);

void HandleProtocol(CLProtocol::CrossRoomSendInviteLinkReq& req);

void HandleProtocol(CLProtocol::CrossRoomMatchInfoRes& res);

void HandleProtocol(CLProtocol::CrossWorldInviteRoomRes& res);

void HandleProtocol(CLProtocol::SceneSyncChat& sync);

class CrossTransChatFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};

#endif //_CS_ROOM_HANDLER_H_