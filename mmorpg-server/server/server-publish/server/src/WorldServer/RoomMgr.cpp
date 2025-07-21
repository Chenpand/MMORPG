#include "RoomMgr.h"
#include "CLErrorCode.h"
#include "CLSystemValueDataEntry.h"
#include "WSPlayerMgr.h"
#include "CLRoomProtocol.h"
#include "CLForbitWord.h"
#include "CLMatchProtocol.h"
#include "WSRouter.h"
#include "CLMsgParser.h"
#include "WSSceneMgr.h"
#include <CLSettingDefine.h>

RoomMgr::RoomMgr()
{
	m_MinID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MIN);
	m_MaxID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MAX);
	//m_MinID = 1000;
	//m_MaxID = 9999;
	m_CurrentID = m_MinID;

	m_IsCross = true;
}

RoomMgr::~RoomMgr()
{
	std::map<UInt32, Room*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		Room* room = iter->second;
		++iter;
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
	m_Rooms.clear();

	while (!m_NotUseRooms.empty())
	{
		Room* room = m_NotUseRooms.front();
		m_NotUseRooms.pop_front();
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
}

UInt32 RoomMgr::GetRoomList(RoomListInfo& info, UInt32 startIndex, UInt32 count, UInt32 playerLevel, UInt32 playerSeasonLevel, RoomStatus status, RoomType type, bool isPassword)
{
	//排序规则
	struct GetRoomListCompare
	{
		bool operator() (Room* RoomA, Room* RoomB)
		{
			//RoomA先序与RoomB 则返回true
			if (!RoomA->IsFull() && RoomB->IsFull())
			{
				return true;
			}
			return false;
		}
	};

	std::map<RoomStatus, std::vector<Room*>>::iterator iterStatus = m_RoomByStatus.begin();
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
			std::vector<Room*>::iterator iter = iterStatus->second.begin();
			while (iter != iterStatus->second.end())
			{
				Room* room = *iter;
				++iter;
				if (room == NULL) continue;

				if (playerLevel != 0 && room->GetLimitPlayerLevel() > playerLevel)
					continue;
				if (playerSeasonLevel != 0 && room->GetLimitPlayerSeasonLevel() > playerSeasonLevel)
					continue;
				if (type != RoomType::ROOM_TYPE_INVALID && room->GetRoomType() != type)
					continue;
				if (room->GetRoomStatus() != RoomStatus::ROOM_STATUS_INVALID && room->IsFull())
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

UInt32 RoomMgr::CreateRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetRoomId() != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;
	if (roomType <= RoomType::ROOM_TYPE_INVALID || roomType >= RoomType::ROOM_TYPE_MAX) return ErrorCode::ROOM_SYSTEM_TYPE_ERROR;

	UInt32 notifyId = 3203;
	if (roomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		notifyId = 3207;
	}

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(notifyId);
	if (name.compare(player->GetName() + nameSuffix) != 0)
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

	Room* room = NULL;
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
		room = new Room(m_CurrentID, roomType, name, isPlayerLevel, playerLevel, isPlayerSeasonLevel, playerSeasonLevel);
		if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
		m_CurrentID++;
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
	room->InitSlot();
	room->SetOwnerId(player->GetID());
	room->SetOwnerOccu(player->GetOccu());
	room->SetOwnerSeasonLevel(player->GetSeasonLv());
	room->JoinRoom(player);

	std::map<UInt32, Room*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		return ErrorCode::ROOM_SYSTEM_ROOM_EXIST;
	}

	m_Rooms.insert(std::make_pair(room->GetID(), room));
	m_RoomByStatus[room->GetRoomStatus()].push_back(room);

	room->SetCreateTime((UInt32)CURRENT_TIME.Sec());
	room->SetPassword(password);

	room->GetRoomInfo(roomInfo);

	player->SendRoomCreateUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
	player->SendRoomJoinUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::UpdateRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	if (room->GetOwnerId() != player->GetID())
	{
		return ErrorCode::ROOM_SYSTEM_PERMISSION_UPDATE;
	}

	//现在不能修改房间类型
	//room->SetRoomType(roomType);

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(3203);
	if (name.compare(player->GetName() + nameSuffix) != 0)
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


UInt32 RoomMgr::DismissRoom(WSPlayer* player, UInt32 roomId)
{
	if (player != NULL)
	{
		if (player->GetRoomId() != roomId) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
	}
	
	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (player != NULL)
	{
		if (room->GetOwnerId() != player->GetID()) return ErrorCode::ROOM_SYSTEM_PERMISSION_UPDATE;
	}

	UInt32 nRes = room->DismissRoom();
	if (nRes == ErrorCode::SUCCESS)
	{
		RecycleRoom(room);

		//player->SendRoomDeleteUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
	}
	return nRes;
}


UInt32 RoomMgr::JoinRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, std::string password, UInt32 createTime)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetRoomId() != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	Room* room = GetRoom(roomId);
	if (room == NULL)
	{
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (createTime != 0 && room->GetCreateTime() != createTime)
	{
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (player->GetLevel() < room->GetLimitPlayerLevel())
	{
		return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_LEVEL;
	}

	if (player->GetSeasonLv() < room->GetLimitPlayerSeasonLevel())
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

	nRes = room->JoinRoom(player, group);
	if (nRes == ErrorCode::SUCCESS)
	{
		room->GetRoomInfo(roomInfo);

		player->SendRoomJoinUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
	}
	return nRes;
}

UInt32 RoomMgr::QuickJoinRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetRoomId() != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	Room* joinRoom = NULL;

	std::map<UInt32, Room*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		Room* room = iter->second;
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
		if (room->GetLimitPlayerLevel() > player->GetLevel())
		{
			continue;
		}
		if (room->GetLimitPlayerSeasonLevel() > player->GetSeasonLv())
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
				if (player->GetQuitRoomId() == joinRoom->GetID() && player->GetQuitRoomTime() + ROOM_QUIT_TIME_INTERVAL > ((UInt32)CURRENT_TIME.Sec()))
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
		UInt32 nRes = joinRoom->JoinRoom(player, RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
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

UInt32 RoomMgr::QuitRoom(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	UInt32 roomId = player->GetRoomId();
	if (roomId == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}
	player->SetRoomId(0);
	Room* room = GetRoom(roomId);
	if (room == NULL)
	{
		ErrorLogStream << PLAYERINFO(player) << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (room->GetRoomStatus() == RoomStatus::ROOM_STATUS_MATCH)
	{
		BattleMatchCencel(player);
		room->SendMatchCencelNotify(player, 3204);
	}

	room->QuitRoom(player);

	player->SendRoomQuitUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());

	//处理所有玩家退出的情况
	UInt32 playerMaxSize = 0;
	UInt32 playerSize = 0;
	room->GetPlayerSize(playerSize, playerMaxSize);
	if (playerSize == 0)
	{
		RecycleRoom(room);

		player->SendRoomDeleteUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
	}
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::KickOutRoom(WSPlayer* player, UInt64 kickPlayerId)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetID() == kickPlayerId) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT_SELF;
	UInt32 roomId = player->GetRoomId();

	WSPlayer* kickPlayer = WSPlayerMgr::Instance()->FindPlayer(kickPlayerId);
	if (kickPlayer == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	if (kickPlayer->GetRoomId() != room->GetID()) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;

	if (player->GetID() != room->GetOwnerId()) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT;

	QuitRoom(kickPlayer);
	room->SyncKickOutInfo(player, kickPlayer, RoomQuitReason::ROOM_QUIT_OWNER_KICK_OUT);

	return ErrorCode::SUCCESS;
}

//邀请玩家
UInt32 RoomMgr::InviteJoin(WSPlayer* player, UInt64 joinPlayerId)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	WSPlayer* joinPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(joinPlayerId);
	if (joinPlayer == NULL)
	{
		return ErrorCode::ROOM_SYSTEM_BE_INVITE_NOT_NORMAL;
	}

	if (joinPlayer->GetRoomId() != 0)
	{
		return ErrorCode::ROOM_SYSTEM_BE_INVITE_IN_ROOM;
	}

	if (joinPlayer->GetGameSetValue<UInt32>(GST_SECRET, SST_DUEL_INVATE))
	{
		return ErrorCode::SETTING_TARGET_SHILED_INVATE;
	}

	UInt32 nRes = room->InvitePlayer(player->GetID(), joinPlayerId);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	CLProtocol::WorldSyncRoomInviteInfo sync;
	sync.roomId = room->GetID();
	sync.roomName = room->GetName();
	sync.roomType = (UInt8)room->GetRoomType();
	sync.inviterId = player->GetID();
	sync.inviterLevel = player->GetLevel();
	sync.inviterName = player->GetName();
	sync.inviterOccu = player->GetOccu();
	sync.inviterAwaken = player->GetAwaken();

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;

	room->GetPlayerSize(playerSize, playerMaxSize);

	sync.playerSize = (UInt8)playerSize;
	sync.playerMaxSize = (UInt8)playerMaxSize;
	sync.group = room->GetRoomSlotGroup(sync.inviterId);
	joinPlayer->SendProtocol(sync);

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::BeInviteJoin(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, UInt64 invitePlayerId, bool isAccept)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetRoomId() != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;

	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	UInt32 nRes = room->CheckInviteTimeout(invitePlayerId, player->GetID());
	room->DeleteInviteInfo(invitePlayerId, player->GetID());
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	if (isAccept)
	{
		nRes = room->JoinRoom(player, group);
		if (nRes == ErrorCode::SUCCESS)
		{
			room->GetRoomInfo(roomInfo);
			player->SendRoomJoinUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
		}
	}
	else
	{
		WSPlayer* invitePlayer = WSPlayerMgr::Instance()->FindNormalPlayer(invitePlayerId);
		if (invitePlayer != NULL)
		{
			CLProtocol::WorldSyncRoomBeInviteInfo sync;
			sync.isAccept = isAccept;
			sync.playerId = player->GetID();
			invitePlayer->SendProtocol(sync);
		}
		nRes = ErrorCode::ROOM_SYSTEM_BE_INVITE_REFUSE;
	}

	return nRes;
}

//转让房主
UInt32 RoomMgr::ChangeOwner(WSPlayer* player, UInt64 newOwnerId)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	if (room->GetOwnerId() != player->GetID()) return ErrorCode::ROOM_SYSTEM_PERMISSION_CHANGE_OWNER;
	room->ChangeOwner(newOwnerId);
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CloseSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	if (room->GetOwnerId() != player->GetID()) return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT;

	return room->CloseSlot(player, slotGroup, index);
}

UInt32 RoomMgr::SwapSlot(WSPlayer* player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	return room->SwapSlot(player, playerId, slotGroup, index);
}

UInt32 RoomMgr::ResponseSwapSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	return room->ResponseSwapSlot(player, slotGroup, index, isAccept);
}

UInt32 RoomMgr::BattleReadyStart(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (room->GetOwnerId() != player->GetID()) return ErrorCode::ROOM_SYSTEM_PERMISSION_START_BATTLE;

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

	nRes = BattleReadyResult(player, RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_ACCEPT);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::BattleMatchCencel(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;

	CLProtocol::MatchServerMatchCancelReq msReq;
	msReq.id = player->GetID();
	msReq.needReturn = false;
	Router::SendToMatch(msReq);

	OnCancleRoomMatch(player);

	room->SendMatchCencelNotify(player, 3206);

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::BattleReadyResult(WSPlayer* player, RoomSlotReadyStatus slotStatus)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(player->GetRoomId());
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
			return nRes;
		}

		UInt32 playerSize = 0;
		UInt32 playerMaxSize = 0;
		room->GetPlayerSize(playerSize, playerMaxSize);
		player->SendRoomMatchStartUdpLog(room->GetID(), room->GetRoomType(), playerSize, room->GetCreateTime());
	}
	else if (nRes == ErrorCode::ROOM_SYSTEM_READY_SLOT_NOT_READY)
	{
		nRes = ErrorCode::SUCCESS;
	}

	return nRes;
}

UInt32 RoomMgr::SendInviteLink(WSPlayer* player, UInt32 roomId, UInt8 channel)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
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
	//std::string content = SysNotifyMgr::Instance()->MakeString(notifyId, room->GetName().c_str(), systemLinkStr);
	std::string content = SysNotifyMgr::Instance()->MakeString(notifyId, systemLinkStr);
	CLProtocol::SceneSyncChat protocol;
	protocol.channel = channel;
	protocol.objid = player->GetID();
	protocol.objname = player->GetName();
	protocol.occu = player->GetOccu();
	protocol.level = player->GetLevel();
	protocol.word = content;
	protocol.bLink = true;
	protocol.isGm = 0;
	protocol.voiceKey = "";
	protocol.voiceDuration = 0;
	protocol.headFrame = player->GetHeadFrame();
	Router::BroadcastToPlayer(protocol);
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossSetRoomInfo(RoomInfo& info, std::string password, UInt32 createTime)
{
	Room* room = GetRoom(info.roomSimpleInfo.id);
	if (room == NULL)
	{
		room = new Room(info.roomSimpleInfo.id, (RoomType)info.roomSimpleInfo.roomType, info.roomSimpleInfo.name, info.roomSimpleInfo.isLimitPlayerLevel,
			info.roomSimpleInfo.limitPlayerLevel, info.roomSimpleInfo.isLimitPlayerSeasonLevel, info.roomSimpleInfo.limitPlayerSeasonLevel);
		if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

		m_Rooms.insert(std::make_pair(room->GetID(), room));
		m_RoomByStatus[room->GetRoomStatus()].push_back(room);

	}
	else
	{
		room->SetRoomType((RoomType)info.roomSimpleInfo.roomType);
		room->SetName(info.roomSimpleInfo.name);
		room->SetIsLimitPlayerLevel(info.roomSimpleInfo.isLimitPlayerLevel);
		room->SetLimitPlayerLevel(info.roomSimpleInfo.limitPlayerLevel);
		room->SetIsLimitPlayerSeasonLevel(info.roomSimpleInfo.isLimitPlayerSeasonLevel);
		room->SetLimitPlayerSeasonLevel(info.roomSimpleInfo.limitPlayerSeasonLevel);
	}

	room->InitSlot();
	room->SetSlots(info.roomSlotInfos);
	room->SetOwnerId(info.roomSimpleInfo.ownerId);
	room->SetOwnerOccu(info.roomSimpleInfo.ownerOccu);
	room->SetOwnerSeasonLevel(info.roomSimpleInfo.ownerSeasonLevel);
	room->SetCreateTime(createTime);
	room->SetPassword(password);
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossSetRoomSimpleInfo(RoomSimpleInfo& info, std::string password)
{
	//if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	Room* room = GetRoom(info.id);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	//现在不能修改房间类型
	//room->SetRoomType(roomType);

	room->SetName(info.name);
	room->SetPassword(password);
	room->SetIsLimitPlayerLevel(info.isLimitPlayerLevel);
	room->SetLimitPlayerLevel(info.limitPlayerLevel);
	room->SetIsLimitPlayerSeasonLevel(info.isLimitPlayerSeasonLevel);
	room->SetLimitPlayerSeasonLevel(info.limitPlayerSeasonLevel);

	//同步给房间里的其他玩家
	room->SyncRoomSimpleInfo();

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossCreateRoom(WSPlayer* player, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (player->GetRoomId() != 0) return ErrorCode::ROOM_SYSTEM_PLAYER_IN_ROOM;
	if (roomType <= RoomType::ROOM_TYPE_INVALID || roomType >= RoomType::ROOM_TYPE_MAX) return ErrorCode::ROOM_SYSTEM_TYPE_ERROR;

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(3203);
	if (name.compare(player->GetName() + nameSuffix) != 0)
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

	CLProtocol::CrossUpdateRoomReq req;
	req.isLimitPlayerLevel = isPlayerLevel;
	req.isLimitPlayerSeasonLevel = isPlayerSeasonLevel;
	req.limitPlayerLevel = playerLevel;
	req.limitPlayerSeasonLevel = playerSeasonLevel;
	req.name = name;
	req.password = password;
	req.roomId = 0;
	req.roomType = roomType;
	GetRoomPlayerInfo(player, req.playerInfo);

	Router::SendToCross(req);
	return ErrorCode::SUCCESS;

}

UInt32 RoomMgr::CrossCreateRoomHandler(WSPlayer* player, UInt32 roomId)
{
	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	player->SendRoomCreateUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());
	player->SendRoomJoinUdpLog(room->GetID(), room->GetRoomType(), room->GetCreateTime());

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossUpdateRoomHandler(WSPlayer* player, UInt32 roomId)
{
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossUpdateRoom(WSPlayer* player, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	if (!IsRoomOpen(room->GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	if (room->GetOwnerId() != player->GetID())
	{
		return ErrorCode::ROOM_SYSTEM_PERMISSION_UPDATE;
	}

	//现在不能修改房间类型
	//room->SetRoomType(roomType);

	UInt32 nRes = ErrorCode::SUCCESS;
	std::string nameSuffix = SysNotifyMgr::Instance()->MakeString(3203);
	if (name.compare(player->GetName() + nameSuffix) != 0)
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

	CLProtocol::CrossUpdateRoomReq req;
	req.isLimitPlayerLevel = isPlayerLevel;
	req.isLimitPlayerSeasonLevel = isPlayerSeasonLevel;
	req.limitPlayerLevel = playerLevel;
	req.limitPlayerSeasonLevel = playerSeasonLevel;
	req.name = name;
	req.password = password;
	req.roomId = roomId;
	req.roomType = roomType;
	GetRoomPlayerInfo(player, req.playerInfo);
	Router::SendToCross(req);

	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::CrossJoinRoom(UInt32 roomId, RoomSlotInfo& info)
{
	Room* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	room->SetSlot(info);
	return ErrorCode::SUCCESS;
}

Room* RoomMgr::GetRoom(UInt32 roomId)
{
	if (roomId > m_MaxID || roomId < m_MinID) return NULL;
	std::map<UInt32, Room*>::iterator iter = m_Rooms.find(roomId);
	if (iter != m_Rooms.end())
	{
		return iter->second;
	}
	return NULL;
}

void RoomMgr::RecycleRoom(Room* room)
{
	if (room == NULL) return;
	room->SetRoomStatus(RoomStatus::ROOM_STATUS_INVALID);

	std::map<UInt32, Room*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		m_Rooms.erase(iter);
	}

	std::map<RoomStatus, std::vector<Room*>>::iterator iterStatus = m_RoomByStatus.begin();
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

void RoomMgr::ChangeRoomStatus(Room* room, RoomStatus roomStatus)
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

void RoomMgr::UpdateAvatar(WSPlayer* player)
{
	if (player == NULL || player->GetRoomId() == 0) return;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return;
	room->UpdateAvatar(player);
}

void RoomMgr::UpdateVipLevel(WSPlayer* player)
{
	if (player == NULL || player->GetRoomId() == 0) return;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return;
	room->UpdateVipLevel(player);
}

void RoomMgr::GetRoomPlayerInfo(WSPlayer* player, RoomPlayerInfo& info)
{
	if (player == NULL) return;
	info.avatar = player->GetAvatar();
	info.awaken = player->GetAwaken();
	info.accId = player->GetAccId();
	info.id = player->GetID();
	info.level = player->GetLevel();
	info.name = player->GetName();
	info.occu = player->GetOccu();
	info.seasonLevel = player->GetSeasonLv();
	info.seasonExp = 0;
	info.seasonStar = 0;
	info.quitRoomId = player->GetQuitRoomId();
	info.quitRoomTime = player->GetQuitRoomTime();
	info.roomId = player->GetRoomId();
	info.vipLevel = player->GetVipLvl();
	info.sceneId = player->GetSceneID();
	info.loseStreak = player->GetScoreWarLoseStreak();
	info.winStreak = player->GetScoreWarWinStreak();
	info.matchScore = player->GetMatchScore();
	info.battleCount = player->GetScoreWarBattleCount();
	info.headFrame = player->GetHeadFrame();
}

void RoomMgr::OnPlayerOnline(WSPlayer* player)
{
	if (player == NULL) return;
	InfoLogStream << "world server id:"<<NODE_ID<<" player(" << player->GetID() << ", " << player->GetAccId() << ") RoomMgr::OnPlayerOnline" << LogStream::eos;

	if (player->GetRoomId() == 0) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomPlayerOnline protocol;
		GetRoomPlayerInfo(player, protocol.player);
		Router::SendToCross(protocol);
		return;
	}

	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL)
	{
		player->SetRoomId(0);
		return;
	}

	//room->OnReconnectSuccess(player);
	QuitRoom(player);
}

void RoomMgr::OnPlayerOffline(WSPlayer* player)
{
	if (player == NULL) return;
	InfoLogStream << "world server id:" << NODE_ID << " player(" << player->GetID() << ", " << player->GetAccId() << ") RoomMgr::OnPlayerOffline" << LogStream::eos;

	if (player->GetRoomId() == 0) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomPlayerOffline protocol;
		GetRoomPlayerInfo(player, protocol.player);
		Router::SendToCross(protocol);
		return;
	}

	Room* room = GetRoom(player->GetRoomId());
	if (room != NULL)
	{
		QuitRoom(player);
	}
	else
	{
		player->SetRoomId(0);
	}
}

void RoomMgr::OnDisconnect(WSPlayer* player)
{
	if (player == NULL) return;
	InfoLogStream << "world server id:" << NODE_ID << " player(" << player->GetID() << ", " << player->GetAccId() << ") RoomMgr::OnDisconnect" << LogStream::eos;

	if (player->GetRoomId() == 0) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomPlayerDisconnect protocol;
		GetRoomPlayerInfo(player, protocol.player);
		Router::SendToCross(protocol);
		return;
	}

	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL)
	{
		player->SetRoomId(0);
		return;
	}

	//取消匹配
	if (room->GetRoomStatus() == RoomStatus::ROOM_STATUS_MATCH)
	{
		BattleMatchCencel(player);
		room->SendMatchCencelNotify(player, 3205);
	}

	room->OnDisconnect(player);
}

void RoomMgr::OnReconnectSuccess(WSPlayer* player)
{
	if (player == NULL) return;
	InfoLogStream << "world server id:" << NODE_ID << " player(" << player->GetID() << ", " << player->GetAccId() << ") RoomMgr::OnReconnectSuccess" << LogStream::eos;

	if (player->GetRoomId() == 0) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomPlayerReconnect protocol;
		GetRoomPlayerInfo(player, protocol.player);
		Router::SendToCross(protocol);
		return;
	}

	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL)
	{
		player->SetRoomId(0);
		return;
	}
	room->OnReconnectSuccess(player);
}

void RoomMgr::OnChangeScene(WSPlayer* player, UInt32 sceneId)
{
	if (player == NULL) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomPlayerChangeScene protocol;
		GetRoomPlayerInfo(player, protocol.player);
		protocol.sceneId = sceneId;
		Router::SendToCross(protocol);
		return;
	}

	bool isQuitRoom = false;
	WSScene* oldScene = WSSceneMgr::Instance()->FindScene(player->GetSceneID());
	if (oldScene != NULL && oldScene->GetDataEntry() != NULL)
	{
		if (oldScene->GetDataEntry()->subType == SceneSubType::SCENE_SUB_ROOM)
		{
			isQuitRoom = true;
		}
	}

	WSScene* newScene = WSSceneMgr::Instance()->FindScene(sceneId);
	if (newScene != NULL && newScene->GetDataEntry() != NULL && isQuitRoom)
	{
		if (newScene->GetType() != SceneType::SCENE_TYPE_PK)
		{
			QuitRoom(player);
		}
	}
}

void RoomMgr::OnTick(const Avalon::Time& now)
{
	std::map<UInt32, Room*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		Room* room = iter->second;
		++iter;
		if (room == NULL)
		{
			continue;
		}

		room->OnTick(now);
	}
}

void RoomMgr::OnStartRoomRace(WSPlayer* player)
{
	if (player == NULL) return;
	InfoLogStream << PLAYERINFO(player) << " OnStartRoomRace roomId = " << (UInt8)player->GetRoomId() << ")." << LogStream::eos;
	if (player->GetRoomId() == 0) return;

	if (IsCross())
	{
		CLProtocol::CrossSyncRoomStartRace protocol;
		GetRoomPlayerInfo(player, protocol.player);
		Router::SendToCross(protocol);
		return;
	}

	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return;

	room->OnStartRoomRace();

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	room->GetPlayerSize(playerSize, playerMaxSize);
	player->SendRoomBattleStartUdpLog(room->GetID(), room->GetRoomType(), playerSize, room->GetCreateTime());
}

void RoomMgr::OnCancleRoomMatch(WSPlayer* player)
{
	if (player == NULL || player->GetRoomId() == 0) return;
	InfoLogStream << PLAYERINFO(player) << " OnCancleRoomMatch roomId = " << (UInt8)player->GetRoomId() << ")." << LogStream::eos;
	Room* room = GetRoom(player->GetRoomId());
	if (room == NULL) return;
	room->OnCancleRoomMatch();
}

void RoomMgr::RemoveStatusRoom(RoomStatus status, Room* room)
{
	if (room == NULL) return;
	std::map<RoomStatus, std::vector<Room*>>::iterator iterStatus = m_RoomByStatus.find(status);
	if (iterStatus != m_RoomByStatus.end())
	{
		std::vector<Room*>::iterator iterRoom = iterStatus->second.begin();
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

void RoomMgr::InsertStatusRoom(Room* room)
{
	if (room == NULL) return;
	std::map<RoomStatus, std::vector<Room*>>::iterator iterStatus = m_RoomByStatus.find(room->GetRoomStatus());
	if (iterStatus != m_RoomByStatus.end())
	{
		iterStatus->second.push_back(room);
	}
	else
	{
		std::vector<Room*> rooms;
		rooms.push_back(room);
		m_RoomByStatus.insert(std::make_pair(room->GetRoomStatus(), rooms));
	}
}

bool RoomMgr::IsStrValid(const std::string& str, UInt32 maxWidth, bool isName)
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

UInt32 RoomMgr::IsNameValid(const std::string& name)
{
	//return ErrorCode::SUCCESS;

	if (!IsStrValid(name, ROOM_NAME_MAX_WIDTH, true))
	{
		return ErrorCode::ROOM_SYSTEM_NAME_INVALID;
	}
	return ErrorCode::SUCCESS;
}

UInt32 RoomMgr::IsPasswordValid(const std::string& password)
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
