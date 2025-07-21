#include "BSRoomMgr.h"
#include "CLErrorCode.h"
#include "CLSystemValueDataEntry.h"
#include "CLRoomProtocol.h"
#include "CLSysNotify.h"
#include "GameZoneMgr.h"


BSRoomMgr::BSRoomMgr()
{
	m_MinID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MIN);
	m_MaxID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_ID_MAX);
	//m_MinID = 1000;
	//m_MaxID = 9999;
	m_CurrentID = m_MinID;
}

BSRoomMgr::~BSRoomMgr()
{
	std::map<UInt32, BSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		BSRoom* room = iter->second;
		++iter;
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
	m_Rooms.clear();

	while (!m_NotUseRooms.empty())
	{
		BSRoom* room = m_NotUseRooms.front();
		m_NotUseRooms.pop_front();
		if (room == NULL) continue;
		CL_SAFE_DELETE(room);
	}
}

UInt32 BSRoomMgr::CreateRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType)
{
	if (roomType <= RoomType::ROOM_TYPE_INVALID || roomType >= RoomType::ROOM_TYPE_MAX) return ErrorCode::ROOM_SYSTEM_TYPE_ERROR;

	//UInt32 nRes = ErrorCode::SUCCESS;

	BSRoom* room = NULL;
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
		room = new BSRoom(m_CurrentID, roomType);
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
	}

	std::map<UInt32, BSRoom*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		return ErrorCode::ROOM_SYSTEM_ROOM_EXIST;
	}

	room->InitSlot();
	room->JoinRoom(zone, player);

	m_Rooms.insert(std::make_pair(room->GetID(), room));

	room->SetCreateTime((UInt32)CURRENT_TIME.Sec());

	room->GetRoomInfo(roomInfo);

	room->SendRoomCreateUdpLog(zone, player.id);
	room->SendRoomJoinUdpLog(zone, player.id);

	return ErrorCode::SUCCESS;
}

UInt32 BSRoomMgr::DismissRoom(GameZone* zone, ObjID_t playerId, UInt32 roomId)
{
	BSRoom* room = GetRoom(roomId);
	if (room == NULL) return ErrorCode::ROOM_SYSTEM_NOT_ROOM;

	UInt32 nRes = room->DismissRoom();
	if (nRes == ErrorCode::SUCCESS)
	{
		RecycleRoom(room);

		room->SendRoomDeleteUdpLog(zone, playerId);
	}
	return nRes;
}


UInt32 BSRoomMgr::JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group)
{
	BSRoom* room = GetRoom(roomId);
	if (room == NULL)
	{
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	if (!IsRoomOpen(room->GetRoomStatus()))
	{
		return ErrorCode::ROOM_SYSTEM_JOIN_BATTLE_START;
	}

	UInt32 nRes = room->JoinRoom(zone, player, group);
	if (nRes == ErrorCode::SUCCESS)
	{
		room->GetRoomInfo(roomInfo);
		room->SendRoomJoinUdpLog(zone, player.id);
	}
	return nRes;
}


UInt32 BSRoomMgr::QuitRoom(GameZone* zone, RoomPlayerInfo& player)
{
	UInt32 roomId = player.roomId;
	if (roomId == 0)
	{
		ErrorLogStream << "playerId=" << player.id << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
	}

	BSRoom* room = GetRoom(roomId);
	if (room == NULL)
	{
		ErrorLogStream << "playerId=" << player.id << " QuitRoom(" << roomId << ")." << LogStream::eos;
		return ErrorCode::ROOM_SYSTEM_NOT_ROOM;
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

BSRoom* BSRoomMgr::GetRoom(UInt32 roomId)
{
	if (roomId > m_MaxID || roomId < m_MinID) return NULL;
	std::map<UInt32, BSRoom*>::iterator iter = m_Rooms.find(roomId);
	if (iter != m_Rooms.end())
	{
		return iter->second;
	}
	return NULL;
}

void BSRoomMgr::RecycleRoom(BSRoom* room)
{
	if (room == NULL) return;
	room->SetRoomStatus(RoomStatus::ROOM_STATUS_INVALID);

	std::map<UInt32, BSRoom*>::iterator iter = m_Rooms.find(room->GetID());
	if (iter != m_Rooms.end())
	{
		m_Rooms.erase(iter);
	}

	m_NotUseRooms.push_back(room);

	InfoLogStream << "RecycleRoom " << " room(" << room->GetID() << "|" << room->GetRoomStatus() << ")." << LogStream::eos;
}

void BSRoomMgr::ChangeRoomStatus(BSRoom* room, RoomStatus roomStatus)
{
	if (room == NULL) return;
	room->SetRoomStatus(roomStatus);
	if (roomStatus == RoomStatus::ROOM_STATUS_OPEN)
	{
		room->SetOpenTime((UInt32)CURRENT_TIME.Sec());
	}
}

void BSRoomMgr::OnPlayerOnline(GameZone* zone, RoomPlayerInfo& player)
{
	BSRoom* room = GetRoom(player.roomId);
	if (room != NULL)
	{
		room->OnPlayerOnline(player);
	}
}

void BSRoomMgr::OnPlayerOffline(GameZone* zone, RoomPlayerInfo& player)
{
	BSRoom* room = GetRoom(player.roomId);
	if (room != NULL)
	{
		room->OnPlayerOffline(player);
		//QuitRoom(zone, player);
	}
}

void BSRoomMgr::OnDisconnect(UInt32 connId)
{
	// 某个服关闭，把来自该服务器所有玩家都踢掉
	std::set<UInt32> dissRoom;
	for (auto& iter : m_Rooms)
	{
		BSRoom* room = iter.second;
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

void BSRoomMgr::OnDisconnect(GameZone* zone, RoomPlayerInfo& player)
{
	if (player.roomId == 0) return;
	BSRoom* room = GetRoom(player.roomId);
	if (room == NULL)
	{
		return;
	}

	room->OnPlayerOffline(player);
}

void BSRoomMgr::OnReconnectSuccess(GameZone* zone, RoomPlayerInfo& player)
{
	if (player.roomId == 0) return;
	BSRoom* room = GetRoom(player.roomId);
	if (room == NULL)
	{
		return;
	}
	room->OnPlayerOnline(player);
}

void BSRoomMgr::OnTick(const Avalon::Time& now)
{
	std::map<UInt32, BSRoom*>::iterator iter = m_Rooms.begin();
	while (iter != m_Rooms.end())
	{
		BSRoom* room = iter->second;
		++iter;
		if (room == NULL)
		{
			continue;
		}

		room->OnTick(now);
	}
}

void BSRoomMgr::OnStartRoomRace(GameZone* zone, RoomPlayerInfo& player)
{
	InfoLogStream << "playerId = " << player.id << " OnStartRoomRace roomId = " << player.roomId << ")." << LogStream::eos;
	if (player.roomId == 0) return;
	BSRoom* room = GetRoom(player.roomId);
	if (room == NULL) return;

	room->OnStartRoomRace();

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	room->GetPlayerSize(playerSize, playerMaxSize);
	room->SendRoomBattleStartUdpLog(zone, player.id, playerSize);
}

