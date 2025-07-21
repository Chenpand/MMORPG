#include "BSRoom.h"
#include "CLSystemValueDataEntry.h"
#include "CLRoomProtocol.h"
#include "CLMatchProtocol.h"
#include "BSRoomMgr.h"
#include "CLErrorCode.h"
#include "CLSceneDataEntry.h"
#include "CLSysNotify.h"


BSRoomSlot::BSRoomSlot()
{
}

BSRoomSlot::~BSRoomSlot()
{
}

void BSRoomSlot::Init(RoomPlayerInfo player, RoomSlotStatus status)
{
	SetConnId(player.connId);
	SetPlayerId(player.id);
	SetPlayerName(player.name);
	SetPlayerLevel(player.level);
	SetPlayerOccu(player.occu);
	SetPlayerAvatar(player.avatar);
	SetPlayerVipLevel(player.vipLevel);
	m_Status = status;

	SetAccId(player.accId);
	SetSceneId(player.sceneId);
}

void BSRoomSlot::GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup group)
{
	info.group = (UInt8)group;
	info.index = (UInt8)index;
	info.playerId = GetPlayerId();
	info.playerName = GetPlayerName();
	info.playerLevel = GetPlayerLevel();
	info.playerOccu = GetPlayerOccu();
	info.status = (UInt8)GetStatus();
	info.avatar = GetPlayerAvatar();
	info.playerVipLevel = GetPlayerVipLevel();
	info.accId = GetAccId();
}

void BSRoomSlot::GetRoomPlayerInfo(RoomPlayerInfo& info)
{
	info.avatar = GetPlayerAvatar();
	info.connId = GetConnId();
	info.id = GetPlayerId();
	info.level = GetPlayerLevel();
	info.name = GetPlayerName();
	info.occu = GetPlayerOccu();
	info.vipLevel = GetPlayerVipLevel();
	info.sceneId = GetSceneId();
}

BSRoom::BSRoom()
{
	m_ID = 0;
	m_RoomType = RoomType::ROOM_TYPE_INVALID;
	m_RoomStatus = RoomStatus::ROOM_STATUS_INVALID;
	m_RoomSlotMaxSize = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_SLOT_MAX_SIZE);
}

BSRoom::BSRoom(UInt32 id, RoomType roomType)
{
	m_ID = id;
	m_RoomType = roomType;
	m_RoomStatus = RoomStatus::ROOM_STATUS_OPEN;
	m_RoomSlotMaxSize = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_SLOT_MAX_SIZE);
}

BSRoom::~BSRoom()
{
}

void BSRoom::InitSlot()
{
	m_RoomSlotGroup.clear();
	UInt32 start = RoomSlotGroup::ROOM_SLOT_GROUP_RED;
	UInt32 end = RoomSlotGroup::ROOM_SLOT_GROUP_BLUE;

	for (UInt32 i = start; i <= end; ++i)
	{
		std::vector<BSRoomSlot> slots;
		for (UInt32 j = 0; j < m_RoomSlotMaxSize; ++j)
		{
			BSRoomSlot slot;
			slot.Init(RoomPlayerInfo(), RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
			slots.push_back(slot);
		}
		m_RoomSlotGroup.insert(std::make_pair((RoomSlotGroup)i, slots));
	}
}

UInt32 BSRoom::JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group)
{
	if (!IsRoomOpen(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;

	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
	if (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == 0 
				&& iterGroup->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
			{
				player.roomId = GetID();
				iterGroup->second[i].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				SyncRoomSlotInfo(iterGroup->first, i, false);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_JOIN);

				return ErrorCode::SUCCESS;
			}
		}
	}
	
	return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_MAX;
}

void BSRoom::QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				iterGroup->second[i].Init(RoomPlayerInfo(), status);
				SyncRoomSlotInfo(iterGroup->first, i);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_QUIT);
			}
		}
		++iterGroup;
	}
}

UInt32 BSRoom::DismissRoom()
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (!IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iterGroup->second[i].GetConnId());
				if (zone == NULL)
				{
					continue;
				}

				RoomPlayerInfo player;
				GetPlayerById(player, iterGroup->second[i].GetPlayerId());

				iterGroup->second[i].Init(RoomPlayerInfo(), RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
				SyncRoomSlotInfo(iterGroup->first, i);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_QUIT);
			}
		}
		++iterGroup;
	}
	return ErrorCode::SUCCESS;
}

UInt32 BSRoom::BattleReadyStart()
{
	BSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_READY);
	SyncRoomSimpleInfo();

	return ErrorCode::SUCCESS;
}

void BSRoom::GetRoomSimpleInfo(RoomSimpleInfo& info)
{
	info.id = GetID();
	info.roomStatus = (UInt8)GetRoomStatus();
	info.roomType = (UInt8)GetRoomType();

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	GetPlayerSize(playerSize, playerMaxSize);

	info.playerSize = (UInt8)playerSize;
	info.playerMaxSize = (UInt8)playerMaxSize;
}

void BSRoom::GetRoomInfo(RoomInfo& info)
{
	GetRoomSimpleInfo(info.roomSimpleInfo);
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			RoomSlotInfo slotInfo;
			iterGroup->second[i].GetRoomSlotInfo(slotInfo, i, iterGroup->first);
			info.roomSlotInfos.push_back(slotInfo);
		}
		++iterGroup;
	}
}

void BSRoom::SyncRoomSimpleInfo()
{
	CLProtocol::WorldSyncRoomSimpleInfo sync;
	GetRoomSimpleInfo(sync.info);

	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iterGroup->second[i].GetConnId());
			if (zone != NULL)
			{
				zone->SendToPlayer(iterGroup->second[i].GetPlayerId(), sync);
			}
		}

		++iterGroup;
	}
}

void BSRoom::Broadcast(Avalon::Protocol& protocol)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			RoomPlayerInfo targetPlayer;

			if (GetPlayerById(targetPlayer, iterGroup->second[i].GetPlayerId()))
			{
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(targetPlayer.connId);
				if (zone != NULL)
				{
					zone->SendToPlayer(targetPlayer.id, protocol);
				}
			}
		}
		++iterGroup;
	}
}

void BSRoom::SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
	// 无效的队伍 无效的索引
	if (iterGroup == m_RoomSlotGroup.end() || index >= iterGroup->second.size()) return;

	CLProtocol::WorldSyncRoomSlotInfo sync;
	iterGroup->second[index].GetRoomSlotInfo(sync.slotInfo, index, group);

	iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			//不给自己同步
			if (!isSyncSelf && iterGroup->first == group && index == i) continue;
			if (!IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iterGroup->second[i].GetConnId());
				if (zone == NULL)
				{
					continue;
				}

				zone->SendToPlayer(iterGroup->second[i].GetPlayerId(), sync);
			}
		}
		++iterGroup;
	}
}

void BSRoom::GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize)
{
	playerSize = 0;
	playerMaxSize = 0;

	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		playerMaxSize += m_RoomSlotMaxSize;
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (!IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				playerSize += 1;
			}
			if (iterGroup->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE)
			{
				playerMaxSize -= 1;
			}
		}
		++iterGroup;
	}
}

bool BSRoom::IsFull()
{
	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	GetPlayerSize(playerSize, playerMaxSize);
	if (playerSize >= playerMaxSize)
		return true;
	return false;
}

bool BSRoom::IsInRoom(UInt64 playerId)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (playerId == iterGroup->second[i].GetPlayerId())
			{
				return true;
			}
		}
		iterGroup++;
	}

	return false;
}

RoomSlotGroup BSRoom::GetRoomSlotGroup(UInt64 playerId)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (playerId == iterGroup->second[i].GetPlayerId())
			{
				return iterGroup->first;
			}
		}
		++iterGroup;
	}
	return RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
}

UInt32 BSRoom::GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (playerId == iterGroup->second[i].GetPlayerId())
			{
				slotGroup = iterGroup->first;
				slotIndex = i;
				return ErrorCode::SUCCESS;
			}
		}
		++iterGroup;
	}
	return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
}

void BSRoom::UpdateAvatar(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player.id)
			{
				iter->second[i].SetPlayerAvatar(player.avatar);
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}
}

void BSRoom::UpdateVipLevel(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player.id)
			{
				iter->second[i].SetPlayerVipLevel(player.vipLevel);
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}
}

void BSRoom::UpdateSceneId(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player.id)
			{
				iter->second[i].SetSceneId(player.sceneId);
			}
		}
		++iter;
	}
}

bool BSRoom::GetPlayerById(RoomPlayerInfo& info, ObjID_t playerId)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == playerId)
			{
				iter->second[i].GetRoomPlayerInfo(info);
				info.roomId = GetID();
				return true;
			}
		}
		++iter;
	}
	return false;
}

void BSRoom::QuitPlayerByConnId(UInt32 connId, std::set<UInt32>& dissRoomSet)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetConnId() != connId)
				continue;

			RoomPlayerInfo info;
			GetPlayerById(info, iter->second[i].GetPlayerId());
			QuitRoom(NULL, info);

			UInt32 playerMaxSize = 0;
			UInt32 playerSize = 0;
			GetPlayerSize(playerSize, playerMaxSize);
			if (playerSize == 0)
			{
				dissRoomSet.insert(GetID());
			}
		}
		++iter;
	}
}

void BSRoom::SendRoomCreateUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_CREATE, 0);
}

void BSRoom::SendRoomDeleteUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_DELETE, 0);
}

void BSRoom::SendRoomJoinUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_JOIN, 0);
}

void BSRoom::SendRoomQuitUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_QUIT, 0);
}

void BSRoom::SendRoomBattleStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_BATTLE_START, roomPlayerSize);
}

void BSRoom::SendUdpLog(GameZone* zone, ObjID_t playerId, LogRoomOperationType opType, UInt32 roomPlayerSize)
{
	if (zone == NULL) return;
	CLProtocol::CrossSyncRoomLogInfo sync;
	sync.createTime = GetCreateTime();
	sync.opType = (UInt32)opType;
	sync.playerSize = roomPlayerSize;
	sync.reason = "";
	sync.roomId = GetID();
	sync.roomType = (UInt32)GetRoomType();
	zone->TransmitToWorld(sync);
}

void BSRoom::OnPlayerOnline(RoomPlayerInfo& player)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);

	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				if (zone != NULL)
				{
					zone->SendToPlayer(player.id, sync);
				}

				SyncRoomSlotInfo(iterGroup->first, i);
				return;
			}
		}
		++iterGroup;
	}
}

void BSRoom::OnPlayerOffline(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE);
				SyncRoomSlotInfo(iterGroup->first, i);
				return;
			}
		}
		++iterGroup;
	}
}

void BSRoom::OnTick(const Avalon::Time& now)
{

}

void BSRoom::OnStartRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_BATTLE)
	{
		BSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_BATTLE);
		SyncRoomSimpleInfo();
	}
}

void BSRoom::OnEndRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_OPEN)
	{
		BSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
		SyncRoomSimpleInfo();
	}
}

void BSRoom::SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent)
{
	if (zone == NULL) return;

	CLProtocol::SceneRoomSyncInfo sync;
	sync.playerId = player.id;
	sync.roomEvent = (UInt8)roomEvent;
	sync.roomId = GetID();
	zone->TransmitToScene(sync);
}


