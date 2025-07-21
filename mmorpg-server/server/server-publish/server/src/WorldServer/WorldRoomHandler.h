#ifndef _WORLD_ROOM_HANDLER_H_
#define _WORLD_ROOM_HANDLER_H_

#include <CLPacketExecutor.h>
#include <CLRoomProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLMatchProtocol.h>
#include <CLScoreWarProtocol.h>
#include <CL2V2ScoreWarProtocol.h>

class WSPlayer;

void HandleProtocol(CLProtocol::MatchServerPkRoomRes& protocol);

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomListReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldJoinRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQuitRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldKickOutRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDismissRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInviteJoinRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeRoomOwnerReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBeInviteRoomReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomCloseSlotReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomSwapSlotReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomResponseSwapSlotReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleStartReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleCancelReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleReadyReq& protocol);

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomSendInviteLinkReq& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerInfo& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomLogInfo& protocol);

void HandleProtocol(CLProtocol::CrossSyncRoomNotifyInfo& protocol);

void HandleProtocol(CLProtocol::CrossWorldRoomListRes& protocol);

void HandleProtocol(CLProtocol::CrossUpdateRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossJoinRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossQuitRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossKickOutRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossDismissRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossInviteJoinRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossChangeRoomOwnerRes& protocol);

void HandleProtocol(CLProtocol::CrossBeInviteRoomRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomCloseSlotRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomSwapSlotRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomResponseSwapSlotRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomBattleStartRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomBattleCancelRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomBattleReadyRes& protocol);

void HandleProtocol(CLProtocol::CrossRoomSendInviteLinkRes& protocol);

void HandleProtocol(CLProtocol::CrossWorldInviteRoomReq& protocol);

void HandleProtocol(CLProtocol::WorldScoreWarSyncBattleInfo& protocol);

void HandleProtocol(CLProtocol::World2V2ScoreWarSyncBattleInfo& protocol);

#endif //_WORLD_ROOM_HANDLER_H_