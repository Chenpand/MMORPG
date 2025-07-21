#include "CSRoomHandler.h"
#include "CSRoomMgr.h"
#include "GameZoneMgr.h"
#include "CLErrorCode.h"
#include "CSScoreWarMgr.h"

void HandleProtocol(CLProtocol::CrossSyncRoomStartRace& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	CSRoomMgr::Instance()->OnStartRoomRace(zone, protocol.player);

}

void HandleProtocol(CLProtocol::MatchServerPkRoomRes& protocol)
{
	if (protocol.result == ErrorCode::SUCCESS)
	{
		InfoLogStream << " MatchServerPkRoomRes(" << protocol.result << "|" << protocol.roomId << ")." << LogStream::eos;

		if (protocol.roomId == 0)
		{
			CLProtocol::WorldMatchStartRes res;
			res.result = protocol.result;
			for (UInt32 i = 0; i < protocol.playerIds.size(); ++i)
			{
				GameZoneMgr::Instance()->TransmitToPlayer(protocol.playerIds[i], res);
			}
		}
	}
	else
	{
		ErrorLogStream << " MatchServerPkRoomRes(" << protocol.result << "|" << protocol.roomId << ")." << LogStream::eos;
		CSRoom* room = CSRoomMgr::Instance()->GetRoom(protocol.roomId);
		if (room == NULL)
		{
			ErrorLogStream << " MatchServerPkRoomRes(" << protocol.roomId << ") is not exist." << LogStream::eos;
			return;
		}
		CSRoomMgr::Instance()->ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
		room->SyncRoomSimpleInfo();

		CLProtocol::WorldMatchStartRes res;
		res.result = protocol.result;
		for (UInt32 i = 0; i < protocol.playerIds.size(); ++i)
		{
			RoomPlayerInfo player;
			if (room->GetPlayerById(player, protocol.playerIds[i]))
			{
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
				if (zone != NULL)
				{
					zone->SendToPlayer(player.id, res);
				}
			}
		}
	}
}

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol)
{
	InfoLogStream << " MatchServerPkRoomRaceEnd(" << protocol.type << ")." << LogStream::eos;

	for (UInt32 i = 0; i < protocol.results.size(); ++i)
	{
		CSRoom* room = CSRoomMgr::Instance()->GetRoom(protocol.results[i].roomId);
		if (!room)
		{
			continue;
		}

		RoomSlotGroup slotGroup = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
		UInt32 slotIndex = 0;

		if (room->GetRoomStatus() == RoomStatus::ROOM_STATUS_BATTLE)
		{
			if (room->GetRoomSlot(protocol.results[i].info.roleId, slotGroup, slotIndex) == ErrorCode::SUCCESS)
			{
				CSRoomMgr::Instance()->ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
				room->SyncRoomSimpleInfo();
			}
			else
			{
				ErrorLogStream << " MatchServerPkRoomRaceEnd " << protocol.results[i].info.roleId << " not in room(" << protocol.results[i].roomId << ")" << LogStream::eos;
			}
		}
	}
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerOnline& protocol)
{
	InfoLogStream << " CrossSyncRoomPlayerOnline( " << protocol.player.roomId << " | " << protocol.player.id << " )." << LogStream::eos;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	CSRoomMgr::Instance()->OnPlayerOnline(zone, protocol.player);
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerOffline& protocol)
{
	InfoLogStream << " CrossSyncRoomPlayerOffline( " << protocol.player.roomId << " | " << protocol.player.id << " )." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	CSRoomMgr::Instance()->OnPlayerOffline(zone, protocol.player);
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerDisconnect& protocol)
{
	InfoLogStream << " CrossSyncRoomPlayerDisconnect( " << protocol.player.roomId << " | " << protocol.player.id << " )." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	protocol.player.connId = protocol.GetConnID();

	CSRoomMgr::Instance()->OnDisconnect(zone, protocol.player);
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerReconnect& protocol)
{
	InfoLogStream << " CrossSyncRoomPlayerReconnect( " << protocol.player.roomId << " | " << protocol.player.id << " )." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	CSRoomMgr::Instance()->OnReconnectSuccess(zone, protocol.player);
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerChangeScene& protocol)
{
	InfoLogStream << " CrossSyncRoomPlayerChangeScene( " << protocol.player.roomId << " | " << protocol.player.id << " )." << LogStream::eos;
	
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.GetConnID());
	if (zone == NULL) return;

	CSRoomMgr::Instance()->OnChangeScene(protocol.player.id, protocol.sceneId);
	
}

void HandleProtocol(CLProtocol::CrossWorldRoomListReq& req)
{
	InfoLogStream << " CrossWorldRoomListReq( " << req.playerId << " )." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	CLProtocol::CrossWorldRoomListRes res;
	res.playerId = req.playerId;
	res.result = CSRoomMgr::Instance()->GetRoomList(res.roomList, req.startIndex, req.count, req.limitPlayerLevel, req.limitPlayerSeasonLevel, (RoomStatus)req.roomStatus, (RoomType)req.roomType, req.isPassword == 1);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossUpdateRoomReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.playerInfo.connId = req.GetConnID();

	CLProtocol::CrossUpdateRoomRes res;
	res.ownerId = req.playerInfo.id;
	if (req.roomId == 0)
	{
		res.result = CSRoomMgr::Instance()->CreateRoom(zone, req.playerInfo, res.info, (RoomType)req.roomType, req.name, req.password, 
			req.isLimitPlayerLevel, req.limitPlayerLevel, req.isLimitPlayerSeasonLevel, req.limitPlayerSeasonLevel);
		res.isCreate = 1;
	}
	else
	{
		res.result = CSRoomMgr::Instance()->UpdateRoom(req.playerInfo, res.info, req.roomId, (RoomType)req.roomType,req.name, req.password, 
			req.isLimitPlayerLevel, req.limitPlayerLevel, req.isLimitPlayerSeasonLevel, req.limitPlayerSeasonLevel);
		res.isCreate = 0;
	}
	
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossJoinRoomReq& req)
{
	InfoLogStream << " CrossJoinRoomReq(" << req.roomId << "|" << req.roomType << "|" << req.password << ")." << LogStream::eos;

	if (req.roomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR && !CSScoreWarMgr::Instance()->IsJoinRoom())
		return;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;
	
	// 如果玩家在老的房间里，把他从老的房间踢掉
	// CSRoomMgr::Instance()->KickOutRoomByPlayerId(req.playerInfo.id);

	req.playerInfo.connId = req.GetConnID();

	CLProtocol::CrossJoinRoomRes res;
	res.ownerId = req.playerInfo.id;

	if (req.roomId == 0)
	{
		res.result = CSRoomMgr::Instance()->QuickJoinRoom(zone, req.playerInfo, res.info, (RoomType)req.roomType);
	}
	else
	{
		res.result = CSRoomMgr::Instance()->JoinRoom(zone, req.playerInfo, res.info, req.roomId, RoomSlotGroup::ROOM_SLOT_GROUP_INVALID, req.password, req.createTime);
	}

	if (res.result != ErrorCode::SUCCESS)
	{
		res.info.roomSimpleInfo.roomType = req.roomType;
	}

	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossQuitRoomReq& req)
{
	InfoLogStream << " CrossQuitRoomReq(" << req.info.roomId << "|" << req.info.id << ")." << LogStream::eos;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossQuitRoomRes res;
	res.result = CSRoomMgr::Instance()->QuitRoom(zone, req.info);
	res.ownerId = req.info.id;
	zone->TransmitToWorld(res);

}

void HandleProtocol(CLProtocol::CrossKickOutRoomReq& req)
{
	InfoLogStream << " CrossKickOutRoomReq(" << req.info.roomId << "|" << req.info.id << ")." << LogStream::eos;

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossKickOutRoomRes res;
	res.result =  CSRoomMgr::Instance()->KickOutRoom(zone, req.info, req.playerId);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossDismissRoomReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();
	CLProtocol::CrossDismissRoomRes res;
	res.result = CSRoomMgr::Instance()->DismissRoom(zone, req.info.id, req.info.roomId);
	zone->TransmitToWorld(res);
	
}

void HandleProtocol(CLProtocol::CrossInviteJoinRoomReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossInviteJoinRoomRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->InviteJoin(req.info, req.playerId);
	if (res.result != ErrorCode::SUCCESS)
	{
		zone->TransmitToWorld(res);
	}
}

void HandleProtocol(CLProtocol::CrossChangeRoomOwnerReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossChangeRoomOwnerRes res;
	res.result = CSRoomMgr::Instance()->ChangeOwner(req.info, req.playerId);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossBeInviteRoomReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossBeInviteRoomRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->BeInviteJoin(zone, req.info, res.info, req.roomId, (RoomSlotGroup)req.slotGroup, req.invitePlayerId, req.isAccept == 1);
	zone->TransmitToWorld(res);

	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << " CrossBeInviteRoomReq(" << req.roomId << "|" << res.result << ")." << LogStream::eos;
	}

}

void HandleProtocol(CLProtocol::CrossRoomCloseSlotReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomCloseSlotRes res;
	res.result = CSRoomMgr::Instance()->CloseSlot(zone, req.info, (RoomSlotGroup)req.slotGroup, (UInt32)req.index);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossRoomSwapSlotReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomSwapSlotRes res;
	res.result = CSRoomMgr::Instance()->SwapSlot(req.info, res.playerId, (RoomSlotGroup)req.slotGroup, req.index);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossRoomResponseSwapSlotReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomResponseSwapSlotRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->ResponseSwapSlot(req.info, (RoomSlotGroup)req.slotGroup, req.slotIndex, req.isAccept == 1);
	zone->TransmitToWorld(res);

}

void HandleProtocol(CLProtocol::CrossRoomBattleStartReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomBattleStartRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->BattleReadyStart(zone, req.info);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossRoomBattleCancelReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomBattleCancelRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->BattleMatchCencel(zone, req.info);
	zone->TransmitToWorld(res);

}

void HandleProtocol(CLProtocol::CrossRoomBattleReadyReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomBattleReadyRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->BattleReadyResult(zone, req.info, (RoomSlotReadyStatus)req.slotStatus);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossRoomSendInviteLinkReq& req)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(req.GetConnID());
	if (zone == NULL) return;

	req.info.connId = req.GetConnID();

	CLProtocol::CrossRoomSendInviteLinkRes res;
	res.playerId = req.info.id;
	res.result = CSRoomMgr::Instance()->SendInviteLink(req.info, req.roomId, req.channel);
	zone->TransmitToWorld(res);
}

void HandleProtocol(CLProtocol::CrossRoomMatchInfoRes& res)
{
	UInt32 nRes = CSRoomMgr::Instance()->UpdateRoomMatchInfo(res.roomId, res.playerId, res.loseStreak, res.winStreak, res.matchScore, res.battleCount);
	if (nRes != ErrorCode::SUCCESS)
	{
		ErrorLogStream << " CrossRoomMatchInfoRes(" << nRes << ")." << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::CrossWorldInviteRoomRes& res)
{
	CSRoom* room = CSRoomMgr::Instance()->GetRoom(res.roomId);
	if (room == NULL) return;
	
	RoomPlayerInfo player;
	if (room->GetPlayerById(player, res.invitePlayerId))
	{
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
		if (NULL == zone)
			return;

		CLProtocol::CrossInviteJoinRoomRes protocol;
		protocol.playerId = res.invitePlayerId;
		protocol.result = res.result;

		do 
		{
			if (res.result != ErrorCode::SUCCESS)
				break;

			UInt32 nRes = room->InvitePlayer(player.id, res.playerId);
			if (nRes != ErrorCode::SUCCESS)
			{
				res.result = nRes;
				break;
			}

		} while (0);

		zone->TransmitToWorld(protocol);
	}
}

void CrossTransChatFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	UInt32 roomId = 0;
	CLProtocol::SceneSyncChat sync;
	stream & roomId & sync;
	sync.channel = ChatChannelType::CHAT_CHANNEL_TEAM;

	CSRoom* room = CSRoomMgr::Instance()->GetRoom(roomId);
	if (room == NULL) return;
	room->Broadcast(sync);
}
