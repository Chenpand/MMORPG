#include "WorldRoomHandler.h"
#include "WSPlayerMgr.h"
#include "RoomMgr.h"
#include "WSRouter.h"
#include "WSServiceSwitchMgr.h"
#include <CLSettingDefine.h>

void HandleProtocol(CLProtocol::MatchServerPkRoomRes& protocol)
{
	if (RoomMgr::Instance()->IsCross())
	{
		Router::SendToCross(protocol);
		return;
	}

	if (protocol.result == ErrorCode::SUCCESS)
	{
		InfoLogStream << " MatchServerPkRoomRes(" << protocol.result << "|" << protocol.roomId << ")." << LogStream::eos;

		if (protocol.roomId == 0)
		{
			CLProtocol::WorldMatchStartRes res;
			res.result = protocol.result;
			for (UInt32 i = 0; i < protocol.playerIds.size(); ++i)
			{
				WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerIds[i]);
				if (player != NULL)
				{
					player->SendProtocol(res);
				}
			}
		}

		/*
		Room* room = RoomMgr::Instance()->GetRoom(protocol.roomId);
		if (room == NULL)
		{
			ErrorLogStream << " MatchServerPkRoomRes(" << protocol.roomId << ") is not exist." << LogStream::eos;
		}

		room->OnStartRoomRace();
		*/
	}
	else
	{
		ErrorLogStream << " MatchServerPkRoomRes(" << protocol.result << "|" << protocol.roomId << ")." << LogStream::eos;
		Room* room = RoomMgr::Instance()->GetRoom(protocol.roomId);
		if (room == NULL)
		{
			ErrorLogStream << " MatchServerPkRoomRes(" << protocol.roomId << ") is not exist." << LogStream::eos;
			return;
		}
		RoomMgr::Instance()->ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
		room->SyncRoomSimpleInfo();
		
		CLProtocol::WorldMatchStartRes res;
		res.result = protocol.result;
		for (UInt32 i = 0; i < protocol.playerIds.size(); ++i)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerIds[i]);
			if (player != NULL)
			{
				player->SendProtocol(res);
			}
		}
		
	}
}

void HandleProtocol(CLProtocol::MatchServerPkRoomRaceEnd& protocol)
{
	InfoLogStream << " MatchServerPkRoomRaceEnd(" << protocol.type << ")." << LogStream::eos;
	// 发到场景服务器
	Router::BroadcastToScene(protocol);

	//2v2乱斗活动没有房间
	if (protocol.type == PK_2V2_ACTIVITY)
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		Router::SendToCross(protocol);
		return;
	}

	for (UInt32 i = 0; i < protocol.results.size(); ++i)
	{
		Room* room = RoomMgr::Instance()->GetRoom(protocol.results[i].roomId);
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
				RoomMgr::Instance()->ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
				room->SyncRoomSimpleInfo();
			}
			else
			{
				ErrorLogStream << " MatchServerPkRoomRaceEnd " << protocol.results[i].info.roleId << " not in room(" << protocol.results[i].roomId << ")" << LogStream::eos;
			}
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomListReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomListReq(" << protocol.startIndex << "|" << protocol.count << "|" << protocol.limitPlayerLevel << "|"
		<< protocol.limitPlayerSeasonLevel << "|" << protocol.roomStatus << "|" << protocol.roomType << "|" << protocol.isPassword << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_ROOM))
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossWorldRoomListReq req;
		req.playerId = player->GetID();
		req.count = protocol.count;
		req.isPassword = protocol.isPassword;
		req.limitPlayerLevel = protocol.limitPlayerLevel;
		req.limitPlayerSeasonLevel = protocol.limitPlayerSeasonLevel;
		req.roomStatus = protocol.roomStatus;
		req.roomType = protocol.roomType;
		req.startIndex = protocol.startIndex;
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomListRes res;
		res.result = RoomMgr::Instance()->GetRoomList(res.roomList, protocol.startIndex, protocol.count, protocol.limitPlayerLevel, protocol.limitPlayerSeasonLevel, (RoomStatus)protocol.roomStatus, (RoomType)protocol.roomType, protocol.isPassword == 1);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomListRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldUpdateRoomReq(" << protocol.roomId << "|" << protocol.roomType << "|" << protocol.name << "|" 
		<< protocol.limitPlayerLevel << "|" << protocol.limitPlayerSeasonLevel << "|" << protocol.password << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_ROOM))
	{
		player->SendNotify(1300036);
		return;
	}

	if (!player->IsOpenRoomFunction())
	{
		return;
	}

	if (protocol.roomType == RoomType::ROOM_TYPE_THREE_TUMBLE && !WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_3v3_TUMBLE))
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{

		CLProtocol::CrossUpdateRoomReq req;
		req.isLimitPlayerLevel = protocol.isLimitPlayerLevel;
		req.isLimitPlayerSeasonLevel = protocol.isLimitPlayerSeasonLevel;
		req.limitPlayerLevel = protocol.limitPlayerLevel;
		req.limitPlayerSeasonLevel = protocol.limitPlayerSeasonLevel;
		req.name = protocol.name;
		req.password = protocol.password;
		req.roomId = protocol.roomId;
		req.roomType = protocol.roomType;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.playerInfo);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldUpdateRoomRes res;
		if (protocol.roomId == 0)
		{
			res.result = RoomMgr::Instance()->CreateRoom(player, res.info, (RoomType)protocol.roomType, protocol.name, protocol.password, protocol.isLimitPlayerLevel, protocol.limitPlayerLevel, protocol.isLimitPlayerSeasonLevel, protocol.limitPlayerSeasonLevel);
		}
		else
		{
			res.result = RoomMgr::Instance()->UpdateRoom(player, res.info, protocol.roomId, (RoomType)protocol.roomType, protocol.name, protocol.password, protocol.isLimitPlayerLevel, protocol.limitPlayerLevel, protocol.isLimitPlayerSeasonLevel, protocol.limitPlayerSeasonLevel);
		}

		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldUpdateRoomRes(" << res.result << ")." << LogStream::eos;
		}
	}

}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldJoinRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldJoinRoomReq(" << protocol.roomId << "|" << protocol.roomType << "|" << protocol.password << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_ROOM))
	{
		return;
	}

	if (!player->IsOpenRoomFunction())
	{
		return;
	}

	if (protocol.roomType == RoomType::ROOM_TYPE_THREE_TUMBLE && !WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_3v3_TUMBLE))
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossJoinRoomReq req;
		req.createTime = protocol.createTime;
		req.password = protocol.password;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.playerInfo);
		req.roomId = protocol.roomId;
		req.roomType = protocol.roomType;
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldJoinRoomRes res;

		if (protocol.roomId == 0)
		{
			res.result = RoomMgr::Instance()->QuickJoinRoom(player, res.info, (RoomType)protocol.roomType);
		}
		else
		{
			res.result = RoomMgr::Instance()->JoinRoom(player, res.info, protocol.roomId, RoomSlotGroup::ROOM_SLOT_GROUP_INVALID, protocol.password, protocol.createTime);
		}

		if (res.result != ErrorCode::SUCCESS)
		{
			res.info.roomSimpleInfo.roomType = protocol.roomType;
			ErrorLogStream << PLAYERINFO(player) << " WorldJoinRoomRes(" << res.result << ")." << LogStream::eos;
		}

		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQuitRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldQuitRoomReq()." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossQuitRoomReq req;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldQuitRoomRes res;
		res.result = RoomMgr::Instance()->QuitRoom(player);
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldQuitRoomRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldKickOutRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldKickOutRoomReq(" << protocol.playerId << ")." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossKickOutRoomReq req;
		req.playerId = protocol.playerId;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldKickOutRoomRes res;
		res.result = RoomMgr::Instance()->KickOutRoom(player, protocol.playerId);
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldKickOutRoomRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDismissRoomReq& protocol)
{
	//TODO
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldInviteJoinRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldInviteJoinRoomReq(" << protocol.playerId << ")." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossInviteJoinRoomReq req;
		req.playerId = protocol.playerId;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldInviteJoinRoomRes res;
		res.result = RoomMgr::Instance()->InviteJoin(player, protocol.playerId);
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldInviteJoinRoomRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeRoomOwnerReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldChangeRoomOwnerReq(" << protocol.playerId << ")." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossChangeRoomOwnerReq req;
		req.playerId = protocol.playerId;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldChangeRoomOwnerRes res;
		res.result = RoomMgr::Instance()->ChangeOwner(player, protocol.playerId);
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldChangeRoomOwnerRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBeInviteRoomReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldBeInviteRoomReq(" << protocol.roomId << "|" << protocol.isAccept << "|" << protocol.slotGroup << ")." << LogStream::eos;

	if (!player->IsOpenRoomFunction())
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossBeInviteRoomReq req;
		req.invitePlayerId = protocol.invitePlayerId;
		req.isAccept = protocol.isAccept;
		req.roomId = protocol.roomId;
		req.slotGroup = protocol.slotGroup;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldBeInviteRoomRes res;
		res.result = RoomMgr::Instance()->BeInviteJoin(player, res.info, protocol.roomId, (RoomSlotGroup)protocol.slotGroup, protocol.invitePlayerId, protocol.isAccept == 1);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldBeInviteRoomReq(" << res.result << ")." << LogStream::eos;
		}
	}
}


void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomCloseSlotReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomCloseSlotReq(" << protocol.slotGroup << "|" << protocol.index << ")." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomCloseSlotReq req;
		req.index = protocol.index;
		req.slotGroup = protocol.slotGroup;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomCloseSlotRes res;
		res.result = RoomMgr::Instance()->CloseSlot(player, (RoomSlotGroup)protocol.slotGroup, (UInt32)protocol.index);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomCloseSlotRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomSwapSlotReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomSwapSlotReq(" << protocol.slotGroup << "|" << protocol.index << ")." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomSwapSlotReq req;
		req.index = protocol.index;
		req.roomId = protocol.roomId;
		req.slotGroup = protocol.slotGroup;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomSwapSlotRes res;
		res.result = RoomMgr::Instance()->SwapSlot(player, res.playerId, (RoomSlotGroup)protocol.slotGroup, (UInt32)protocol.index);
		player->SendProtocol(res);
		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomSwapSlotRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomResponseSwapSlotReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomSwapSlotReq(" << protocol.isAccept << "|" << protocol.slotGroup << "|" << protocol.slotIndex << ")." << LogStream::eos;
	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomResponseSwapSlotReq req;
		req.isAccept = protocol.isAccept;
		req.slotGroup = protocol.slotGroup;
		req.slotIndex = protocol.slotIndex;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomResponseSwapSlotRes res;
		res.result = RoomMgr::Instance()->ResponseSwapSlot(player, (RoomSlotGroup)protocol.slotGroup, protocol.slotIndex, protocol.isAccept == 1);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomResponseSwapSlotRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleStartReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomBattleStartReq()." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomBattleStartReq req;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomBattleStartRes res;
		res.result = RoomMgr::Instance()->BattleReadyStart(player);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomBattleStartRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleCancelReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomBattleCancelReq()." << LogStream::eos;

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomBattleCancelReq req;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomBattleCancelRes res;
		res.result = RoomMgr::Instance()->BattleMatchCencel(player);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomBattleCancelRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomBattleReadyReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomBattleReadyReq(" << protocol.slotStatus << ")." << LogStream::eos;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_ROOM))
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomBattleReadyReq req;
		req.slotStatus = protocol.slotStatus;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomBattleReadyRes res;
		res.result = RoomMgr::Instance()->BattleReadyResult(player, (RoomSlotReadyStatus)protocol.slotStatus);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomBattleReadyRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRoomSendInviteLinkReq& protocol)
{
	InfoLogStream << PLAYERINFO(player) << " WorldRoomSendInviteLinkReq(" << protocol.roomId << "|" << protocol.channel << ")." << LogStream::eos;

	if (protocol.channel != ChatChannelType::CHAT_CHANNEL_ROOM && 
		protocol.channel != ChatChannelType::CHAT_CHANNEL_TRIBE && 
		protocol.channel != ChatChannelType::CHAT_CHANNEL_TEAM_INVITE &&
		protocol.channel != ChatChannelType::CHAT_CHANNEL_AROUND)
	{
		return;
	}

	if (RoomMgr::Instance()->IsCross())
	{
		CLProtocol::CrossRoomSendInviteLinkReq req;
		req.roomId = protocol.roomId;
		req.channel = protocol.channel;
		RoomMgr::Instance()->GetRoomPlayerInfo(player, req.info);
		Router::SendToCross(req);
	}
	else
	{
		CLProtocol::WorldRoomSendInviteLinkRes res;
		res.result = RoomMgr::Instance()->SendInviteLink(player, protocol.roomId, protocol.channel);
		player->SendProtocol(res);

		if (res.result != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << " WorldRoomSendInviteLinkRes(" << res.result << ")." << LogStream::eos;
		}
	}
}

void HandleProtocol(CLProtocol::CrossSyncRoomPlayerInfo& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		player->SetRoomId(protocol.roomId);
		player->SetQuitRoomId(protocol.quitRoomId);
		player->SetQuitRoomTime(protocol.quitRoomTime);
	}
	
}

void HandleProtocol(CLProtocol::CrossSyncRoomLogInfo& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		player->SendUdpLog("room", LOG_ROOM_FORMAT, protocol.opType, protocol.roomId, protocol.roomType, protocol.reason, protocol.createTime, protocol.playerSize);
	}
}

void HandleProtocol(CLProtocol::CrossSyncRoomNotifyInfo& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player != NULL)
	{
		player->SendNotify(protocol.notifyId, protocol.param1);
	}
}

void HandleProtocol(CLProtocol::CrossWorldRoomListRes& protocol)
{
	InfoLogStream << " CrossWorldRoomListRes(" << protocol.playerId << "|" << protocol.result << ")." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomListRes res;
	res.result = protocol.result;
	res.roomList = protocol.roomList;
	player->SendProtocol(res);

}

void HandleProtocol(CLProtocol::CrossUpdateRoomRes& protocol)
{
	InfoLogStream << " CrossUpdateRoomRes(" << protocol.ownerId << "|" << protocol.result << ")." << LogStream::eos;
	
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.ownerId);
	if (player == NULL) return;

	CLProtocol::WorldUpdateRoomRes res;
	res.info = protocol.info;
	res.result = protocol.result;
	player->SendProtocol(res);
	
}

void HandleProtocol(CLProtocol::CrossJoinRoomRes& protocol)
{
	InfoLogStream << " CrossQuitRoomRes( " << protocol.ownerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.ownerId);
	if (player == NULL) return;

	CLProtocol::WorldJoinRoomRes res;
	res.info = protocol.info;
	res.result = protocol.result;
	
	player->SendProtocol(res);
	
}

void HandleProtocol(CLProtocol::CrossQuitRoomRes& protocol)
{
	InfoLogStream << " CrossQuitRoomRes( " << protocol.ownerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.ownerId);
	if (player == NULL) return;

	CLProtocol::WorldQuitRoomRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
	
}

void HandleProtocol(CLProtocol::CrossKickOutRoomRes& protocol)
{
	InfoLogStream << " CrossKickOutRoomRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldKickOutRoomRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossDismissRoomRes& protocol)
{

}

void HandleProtocol(CLProtocol::CrossInviteJoinRoomRes& protocol)
{
	InfoLogStream << " CrossInviteJoinRoomRes( " << protocol.playerId << "|" << protocol.result << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldInviteJoinRoomRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossChangeRoomOwnerRes& protocol)
{
	InfoLogStream << " CrossChangeRoomOwnerRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldChangeRoomOwnerRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossBeInviteRoomRes& protocol)
{
	InfoLogStream << " CrossBeInviteRoomRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldBeInviteRoomRes res;
	res.result = protocol.result;
	res.info = protocol.info;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomCloseSlotRes& protocol)
{
	InfoLogStream << " CrossRoomCloseSlotRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomCloseSlotRes res;
	res.result = protocol.result;
	player->SendProtocol(res);

}

void HandleProtocol(CLProtocol::CrossRoomSwapSlotRes& protocol)
{
	InfoLogStream << " CrossRoomSwapSlotRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomSwapSlotRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomResponseSwapSlotRes& protocol)
{
	InfoLogStream << " CrossRoomResponseSwapSlotRes( " << protocol.playerId << " )." << LogStream::eos;
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;
	
	CLProtocol::WorldRoomResponseSwapSlotRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomBattleStartRes& protocol)
{
	InfoLogStream << " CrossRoomBattleStartRes( " << protocol.playerId << " )." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomBattleStartRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomBattleCancelRes& protocol)
{
	InfoLogStream << " CrossRoomBattleCancelRes( " << protocol.playerId << " )." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomBattleCancelRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomBattleReadyRes& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomBattleReadyRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossRoomSendInviteLinkRes& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL) return;

	CLProtocol::WorldRoomSendInviteLinkRes res;
	res.result = protocol.result;
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::CrossWorldInviteRoomReq& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL)
	{
		return;
	}

	CLProtocol::CrossWorldInviteRoomRes res;
	res.invitePlayerId = protocol.invitePlayer;
	res.playerId = protocol.playerId;
	res.roomId = protocol.roomId;

	do 
	{
		if (player->GetRoomId() != 0)
		{
			res.result = ErrorCode::ROOM_SYSTEM_BE_INVITE_IN_ROOM;
			break;
		}
			
		if (player->GetGameSetValue<UInt32>(GST_SECRET, SST_DUEL_INVATE))
		{
			res.result = ErrorCode::SETTING_TARGET_SHILED_INVATE;
			break;
		}

		res.result = ErrorCode::SUCCESS;
		player->SendPacket(protocol.packet);

	} while (0);

	Router::SendToCross(res);
}

void HandleProtocol(CLProtocol::WorldScoreWarSyncBattleInfo& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL)
	{
		return;
	}

	player->SetScoreWarBattleCount(protocol.battleCount);
	player->SetScoreWarLoseStreak(protocol.loseStreak);
	player->SetScoreWarWinStreak(protocol.winStreak);
	player->SetMatchScore(protocol.matchScore);

	if (player->GetRoomId() != 0)
	{
		CLProtocol::CrossScoreWarBattleInfoSync sync;
		sync.battleCount = protocol.battleCount;
		sync.loseStreak = protocol.loseStreak;
		sync.matchScore = protocol.matchScore;
		sync.playerId = protocol.playerId;
		sync.roomId = player->GetRoomId();
		sync.winStreak = protocol.winStreak;
		Router::SendToCross(sync);
	}
}

void HandleProtocol(CLProtocol::World2V2ScoreWarSyncBattleInfo& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if (player == NULL)
	{
		return;
	}

	player->SetScoreWarBattleCount(protocol.battleCount);
	player->SetScoreWarLoseStreak(protocol.loseStreak);
	player->SetScoreWarWinStreak(protocol.winStreak);
	player->SetMatchScore(protocol.matchScore);

	if (player->GetRoomId() != 0)
	{
		CLProtocol::Cross2V2ScoreWarBattleInfoSync sync;
		sync.battleCount = protocol.battleCount;
		sync.loseStreak = protocol.loseStreak;
		sync.matchScore = protocol.matchScore;
		sync.playerId = protocol.playerId;
		sync.roomId = player->GetRoomId();
		sync.winStreak = protocol.winStreak;
		Router::SendToCross(sync);
	}
}
