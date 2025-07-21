#include "CSRoomMgr.h"
#include "CLErrorCode.h"
#include "CLSystemValueDataEntry.h"
#include "CLRoomProtocol.h"
#include "CLForbitWord.h"
#include "CLMatchProtocol.h"
#include "CLMsgParser.h"
#include "CLSysNotify.h"
#include "GameZoneMgr.h"
#include "CLSceneDataEntry.h"
#include "CLSceneDefine.h"
#include "CLChatProtocol.h"
#include "CSScoreWarMgr.h"

CSRoomMgr::CSRoomMgr()
{
	m_MinID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MIN);
	m_MaxID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MAX);
	//m_MinID = 1000;
	//m_MaxID = 9999;
	m_CurrentID = m_MinID;
}

CSRoomMgr::~CSRoomMgr()
{
	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		CSRoom* room = iter->second;
		++iter;
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
	m_Rooms.clear();

	while (!m_NotUseRooms.empty())
	{
		CSRoom* room = m_NotUseRooms.front();
		m_NotUseRooms.pop_front();
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
}

UInt32 CSRoomMgr::GetRoomList(RoomListInfo& info, UInt32 startIndex, UInt32 count, UInt32 playerLevel, UInt32 playerSeasonLevel, RoomStatus status, RoomType type, bool isPassword)
{
	//排序规则
	struct GetRoomListCompare
	{
		bool operator() (CSRoom* RoomA, CSRoom* RoomB)
		{
			//RoomA先序与RoomB 则返回true
			if (!RoomA->IsFull() && RoomB->IsFull())
			{
				return true;
			}
			return false;
		}
	};

	std::map<RoomStatus, std::vector<CSRoom*>>::iterator iterStatus = m_RoomByStatus.begin();
	while (iterStatus != m_RoomByStatus.end())
	{
		if (iterStatus->first != status && status != RoomStatus::ROOM_STATUS_INVALID)
		{
			++iterStatus;
			continue;
		}

		std::stable_sort(iterStatus->second.begin(), iterStatus->second.end(), GetRoomListCompare());

		UInt32 roomSize = iterStatus->second.size();
		if (roomSize > 0 && startIndex <= roomSize - 1)
		{
			info.startIndex = startIndex;
			info.total = 0;
			UInt32 index = 0;
			UInt32 tempCount = 0;
			std::vector<CSRoom*>::iterator iter = iterStatus->second.begin();
			while (iter != iterStatus->second.end())
			{
				CSRoom* room = *iter;
				++iter;
				if (room == NULL) continue;

				if (playerLevel != 0 && room->GetLimitPlayerLevel() > playerLevel)
					continue;
				if (playerSeasonLevel != 0 && room->GetLimitPlayerSeasonLevel() > playerSeasonLevel)
					continue;
				if (type == RoomType::ROOM_TYPE_INVALID)
					continue;
				if (type == RoomType::ROOM_TYPE_THREE_FREE || type == RoomType::ROOM_TYPE_THREE_TUMBLE)
				{
					if (room->GetRoomType() != RoomType::ROOM_TYPE_THREE_FREE && room->GetRoomType() != RoomType::ROOM_TYPE_THREE_TUMBLE)
						continue;
				}
				else
				{
					if (room->GetRoomType() != type)
						continue;
				}

				if (status != RoomStatus::ROOM_STATUS_INVALID && room->IsFull())
					continue;

				if (index >= info.startIndex && tempCount < count)
				{
					RoomSimpleInfo tempRoomInfo;
					room->GetRoomSimpleInfo(tempRoomInfo);
					info.rooms.push_back(tempRoomInfo);
					tempCount++;
				}
				++index;
				info.total++;
			}
		}
		++iterStatus;
	}

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::CreateRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (GetPlayerRoomID(player.id) != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;
	if (roomType <= RoomType::ROOM_TYPE_INVALID || roomType >= RoomType::ROOM_TYPE_MAX) return ErrorCode::ROOM_SYSTEM_TYPE_ERROR;
	if (roomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR && player.level < SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SCORE_WAR_LEVEL)) return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_LEVEL;

	UInt32 notifyId = 3203;
	if (roomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		notifyId = 3207;
	}

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(notifyId);
	if (name.compare(player.name + nameSuffix) != 0)
	{
		nRes = IsNameValid(name);
		if (nRes != ErrorCode::SUCCESS)
		{
			return nRes;
		}
	}

	if (!password.empty())
	{
		nRes = IsPasswordValid(password);
		if (nRes != ErrorCode::SUCCESS)
		{
			return nRes;
		}
	}

	CSRoom* room = NULL;
	while (!m_NotUseRooms.empty())
	{
		room = m_NotUseRooms.front();
		m_NotUseRooms.pop_front();

		if (room != NULL)
		{
			break;
		}
	}

	if (room == NULL)
	{
		if (m_CurrentID > m_MaxID) return ErrorCode::ROOM_SYSTEM_CREATE_ROOM_MAX;
		room = new CSRoom(m_CurrentID, roomType, name, isPlayerLevel, playerLevel, isPlayerSeasonLevel, playerSeasonLevel);
		if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
		++m_CurrentID;
	}
	else
	{
		if (room->GetRoomStatus() != RoomStatus::ROOM_STATUS_INVALID)
		{
			return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
		}
		room->SetRoomType(roomType);
		room->SetRoomStatus(RoomStatus::ROOM_STATUS_OPEN);
		room->SetName(name);
		room->SetIsLimitPlayerLevel(isPlayerLevel);
		room->SetLimitPlayerLevel(playerLevel);
		room->SetIsLimitPlayerSeasonLevel(isPlayerSeasonLevel);
		room->SetLimitPlayerSeasonLevel(playerSeasonLevel);
	}

	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		return ErrorCode::ROOM_SYSTEM_ROOM_EXIST;
	}

	room->InitSlot();
	room->SetOwnerId(player.id);
	room->SetOwnerOccu(player.occu);
	room->SetOwnerSeasonLevel(player.seasonLevel);
	room->JoinRoom(zone, player);

	m_Rooms.insert(std::make_pair(room->GetID(), room));
	m_RoomByStatus[room->GetRoomStatus()].push_back(room);

	room->SetCreateTime((UInt32)CURRENT_TIME.Sec());
	room->SetPassword(password);

	room->GetRoomInfo(roomInfo);

	room->SendRoomCreateUdpLog(zone, player.id);
	room->SendRoomJoinUdpLog(zone, player.id);

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::UpdateRoom(RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (roomId != GetPlayerRoomID(player.id)) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	if (room->GetOwnerId() != player.id)
	{
		return ErrorCode::ROOM_SYSTEM_PERMISSION_UPDATE;
	}

	//现在不能修改房间类型
	//room->SetRoomType(roomType);

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(3203);
	if (name.compare(player.name + nameSuffix) != 0)
	{
		nRes = IsNameValid(name);
		if (nRes != ErrorCode::SUCCESS)
		{
			return nRes;
		}
	}

	if (!password.empty())
	{
		nRes = IsPasswordValid(password);
		if (nRes != ErrorCode::SUCCESS)
		{
			return nRes;
		}
	}

	room->SetName(name);
	room->SetPassword(password);
	room->SetIsLimitPlayerLevel(isPlayerLevel);
	room->SetLimitPlayerLevel(playerLevel);
	room->SetIsLimitPlayerSeasonLevel(isPlayerSeasonLevel);
	room->SetLimitPlayerSeasonLevel(playerSeasonLevel);

	room->GetRoomInfo(roomInfo);

	//同步给房间里的其他玩家
	room->SyncRoomSimpleInfo();

	return ErrorCode::SUCCESS;
}


UInt32 CSRoomMgr::DismissRoom(GameZone* zone, ObjID_t playerId, UInt32 roomId)
{
	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	UInt32 nRes = room->DismissRoom();
	if (nRes == ErrorCode::SUCCESS)
	{
		RecycleRoom(room);

		room->SendRoomDeleteUdpLog(zone, playerId);
	}
	return nRes;
}


UInt32 CSRoomMgr::JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, std::string password, UInt32 createTime)
{
	if (GetPlayerRoomID(player.id) != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	CSRoom* room = GetRoom(roomId);
	if (room == NULL)
	{
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR && !CSScoreWarMgr::Instance()->IsJoinRoom())
	{
		return ErrorCode::SCORE_WAR_STATUS_END;
	}

	if (createTime != 0 && room->GetCreateTime() != createTime)
	{
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (player.level < room->GetLimitPlayerLevel())
	{
		return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_LEVEL;
	}

	if (player.seasonLevel < room->GetLimitPlayerSeasonLevel())
	{
		return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_SEASON_LEVEL;
	}

	if (!IsRoomOpen(room->GetRoomStatus()))
	{
		return ErrorCode::ROOM_SYSTEM_JOIN_BATTLE_START;
	}

	UInt32 nRes = room->CheckKickOutInfo(player);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	nRes = room->CheckPassword(password);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	nRes = room->JoinRoom(zone, player, group);
	if (nRes == ErrorCode::SUCCESS)
	{
		room->GetRoomInfo(roomInfo);
		room->SendRoomJoinUdpLog(zone, player.id);
	}
	return nRes;
}

UInt32 CSRoomMgr::QuickJoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType)
{
	if (GetPlayerRoomID(player.id) != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	if (roomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR && !CSScoreWarMgr::Instance()->IsJoinRoom())
	{
		return ErrorCode::SCORE_WAR_STATUS_END;
	}

	CSRoom* joinRoom = NULL;

	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		CSRoom* room = iter->second;
		++iter;
		if (room == NULL) continue;

		if (roomType != room->GetRoomType())
		{
			continue;
		}
		if (!IsRoomOpen(room->GetRoomStatus()))
		{
			continue;
		}
		if (!room->GetPassword().empty())
		{
			continue;
		}
		if (room->GetLimitPlayerLevel() > player.level)
		{
			continue;
		}
		if (room->GetLimitPlayerSeasonLevel() > player.seasonLevel)
		{
			continue;
		}
		UInt32 playerSize = 0;
		UInt32 playerMaxSize = 0;
		room->GetPlayerSize(playerSize, playerMaxSize);
		if (playerMaxSize <= playerSize)
		{
			continue;
		}

		UInt32 nRes = room->CheckKickOutInfo(player);
		if (nRes != ErrorCode::SUCCESS)
		{
			continue;
		}

		
		if (joinRoom == NULL)
		{
			joinRoom = room;
		}
		else
		{
			do
			{
				//最后加入刚刚退出的房间
				if (player.quitRoomId == joinRoom->GetID() && player.quitRoomTime + ROOM_QUIT_TIME_INTERVAL > ((UInt32)CURRENT_TIME.Sec()))
				{
					joinRoom = room;
					break;
				}

				//优先加入缺少人数少的房间
				UInt32 joinRoomPlayerSize = 0;
				UInt32 joinRoomPlayerMaxSize = 0;
				joinRoom->GetPlayerSize(joinRoomPlayerSize, joinRoomPlayerMaxSize);
				if ((playerMaxSize - playerSize) < (joinRoomPlayerMaxSize - joinRoomPlayerSize))
				{
					joinRoom = room;
					break;
				}

				//优先加入等候长的房间
				if (joinRoom->GetOpenTime() > room->GetOpenTime())
				{
					joinRoom = room;
					break;
				}
			} while (0);
		}
	}

	if (joinRoom != NULL)
	{
		UInt32 nRes = joinRoom->JoinRoom(zone, player, RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
		if (nRes == ErrorCode::SUCCESS)
		{
			joinRoom->GetRoomInfo(roomInfo);
		}
		return nRes;
	}

	//根据快速加入的房间类型返回错误码.
	if (RoomType::ROOM_TYPE_THREE_FREE == roomType)
	{
		return ErrorCode::ROOM_SYSTEM_QUIT_JOIN_ROOM_NOT_EXIST;
	}
	return ErrorCode::ROOM_SYSTEM_QUIT_JOIN_MATCH_NOT_EXIST;
}

UInt32 CSRoomMgr::QuitRoom(GameZone* zone, RoomPlayerInfo& player)
{
	UInt32 roomId = player.roomId;
	if (roomId == 0)
	{
		ErrorLogStream << "playerId=" << player.id << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}
	//player->SetRoomId(0);
	CSRoom* room = GetRoom(roomId);
	if (room == NULL)
	{
		ErrorLogStream << "playerId=" << player.id << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (room->GetRoomStatus() == RoomStatus::ROOM_STATUS_MATCH)
	{
		BattleMatchCencel(zone, player);
		room->SendMatchCencelNotify(player, 3204);
	}

	room->QuitRoom(zone, player);
	if (zone != NULL)
	{
		room->SendRoomQuitUdpLog(zone, player.id);
	}

	//处理所有玩家退出的情况
	UInt32 playerMaxSize = 0;
	UInt32 playerSize = 0;
	room->GetPlayerSize(playerSize, playerMaxSize);
	if (playerSize == 0)
	{
		RecycleRoom(room);

		if (zone != NULL)
		{
			room->SendRoomDeleteUdpLog(zone, player.id);
		}
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::KickOutRoom(GameZone* zone, RoomPlayerInfo& player, UInt64 kickPlayerId)
{
	if (player.id == kickPlayerId) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT_SELF;
	UInt32 roomId = player.roomId;

	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	RoomPlayerInfo kickPlayer;
	if (!room->GetPlayerById(kickPlayer, kickPlayerId)) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;

	if (player.id != room->GetOwnerId()) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT;

	GameZone* kickZone = GameZoneMgr::Instance()->FindGameZone(kickPlayer.connId);
	if (kickZone != NULL)
	{
		QuitRoom(kickZone, kickPlayer);
	}

	room->SyncKickOutInfo(player, kickPlayer, RoomQuitReason::ROOM_QUIT_OWNER_KICK_OUT);

	return ErrorCode::SUCCESS;
}

void CSRoomMgr::KickOutRoomByPlayerId(UInt64 playerId)
{
	std::map<ObjID_t, UInt32>::iterator iter = m_PlayerMap.find(playerId);
	if (iter == m_PlayerMap.end())
		return;

	UInt32 roomId = iter->second;
	CSRoom* room = GetRoom(roomId);
	if (room != NULL)
	{
		RoomPlayerInfo info;
		room->GetPlayerById(info, playerId);
		QuitRoom(NULL, info);

		InfoLogStream << "CSRoomMgr::KickOutRoomByPlayerId RoomId:"<<roomId<<" PlayerId:"<<playerId<< LogStream::eos;
	}
}

//邀请玩家
UInt32 CSRoomMgr::InviteJoin(RoomPlayerInfo& player, UInt64 joinPlayerId)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR && !CSScoreWarMgr::Instance()->IsJoinRoom())
	{
		return ErrorCode::SCORE_WAR_STATUS_END;
	}

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	RoomPlayerInfo joinPlayer;
	if (room->GetPlayerById(joinPlayer, joinPlayerId))
	{
		return ErrorCode::ROOM_SYSTEM_BE_INVITE_IN_ROOM;
	}

	/*UInt32 nRes = room->InvitePlayer(player.id, joinPlayerId);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}*/

	CLProtocol::WorldSyncRoomInviteInfo sync;
	sync.roomId = room->GetID();
	sync.roomName = room->GetName();
	sync.roomType = (UInt8)room->GetRoomType();
	sync.inviterId = player.id;
	sync.inviterLevel = player.level;
	sync.inviterName = player.name;
	sync.inviterOccu = player.occu;
	sync.inviterAwaken = player.awaken;

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;

	room->GetPlayerSize(playerSize, playerMaxSize);

	sync.playerSize = (UInt8)playerSize;
	sync.playerMaxSize = (UInt8)playerMaxSize;
	sync.group = room->GetRoomSlotGroup(sync.inviterId);

	//GameZoneMgr::Instance()->TransmitToWorld(sync);

	CLProtocol::CrossWorldInviteRoomReq req;
	req.invitePlayer = player.id;
	req.playerId = joinPlayerId;
	req.roomId = room->GetID();
	if (sync.Encode(req.packet))
	{
		GameZoneMgr::Instance()->TransmitToWorld(req);
	}

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::BeInviteJoin(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, UInt64 invitePlayerId, bool isAccept)
{
	if (player.roomId != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	UInt32 nRes = room->CheckInviteTimeout(invitePlayerId, player.id);
	room->DeleteInviteInfo(invitePlayerId, player.id);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	if (isAccept)
	{
		nRes = room->JoinRoom(zone, player, group);
		if (nRes == ErrorCode::SUCCESS)
		{
			room->GetRoomInfo(roomInfo);
			if (zone != NULL)
			{
				room->SendRoomJoinUdpLog(zone, player.id);
			}
			
		}
	}
	else
	{
		if (zone != NULL)
		{
			CLProtocol::WorldSyncRoomBeInviteInfo sync;
			sync.isAccept = isAccept;
			sync.playerId = player.id;
			zone->SendToPlayer(player.id, sync);
		}
		nRes = ErrorCode::ROOM_SYSTEM_BE_INVITE_REFUSE;
	}

	return nRes;
}

//转让房主
UInt32 CSRoomMgr::ChangeOwner(RoomPlayerInfo& player, UInt64 newOwnerId)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	if (room->GetOwnerId() != player.id) return ErrorCode::ROOM_SYSTEM_PERMISSION_CHANGE_OWNER;
	room->ChangeOwner(newOwnerId);
	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::CloseSlot(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	if (room->GetOwnerId() != player.id) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT;

	return room->CloseSlot(zone, player, slotGroup, index);
}

UInt32 CSRoomMgr::SwapSlot(RoomPlayerInfo& player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	return room->SwapSlot(player, playerId, slotGroup, index);
}

UInt32 CSRoomMgr::ResponseSwapSlot(RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	return room->ResponseSwapSlot(player, slotGroup, index, isAccept);
}

UInt32 CSRoomMgr::BattleReadyStart(GameZone* zone, RoomPlayerInfo& player)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (room->GetOwnerId() != player.id) return ErrorCode::ROOM_SYSTEM_PERMISSION_START_BATTLE;

	UInt32 nRes = room->CheckStartReady();
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	nRes = room->BattleReadyStart();
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	nRes = BattleReadyResult(zone, player, RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_ACCEPT);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::BattleMatchCencel(GameZone* zone, RoomPlayerInfo& player)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;

	if (zone != NULL)
	{
		CLProtocol::MatchServerMatchCancelReq msReq;
		msReq.id = player.id;
		msReq.needReturn = false;
		zone->TransmitToWorld(msReq);
	}

	OnCancleRoomMatch(player);

	room->SendMatchCencelNotify(player, 3206);

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::BattleReadyResult(GameZone* zone, RoomPlayerInfo& player, RoomSlotReadyStatus slotStatus)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	UInt32 nRes = room->BattleReadyResult(player, slotStatus);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	if (IsSlotReadyRefuse(slotStatus))
	{
		ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
		room->SyncRoomSimpleInfo();
		room->UpdateSlotReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
		return ErrorCode::SUCCESS;
	}

	nRes = room->CheckBattleReadySlotStatus();
	if (nRes == ErrorCode::SUCCESS)
	{
		nRes = room->BattleMatchStart();
		if (nRes != ErrorCode::SUCCESS)
		{
			ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
			room->SyncRoomSimpleInfo();
			room->UpdateSlotReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
			return nRes;
		}

		UInt32 playerSize = 0;
		UInt32 playerMaxSize = 0;
		room->GetPlayerSize(playerSize, playerMaxSize);
		room->SendRoomMatchStartUdpLog(zone, player.id, playerSize);
	}
	else if (nRes == ErrorCode::ROOM_SYSTEM_READY_SLOT_NOT_READY)
	{
		nRes = ErrorCode::SUCCESS;
	}

	return nRes;
}

UInt32 CSRoomMgr::SendInviteLink(RoomPlayerInfo& player, UInt32 roomId, UInt8 channel)
{
	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR && !CSScoreWarMgr::Instance()->IsJoinRoom())
	{
		return ErrorCode::SCORE_WAR_STATUS_END;
	}

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	//if (room->GetOwnerId() != player->GetID()) return ErrorCode::ROOM_SYSTEM_PERMISSION_NOT_SEND_INVITE_LINK;

	SystemSuffixesType suffixesType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_ROOM_INVITE;
	if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		suffixesType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_SCORE_WAR_INVITE;
	}
	else if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		suffixesType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_THREE_TUMBLE;
	}

	std::string systemLinkStr = "";
	MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, suffixesType, room->GetID(), room->GetCreateTime());

	UInt32 notifyId = 3201;
	if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_MATCH)
	{
		notifyId = 3202;
	}
	else if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		notifyId = 3208;
	}
	else if (room->GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		notifyId = 3209;
	}

	std::string content = SysNotifyMgr::Instance()->MakeString(notifyId, systemLinkStr);
	CLProtocol::SceneSyncChat protocol;
	protocol.channel = channel;
	protocol.objid = player.id;
	protocol.objname = player.name;
	protocol.occu = player.occu;
	protocol.level = player.level;
	protocol.word = content;
	protocol.bLink = true;
	protocol.isGm = 0;
	protocol.voiceKey = "";
	protocol.voiceDuration = 0;

	if (channel == CHAT_CHANNEL_TRIBE)
	{
		UInt64 targetId = 0;
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::WORLD_TRANSMIT_CHAT);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			stream & targetId & protocol;
		}
		catch (Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return ErrorCode::SUCCESS;
		}
		packet->SetSize(stream.GetPos());
		
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
		if (zone != NULL)
		{
			zone->TransmitToWorld(packet);
		}

		Avalon::Packet::Destroy(packet);
	}
	else if (channel == CHAT_CHANNEL_AROUND)
	{
		GameZoneMgr::Instance()->TransmitToScene(protocol);
	}
	else
	{
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
		if (zone != NULL)
		{
			zone->TransmitToScene(protocol);
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::UpdateRoomMatchInfo(UInt32 roomId, ObjID_t playerId, UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount)
{
	CSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	UInt32 nRes = room->UpdateMatchInfo(playerId, loseStreak, winStreak, matchScore, battleCount);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	nRes = room->CheckBattleReadySlotStatus();
	if (nRes == ErrorCode::SUCCESS)
	{
		nRes = room->BattleMatchStart();
		if (nRes != ErrorCode::SUCCESS)
		{
			ChangeRoomStatus(room, RoomStatus::ROOM_STATUS_OPEN);
			room->SyncRoomSimpleInfo();
			return nRes;
		}
		

		RoomPlayerInfo ownerPlayer;
		if (room->GetPlayerById(ownerPlayer, playerId))
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(ownerPlayer.connId);
			if (zone != NULL)
			{
				UInt32 playerSize = 0;
				UInt32 playerMaxSize = 0;
				room->GetPlayerSize(playerSize, playerMaxSize);
				room->SendRoomMatchStartUdpLog(zone, playerId, playerSize);
			}
		}
	}
	else if (nRes == ErrorCode::ROOM_SYSTEM_READY_SLOT_NOT_READY)
	{
		nRes = ErrorCode::SUCCESS;
	}

	return nRes;
}

CSRoom* CSRoomMgr::GetRoom(UInt32 roomId)
{
	if (roomId > m_MaxID || roomId < m_MinID) return NULL;
	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.find(roomId);
	if (iter != m_Rooms.end())
	{
		return iter->second;
	}
	return NULL;
}

void CSRoomMgr::RecycleRoom(CSRoom* room)
{
	if (room == NULL) return;
	room->SetRoomStatus(RoomStatus::ROOM_STATUS_INVALID);

	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		m_Rooms.erase(iter);
	}

	std::map<RoomStatus, std::vector<CSRoom*>>::iterator iterStatus = m_RoomByStatus.begin();
	while (iterStatus != m_RoomByStatus.end())
	{
		for (UInt32 i = 0; i < iterStatus->second.size(); ++i)
		{
			if (iterStatus->second[i] != NULL && iterStatus->second[i]->GetID() == room->GetID())
			{
				iterStatus->second.erase(iterStatus->second.begin() + i);
				break;
			}
		}
		++iterStatus;
	}

	m_NotUseRooms.push_back(room);

	InfoLogStream << "RecycleRoom " << " room(" << room->GetID() << "|" << room->GetRoomStatus() << ")." << LogStream::eos;
}

void CSRoomMgr::ChangeRoomStatus(CSRoom* room, RoomStatus roomStatus)
{
	if (room == NULL) return;
	RemoveStatusRoom(room->GetRoomStatus(), room);
	room->SetRoomStatus(roomStatus);
	if (roomStatus == RoomStatus::ROOM_STATUS_OPEN)
	{
		room->SetOpenTime((UInt32)CURRENT_TIME.Sec());
	}
	InsertStatusRoom(room);
}

void CSRoomMgr::StopMatch(RoomType type)
{
	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		CSRoom* room = iter->second;
		++iter;
		if (room == NULL)
		{
			continue;
		}
		if (room->GetRoomType() != type)
		{
			continue;
		}
		if (room->GetRoomStatus() != RoomStatus::ROOM_STATUS_MATCH)
		{
			continue;
		}

		ObjID_t playerId = room->GetOwnerId();
		RoomPlayerInfo info;
		if (room->GetPlayerById(info, playerId))
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(info.connId);
			if (zone != NULL)
			{
				BattleMatchCencel(zone, info);
			}
		}
	}
}


void CSRoomMgr::AddPlayerMap(ObjID_t playerId, UInt32 roomId)
{
	std::map<ObjID_t, UInt32>::iterator iter = m_PlayerMap.find(playerId);
	if (iter != m_PlayerMap.end())
	{
		iter->second = roomId;
	}
	else
	{
		m_PlayerMap.insert(std::make_pair(playerId, roomId));
	}
}

void CSRoomMgr::DelPlayerMap(ObjID_t playerId)
{
	std::map<ObjID_t, UInt32>::iterator iter = m_PlayerMap.find(playerId);
	if (iter != m_PlayerMap.end())
	{
		m_PlayerMap.erase(iter);
	}
}

UInt32 CSRoomMgr::GetPlayerRoomID(ObjID_t playerId)
{
	std::map<ObjID_t, UInt32>::iterator iter = m_PlayerMap.find(playerId);
	if (iter != m_PlayerMap.end())
	{
		return iter->second;
	}

	return 0;
}

void CSRoomMgr::OnPlayerOnline(GameZone* zone, RoomPlayerInfo& player)
{
	if (player.roomId == 0 || zone==NULL) return;
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL)
	{
		CLProtocol::CrossSyncRoomPlayerInfo sync;
		sync.playerId = player.id;
		sync.roomId = 0;
		sync.quitRoomId = player.roomId;
		sync.quitRoomTime = (UInt32)CURRENT_TIME.Sec();
		zone->TransmitToWorld(sync);
		return;
	}

	QuitRoom(zone, player);
}

void CSRoomMgr::OnPlayerOffline(GameZone* zone, RoomPlayerInfo& player)
{
	CSRoom* room = GetRoom(player.roomId);
	if (room != NULL)
	{
		QuitRoom(zone, player);
	}
	else
	{
		CLProtocol::CrossSyncRoomPlayerInfo sync;
		sync.playerId = player.id;
		sync.roomId = 0;
		sync.quitRoomId = player.roomId;
		sync.quitRoomTime = (UInt32)CURRENT_TIME.Sec();
		zone->TransmitToWorld(sync);
	}
}

void CSRoomMgr::OnDisconnect(UInt32 connId)
{
	// 某个服关闭，把来自该服务器所有玩家都踢掉
	std::set<UInt32> dissRoom;
	for (auto& iter : m_Rooms)
	{
		CSRoom* room = iter.second;
		if (NULL != room)
		{
			room->QuitPlayerByConnId(connId, dissRoom);
		}
	}

	for (auto& id : dissRoom)
	{
		DismissRoom(NULL, 0, id);
	}
}

void CSRoomMgr::OnDisconnect(GameZone* zone, RoomPlayerInfo& player)
{
	if (player.roomId == 0) return;
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL)
	{
		CLProtocol::CrossSyncRoomPlayerInfo sync;
		sync.playerId = player.id;
		sync.roomId = 0;
		sync.quitRoomId = player.roomId;
		sync.quitRoomTime = (UInt32)CURRENT_TIME.Sec();
		zone->TransmitToWorld(sync);
		return;
	}

	//取消匹配
	if (room->GetRoomStatus() == RoomStatus::ROOM_STATUS_MATCH)
	{
		BattleMatchCencel(zone, player);
		room->SendMatchCencelNotify(player, 3205);
	}

	room->OnDisconnect(player);
}

void CSRoomMgr::OnReconnectSuccess(GameZone* zone, RoomPlayerInfo& player)
{
	if (player.roomId == 0) return;
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL)
	{
		CLProtocol::CrossSyncRoomPlayerInfo sync;
		sync.playerId = player.id;
		sync.roomId = 0;
		sync.quitRoomId = player.roomId;
		sync.quitRoomTime = (UInt32)CURRENT_TIME.Sec();
		zone->TransmitToWorld(sync);
		return;
	}
	room->OnReconnectSuccess(player);
}

void CSRoomMgr::OnChangeScene(ObjID_t playerId, UInt32 sceneId)
{
	InfoLogStream << "CSRoomMgr::OnChangeScene  playerId = " << playerId << " OnChangeScene sceneId = " << sceneId << ")." << LogStream::eos;
	UInt32 roomId = 0;
	std::map<ObjID_t, UInt32>::iterator iter = m_PlayerMap.find(playerId);
	if (iter != m_PlayerMap.end())
	{
		roomId = iter->second;
	}
	else
	{
		return;
	}
	InfoLogStream << "CSRoomMgr::OnChangeScene  playerId = " << playerId << " OnChangeScene roomId = " << roomId << ")." << LogStream::eos;

	CSRoom* room = GetRoom(roomId);
	if (room == NULL)
	{
		return;
	}

	RoomPlayerInfo player;
	if (!room->GetPlayerById(player, playerId))
	{
		return;
	}

	InfoLogStream << "CSRoomMgr::OnChangeScene  playerId = " << playerId << " OnChangeScene old SceneId = " << player.sceneId << ")." << LogStream::eos;
	/*
	bool isQuitRoom = false;

	SceneDataEntry* oldScene = SceneDataEntryMgr::Instance()->FindEntry(player.sceneId);
	if (oldScene != NULL)
	{
		if (oldScene->subType == SceneSubType::SCENE_SUB_ROOM)
		{
			isQuitRoom = true;
		}
	}

	SceneDataEntry* newScene = SceneDataEntryMgr::Instance()->FindEntry(sceneId);
	if (newScene != NULL)
	{
		if (isQuitRoom && newScene->type != SceneType::SCENE_TYPE_PK)
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
			if (zone != NULL)
			{
				QuitRoom(zone, player);
			}
		}
		else
		{
			player.sceneId = sceneId;
			room->UpdateSceneId(player);
		}
	}
	*/

	SceneDataEntry* newScene = SceneDataEntryMgr::Instance()->FindEntry(sceneId);
	if (newScene != NULL)
	{
		if ( newScene->type == SceneType::SCENE_TYPE_PK ||
			(newScene->subType == SceneSubType::SCENE_SUB_ROOM && room->GetRoomType() == RoomType::ROOM_TYPE_THREE_FREE) ||
			(newScene->subType == SceneSubType::SCENE_SUB_SCORE_WAR && room->GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR) ||
			(newScene->subType == SceneSubType::SCENE_SUB_ROOM && room->GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE))
		{
			player.sceneId = sceneId;
			room->UpdateSceneId(player);
		}
		else
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
			if (zone != NULL)
			{
				QuitRoom(zone, player);
			}
		}
	}
}

void CSRoomMgr::OnTick(const Avalon::Time& now)
{
	std::map<UInt32, CSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		CSRoom* room = iter->second;
		++iter;
		if (room == NULL)
		{
			continue;
		}

		room->OnTick(now);
	}
}

void CSRoomMgr::OnStartRoomRace(GameZone* zone, RoomPlayerInfo& player)
{
	InfoLogStream << "playerId = " << player.id << " OnStartRoomRace roomId = " << player.roomId << ")." << LogStream::eos;
	if (player.roomId == 0) return;
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return;

	room->OnStartRoomRace();

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	room->GetPlayerSize(playerSize, playerMaxSize);
	room->SendRoomBattleStartUdpLog(zone, player.id, playerSize);
}

void CSRoomMgr::OnCancleRoomMatch(RoomPlayerInfo& player)
{
	if (player.roomId == 0) return;
	InfoLogStream << "playerId = " << player.id << " OnCancleRoomMatch roomId = " << player.roomId << ")." << LogStream::eos;
	CSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return;
	room->OnCancleRoomMatch();
}

void CSRoomMgr::RemoveStatusRoom(RoomStatus status, CSRoom* room)
{
	if (room == NULL) return;
	std::map<RoomStatus, std::vector<CSRoom*>>::iterator iterStatus = m_RoomByStatus.find(status);
	if (iterStatus != m_RoomByStatus.end())
	{
		std::vector<CSRoom*>::iterator iterRoom = iterStatus->second.begin();
		while (iterRoom != iterStatus->second.end())
		{
			if (*iterRoom != NULL)
			{
				if (room->GetID() != (*iterRoom)->GetID())
				{
					++iterRoom;
					continue;
				}
			}
			iterStatus->second.erase(iterRoom);
			break;
		}
	}
}

void CSRoomMgr::InsertStatusRoom(CSRoom* room)
{
	if (room == NULL) return;
	std::map<RoomStatus, std::vector<CSRoom*>>::iterator iterStatus = m_RoomByStatus.find(room->GetRoomStatus());
	if (iterStatus != m_RoomByStatus.end())
	{
		iterStatus->second.push_back(room);
	}
	else
	{
		std::vector<CSRoom*> rooms;
		rooms.push_back(room);
		m_RoomByStatus.insert(std::make_pair(room->GetRoomStatus(), rooms));
	}
}

bool CSRoomMgr::IsStrValid(const std::string& str, UInt32 maxWidth, bool isName)
{
	if (isName && !CheckNameValid(str.c_str(), maxWidth, true))
	{
		return false;
	}

	char acStr[1024];
	memset(acStr, 0, sizeof(acStr));
	avalon_strncpy(acStr, str.c_str(), sizeof(acStr)-1);
	acStr[sizeof(acStr)-1] = 0;

	if (ForbitWordParser::Instance()->Parse(acStr) != FORBIT_WORD_UNCHANGED)
	{
		return false;
	}

	return true;
}

UInt32 CSRoomMgr::IsNameValid(const std::string& name)
{
	//return ErrorCode::SUCCESS;

	if (!IsStrValid(name, ROOM_NAME_MAX_WIDTH, true))
	{
		return ErrorCode::ROOM_SYSTEM_NAME_INVALID;
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSRoomMgr::IsPasswordValid(const std::string& password)
{
	if (password.size() != ROOM_PASSWORD_SIZE) return ErrorCode::ROOM_SYSTEM_PASSWORD_SIZE_ERROR;

	for (UInt32 i = 0; i < password.size(); i++)
	{
		if (!IsNumber(password[i]))
		{
			return ErrorCode::ROOM_SYSTEM_PASSWORD_NOT_NUMBER;
		}
	}
	return ErrorCode::SUCCESS;
}
