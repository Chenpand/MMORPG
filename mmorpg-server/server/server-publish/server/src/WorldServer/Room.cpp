#include "Room.h"
#include "CLSystemValueDataEntry.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "CLRoomProtocol.h"
#include "CLMatchProtocol.h"
#include "WSRouter.h"
#include "RoomMgr.h"
#include "WSSceneMgr.h"

RoomKickOutPlayerInfo::RoomKickOutPlayerInfo(UInt64 playerId, UInt32 kickoutTime)
{
	m_PlayerId = playerId;
	m_KickOutTime = kickoutTime;
}

RoomSlot::RoomSlot()
{
}

RoomSlot::~RoomSlot()
{
}

void RoomSlot::Init(WSPlayer* player, RoomSlotStatus status)
{
	if (player == NULL)
	{
		SetPlayerId(0);
		SetPlayerName("");
		SetPlayerLevel(0);
		SetPlayerSeasonLevel(0);
		SetPlayerVipLevel(0);
		SetPlayerOccu(0);
		SetPlayerAwaken(0);
		SetTargetSlotGroup(RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
		SetTargetSlotIndex(0);
		SetSwapStartTime(0);
	}
	else
	{
		SetPlayerId(player->GetID());
		SetPlayerName(player->GetName());
		SetPlayerLevel(player->GetLevel());
		SetPlayerSeasonLevel(player->GetSeasonLv());
		SetPlayerVipLevel(player->GetVipLvl());
		SetPlayerOccu(player->GetOccu());
		SetPlayerAwaken(player->GetAwaken());
		SetPlayerAvatar(player->GetAvatar());
		SetTargetSlotGroup(RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
		SetTargetSlotIndex(0);
		SetSwapStartTime(0);
	}
	SetReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
	m_Status = status;
}

void RoomSlot::Init(RoomSlotInfo& slotInfo)
{
	SetPlayerId(slotInfo.playerId);
	SetPlayerName(slotInfo.playerName);
	SetPlayerLevel(slotInfo.playerLevel);
	SetPlayerSeasonLevel(slotInfo.playerSeasonLevel);
	SetPlayerVipLevel(slotInfo.playerVipLevel);
	SetPlayerOccu(slotInfo.playerOccu);
	SetPlayerAwaken(slotInfo.playerAwake);
	SetPlayerAvatar(slotInfo.avatar);
	SetTargetSlotGroup(RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
	SetTargetSlotIndex(0);
	SetSwapStartTime(0);
	SetReadyStatus((RoomSlotReadyStatus)slotInfo.readyStatus);
	m_Status = (RoomSlotStatus)slotInfo.status;
}

void RoomSlot::GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup group)
{
	info.group = (UInt8)group;
	info.index = (UInt8)index;
	info.playerId = GetPlayerId();
	info.playerName = GetPlayerName();
	info.playerLevel = GetPlayerLevel();
	info.playerSeasonLevel = GetPlayerSeasonLevel();
	info.playerVipLevel = GetPlayerVipLevel();
	info.playerOccu = GetPlayerOccu();
	info.playerAwake = GetPlayerAwaken();
	info.avatar = GetPlayerAvatar();
	info.status = (UInt8)GetStatus();
	info.readyStatus = GetReadyStatus();
}

void RoomSlot::SetSwapSlotInfo(RoomSlotGroup slotGroup, UInt32 slotIndex, UInt32 startTime)
{
	m_TargetSlotGroup = slotGroup;
	m_TargetSlotIndex = slotIndex;
	m_SwapStartTime = startTime;
}

bool RoomSlot::IsSwapStatus()
{
	return m_TargetSlotGroup != RoomSlotGroup::ROOM_SLOT_GROUP_INVALID && m_SwapStartTime != 0;
}

bool RoomSlot::CheckSwapInfo(RoomSlotGroup targetSlotGroup, UInt32 targetSlotIndex, UInt32 timeInterval, UInt32 nowTime)
{
	if (m_TargetSlotGroup == targetSlotGroup && m_TargetSlotIndex == targetSlotIndex)
	{
		if (m_SwapStartTime + timeInterval >= nowTime)
		{
			return true;
		}
	}
	return false;
}

bool RoomSlot::CheckSwapInfoInvaild(UInt32 timeInterval, UInt32 nowTime)
{
	if (m_SwapStartTime + timeInterval >= nowTime)
	{
		return true;
	}
	return false;
}

void RoomSlot::ClearSwapInfo()
{
	m_TargetSlotGroup = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
	m_TargetSlotIndex = 0;
	m_SwapStartTime = 0;
}


Room::Room()
{
	m_ID = 0;
	m_OwnerId = 0;
	m_RoomType = RoomType::ROOM_TYPE_INVALID;
	m_RoomStatus = RoomStatus::ROOM_STATUS_INVALID;
	m_LimitPlayerLevel = 0;
	m_LimitPlayerSeasonLevel = 0;
	m_RoomSlotMaxSize = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_SLOT_MAX_SIZE);
	m_ReadyTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_BATTLE_READY_TIME);
	m_InviteTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_ENTER_TIME);
	m_SwapSlotTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_CHANGEPOS_TIME);
	m_KickOutTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_KICKOUT_JOIN_TIME);
}

Room::Room(UInt32 id, RoomType roomType, std::string name, UInt8 isLimitPlayerLevel, UInt16 limitPlayerLevel, UInt8 isLimitPlayerSeasonLevel, UInt32 limitPlayerSeasonLevel)
{
	m_ID = id;
	m_OwnerId = 0;
	m_Name = name;
	m_RoomType = roomType;
	m_RoomStatus = RoomStatus::ROOM_STATUS_OPEN;
	m_IsLimitPlayerLevel = isLimitPlayerLevel;
	m_LimitPlayerLevel = limitPlayerLevel;
	m_IsLimitPlayerLevel = isLimitPlayerSeasonLevel;
	m_LimitPlayerSeasonLevel = limitPlayerSeasonLevel;
	m_RoomSlotMaxSize = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_SLOT_MAX_SIZE);
	m_ReadyTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_BATTLE_READY_TIME);
	m_InviteTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_ENTER_TIME);
	m_SwapSlotTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_REJECT_CHANGEPOS_TIME);
	m_KickOutTimeInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_KICKOUT_JOIN_TIME);
}

Room::~Room()
{
}

void Room::InitSlot()
{
	m_RoomSlotGroup.clear();
	UInt32 start = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
	UInt32 end = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
	if (m_RoomType == RoomType::ROOM_TYPE_THREE_FREE || m_RoomType == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		start = RoomSlotGroup::ROOM_SLOT_GROUP_RED;
		end = RoomSlotGroup::ROOM_SLOT_GROUP_BLUE;
	}
	else if (m_RoomType == RoomType::ROOM_TYPE_THREE_MATCH || m_RoomType == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		start = RoomSlotGroup::ROOM_SLOT_GROUP_RED;
		end = RoomSlotGroup::ROOM_SLOT_GROUP_RED;
	}
	else
	{
		return;
	}

	for (UInt32 i = start; i <= end; ++i)
	{
		std::vector<RoomSlot> slots;
		for (UInt32 j = 0; j < m_RoomSlotMaxSize; ++j)
		{
			RoomSlot slot;
			slot.Init(NULL, RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
			slots.push_back(slot);
		}
		m_RoomSlotGroup.insert(std::make_pair((RoomSlotGroup)i, slots));
	}
}

void Room::SetSlots(std::vector<RoomSlotInfo> roomSlots, bool isSync)
{
	for (UInt32 i = 0; i < roomSlots.size(); ++i)
	{
		SetSlot(roomSlots[i]);
	}
}

void Room::SetSlot(RoomSlotInfo& slotInfo)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.find((RoomSlotGroup)slotInfo.group);
	if (iter != m_RoomSlotGroup.end())
	{
		UInt32 index = slotInfo.index;

		if (iter->second.size() > index)
		{
			iter->second[index].Init(slotInfo);
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(slotInfo.playerId);
			if (player != NULL)
			{
				player->SetRoomId(GetID());
				SenceSyncRoomInfo(player, RoomEvent::ROOM_EVENT_JOIN);
			}
		}

	}
}

UInt32 Room::CheckPassword(std::string& password)
{
	if (!GetPassword().empty())
	{
		if (password.empty()) return ErrorCode::ROOM_SYSTEM_PASSWORD_NOT_EMPTY;
		if (GetPassword().compare(password) != 0) return ErrorCode::ROOM_SYSTEM_PASSWORD_ERROR;
	}
	return ErrorCode::SUCCESS;
}

UInt32 Room::JoinRoom(WSPlayer* player, RoomSlotGroup group)
{
	if (!IsRoomOpen(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	//邀请进来的跟邀请者同一队
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
	if (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == 0 
				&& iterGroup->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
			{
				iterGroup->second[i].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				SyncRoomSlotInfo(iterGroup->first, i, false);
				player->SetRoomId(GetID());
				SenceSyncRoomInfo(player, RoomEvent::ROOM_EVENT_JOIN);
				return ErrorCode::SUCCESS;
			}
		}
	}
	
	//如果邀请者队伍没有空位.则进入第一个空位
	iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		if (iterGroup->first == group)
		{
			++iterGroup;
			continue;
		}

		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == 0 
				&& iterGroup->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
			{
				iterGroup->second[i].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				SyncRoomSlotInfo(iterGroup->first, i, false);
				player->SetRoomId(GetID());
				SenceSyncRoomInfo(player, RoomEvent::ROOM_EVENT_JOIN);
				return ErrorCode::SUCCESS;
			}
		}
		++iterGroup;
	}

	return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_MAX;
}

void Room::QuitRoom(WSPlayer* player, RoomSlotStatus status)
{
	if (player == NULL) return;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player->GetID())
			{
				ClearQuitSwapInfo(&(iterGroup->second[i]), iterGroup->first, i);

				iterGroup->second[i].Init(NULL, status);
				//房主退出
				if (player->GetID() == GetOwnerId())
				{
					ChangeOwner();
				}

				SyncRoomSlotInfo(iterGroup->first, i);
				player->SetRoomId(0);
				player->SetQuitRoomId(GetID());
				player->SetQuitRoomTime((UInt32)CURRENT_TIME.Sec());
				SenceSyncRoomInfo(player, RoomEvent::ROOM_EVENT_QUIT);
			}
		}
		++iterGroup;
	}
}

UInt32 Room::DismissRoom()
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (!IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(iterGroup->second[i].GetPlayerId());
				ClearQuitSwapInfo(&(iterGroup->second[i]), iterGroup->first, i);
				iterGroup->second[i].Init(NULL, RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
				if (player != NULL)
				{
					SyncRoomSlotInfo(iterGroup->first, i);
					player->SetRoomId(0);
					player->SetQuitRoomId(GetID());
					player->SetQuitRoomTime((UInt32)CURRENT_TIME.Sec());
					SyncKickOutInfo(player, player, RoomQuitReason::ROOM_QUIT_DISMISS);
					SenceSyncRoomInfo(player, RoomEvent::ROOM_EVENT_QUIT);
				}
			}
		}
		++iterGroup;
	}
	return ErrorCode::SUCCESS;
}

void Room::ChangeOwner(UInt64 newOwnerId)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (!IsSlotExistPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			if (newOwnerId != 0 && newOwnerId != iterGroup->second[i].GetPlayerId())
			{
				continue;
			}

			WSPlayer* newOwnerPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[i].GetPlayerId());
			if (newOwnerPlayer == NULL)
			{
				continue;
			}

			//newOwnerPlayer->SetRoomId(GetID());
			SetOwnerId(newOwnerPlayer->GetID());
			SetOwnerOccu(newOwnerPlayer->GetOccu());
			SetOwnerSeasonLevel(newOwnerPlayer->GetSeasonLv());
			SyncRoomSimpleInfo();

			CLProtocol::WorldSyncRoomPasswordInfo sync;
			sync.roomId = GetID();
			sync.password = GetPassword();
			newOwnerPlayer->SendProtocol(sync);

			return;
		}
		++iterGroup;
	}
}

UInt32 Room::CloseSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(slotGroup);
	if (iterGroup != m_RoomSlotGroup.end())
	{
		if (iterGroup->second.size() <= index)
		{
			return ErrorCode::ROOM_SYSTEM_SLOT_INDEX_ERROR;
		}

		if (GetOwnerId() == iterGroup->second[index].GetPlayerId())
		{
			return ErrorCode::ROOM_SYSTEM_PERMISSION_KICK_OUT_SELF;
		}

		if (iterGroup->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE)
		{
			iterGroup->second[index].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
			SyncRoomSlotInfo(iterGroup->first, index);
		}
		else if (iterGroup->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
		{
			iterGroup->second[index].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE);
			SyncRoomSlotInfo(iterGroup->first, index);
		}
		else if (iterGroup->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE
			|| iterGroup->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_WAIT)
		{
			WSPlayer* quitPlayer = WSPlayerMgr::Instance()->FindPlayer(iterGroup->second[index].GetPlayerId());
			if (player == NULL)
			{
				return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
			}
			
			QuitRoom(quitPlayer, RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE);
			SyncKickOutInfo(player, quitPlayer, RoomQuitReason::ROOM_QUIT_OWNER_KICK_OUT);
		}
	}
	else
	{
		return ErrorCode::ROOM_SYSTEM_SLOT_GROUP_ERROR;
	}
	return ErrorCode::SUCCESS;
}

UInt32 Room::SwapSlot(WSPlayer* player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index)
{
	//GetRoomSlotGroup(player->GetID())
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	//UInt32 requestTime = (UInt32)CURRENT_TIME.Sec();

	bool isFound = false;

	//位置索引
	UInt32 reqSlotIndex = 0;
	//位置迭代器
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (player->GetID() == iterGroup->second[i].GetPlayerId())
			{
				reqSlotIndex = i;
				isFound = true;
				break;
			}
		}

		if (isFound)
		{
			break;
		}
		++iterGroup;
	}

	if (!isFound)
	{
		return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
	}

	if (iterGroup->second[reqSlotIndex].IsSwapStatus())
	{
		return ErrorCode::ROOM_SYSTEM_SWAP_REQUEST_EXIST;
	}

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterFind = m_RoomSlotGroup.find(slotGroup);
	if (iterFind != m_RoomSlotGroup.end())
	{
		if (iterFind->second.size() <= index)
		{
			return ErrorCode::ROOM_SYSTEM_SLOT_INDEX_ERROR;
		}

		if (iterFind->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
		{
			//删除之前的位置信息
			iterGroup->second[reqSlotIndex].Init(NULL, ROOM_SLOT_STATUS_OPEN);
			//增加新的位置信息
			iterFind->second[index].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);

			SyncRoomSlotInfo(iterGroup->first, reqSlotIndex);
			SyncRoomSlotInfo(iterFind->first, index);

			playerId = 0;
		}
		else if (IsSlotExistPlayer(iterFind->second[index].GetStatus()))
		{
			if (player->GetID() == iterFind->second[index].GetPlayerId()) return ErrorCode::ROOM_SYSTEM_SWAP_NOT_SELF;
			WSPlayer* swapPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(iterFind->second[index].GetPlayerId());
			if (swapPlayer == NULL)
			{
				return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
			}

			if (iterFind->second[index].IsSwapStatus())
			{
				return ErrorCode::ROOM_SYSTEM_SWAP_RESPOND_EXIST;
			}

			UInt32 currentTime = (UInt32)CURRENT_TIME.Sec();
			iterGroup->second[reqSlotIndex].SetSwapSlotInfo(iterFind->first, index, currentTime);
			iterFind->second[index].SetSwapSlotInfo(iterGroup->first, reqSlotIndex, currentTime);

			CLProtocol::WorldSyncRoomSwapSlotInfo sync;
			sync.playerId = player->GetID();
			sync.playerLevel = player->GetLevel();
			sync.playerName = player->GetName();
			sync.playerOccu = player->GetOccu();
			sync.playerAwaken = player->GetAwaken();
			sync.slotGroup = (UInt8)iterGroup->first;
			sync.slotIndex = (UInt8)reqSlotIndex;

			swapPlayer->SendProtocol(sync);

			playerId = swapPlayer->GetID();

			//return ErrorCode::ROOM_SYSTEM_SLOT_HAVE_PLAYER;
		}
		else if (iterFind->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE)
		{
			return ErrorCode::ROOM_SYSTEM_PLAYER_OFFLINE;
		}
		else if (iterFind->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE)
		{
			return ErrorCode::ROOM_SYSTEM_SLOT_CLOSE;
		}
	}
	else
	{
		return ErrorCode::ROOM_SYSTEM_SLOT_GROUP_ERROR;
	}

	return ErrorCode::SUCCESS;
}

UInt32 Room::ResponseSwapSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterRes, iterReq;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	UInt32 resSlotIndex = 0;

	bool resSlotIsExist = false;
	bool reqSlotIsExist = false;

	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player->GetID())
			{
				iterRes = iter;
				resSlotIndex = i;
				resSlotIsExist = true;
			}
			if (iter->first == slotGroup && i == index)
			{
				iterReq = iter;
				reqSlotIsExist = true;
			}
		}
		++iter;
	}

	if (!resSlotIsExist || !reqSlotIsExist)
	{
		return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
	}

	UInt32 nRes = ErrorCode::SUCCESS;
	
	UInt32 nowTime = (UInt32)CURRENT_TIME.Sec();
	if (!iterReq->second[index].CheckSwapInfo(iterRes->first, resSlotIndex, m_SwapSlotTimeInterval, nowTime))
	{
		nRes = ErrorCode::ROOM_SYSTEM_SWAP_INFO_NOT_FOUND;
	}
	if (!iterRes->second[resSlotIndex].CheckSwapInfo(iterReq->first, index, m_SwapSlotTimeInterval, nowTime))
	{
		nRes = ErrorCode::ROOM_SYSTEM_SWAP_INFO_NOT_FOUND;
	}

	//清空交换信息
	iterRes->second[resSlotIndex].ClearSwapInfo();
	iterReq->second[index].ClearSwapInfo();

	if (isAccept)
	{
		if (nRes == ErrorCode::SUCCESS)
		{
			//交换位置
			RoomSlot tempRoomSlot = iterRes->second[resSlotIndex];
			iterRes->second[resSlotIndex] = iterReq->second[index];
			iterReq->second[index] = tempRoomSlot;

			SyncRoomSlotInfo(iterRes->first, resSlotIndex);
			SyncRoomSlotInfo(iterReq->first, index);
		}
		else
		{
			isAccept = false;
		}
	}

	WSPlayer* reqPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(iterReq->second[index].GetPlayerId());
	if (reqPlayer != NULL)
	{
		UInt8 swapResult = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_REFUSE;
		if (isAccept)
		{
			swapResult = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_ACCEPT;
		}
		CLProtocol::WorldSyncRoomSwapResultInfo sync;
		sync.result = swapResult;
		sync.playerId = player->GetID();
		sync.playerName = player->GetName();
		reqPlayer->SendProtocol(sync);
	}

	return nRes;
}

UInt32 Room::CheckStartReady()
{
	if (!IsRoomOpen(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_READY_STATUS_ERROR;

	//每只队伍必须有人
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		bool isError = false;
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (IsSlotExistPlayer(iter->second[i].GetStatus()))
			{
				isError = true;

				WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(iter->second[i].GetPlayerId());
				if (player != NULL)
				{
					WSScene* scene = WSSceneMgr::Instance()->FindScene(player->GetSceneID());
					if (scene != NULL && scene->GetDataEntry() != NULL)
					{
						if (GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
						{
							if (scene->GetDataEntry()->subType != SCENE_SUB_SCORE_WAR)
							{
								return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ROOM;
							}
						}
						else if (GetRoomType() == RoomType::ROOM_TYPE_THREE_FREE || 
							GetRoomType() == RoomType::ROOM_TYPE_THREE_MATCH ||
							GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
						{
							if (scene->GetDataEntry()->subType != SCENE_SUB_ROOM)
							{
								return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ROOM;
							}
						}
					}
				}
				else
				{
					return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ONLINE;
				}
			}
			else if (IsSlotOfflinePlayer(iter->second[i].GetStatus()))
			{
				return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ONLINE;
			}
		}

		if (!isError)
		{
			return ErrorCode::ROOM_SYSTEM_READY_PLAYER_SIZE;
		}
		++iter;
	}
	
	return ErrorCode::SUCCESS;
}

UInt32 Room::BattleReadyStart()
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (!IsSlotReadyInvalid(iter->second[i].GetReadyStatus()))
			{
				iter->second[i].SetReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}

	RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_READY);
	SyncRoomSimpleInfo();

	ResetReadyStartTime();

	return ErrorCode::SUCCESS;
}

UInt32 Room::BattleReadyResult(WSPlayer* player, RoomSlotReadyStatus slotStatus)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	if (!IsRoomReadyStatus(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_READY_SLOT_STATUS_ERROR;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();

	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player->GetID())
			{
				iter->second[i].SetReadyStatus(slotStatus);
				SyncRoomSlotInfo(iter->first, i);
				return ErrorCode::SUCCESS;
			}
		}
		++iter;
	}
	return ErrorCode::ROOM_SYSTEM_PLAYER_NOT_IN_ROOM;
}

UInt32 Room::CheckBattleReadySlotStatus()
{
	//bool isMatch = true;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iter->second[i].GetStatus()))
			{
				continue;
			}

			if (IsSlotReadyRefuse(iter->second[i].GetReadyStatus()))
			{
				return ErrorCode::ROOM_SYSTEM_READY_SLOT_REFUSE;
			}

			if (IsSlotReadyInvalid(iter->second[i].GetReadyStatus()))
			{
				return ErrorCode::ROOM_SYSTEM_READY_SLOT_NOT_READY;
			}
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 Room::RefuseBattle()
{
	return ErrorCode::SUCCESS;
}

UInt32 Room::BattleMatchStart()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_READY) return ErrorCode::ROOM_SYSTEM_MATCH_ROOM_STATUS_ERROR;

	CLProtocol::SceneRoomStartMatch pkReq;
	if (GetRoomType() == RoomType::ROOM_TYPE_THREE_FREE)
	{
		pkReq.pkType = (UInt8)PkType::PK_3V3_ROOM;
	}
	else if (GetRoomType() == RoomType::ROOM_TYPE_THREE_MATCH)
	{
		pkReq.pkType = (UInt8)PkType::PK_3V3_MATCH;
	}
	else if (GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
	{
		pkReq.pkType = (UInt8)PkType::PK_3V3_ACTIVITY;
	}
	else if (GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		pkReq.pkType = (UInt8)PkType::PK_3V3_TUMBLE;
	}
	else
	{
		return ErrorCode::ROOM_SYSTEM_MATCH_ROOM_TYPE_ERROR;
	}
	pkReq.roomId = GetID();
	GetMatchPlayerIds(pkReq.playerIds);

	//如果房间跨服...这里就有问题了 by huchenhui
	Router::BroadcastToScene(pkReq);
	//WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(GetOwnerId());
	//if (player == NULL) return ErrorCode::ROOM_SYSTEM_MATCH_ROOM_OWNER_QUIT;
	//player->SendToScene(pkReq);

	OnStartRoomMatch();

	return ErrorCode::SUCCESS;

	//Result  CLProtocol::MatchServerPkRoomRes
}

void Room::GetRoomSimpleInfo(RoomSimpleInfo& info)
{
	info.id = GetID();
	info.name = GetName();
	info.roomStatus = (UInt8)GetRoomStatus();
	info.roomType = (UInt8)GetRoomType();
	info.isLimitPlayerLevel = m_IsLimitPlayerLevel;
	info.limitPlayerLevel = m_LimitPlayerLevel;
	info.isLimitPlayerSeasonLevel = m_IsLimitPlayerSeasonLevel;
	info.limitPlayerSeasonLevel = m_LimitPlayerSeasonLevel;

	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	GetPlayerSize(playerSize, playerMaxSize);

	info.playerSize = (UInt8)playerSize;
	info.playerMaxSize = (UInt8)playerMaxSize;

	info.isPassword = false;
	if (!m_Password.empty())
	{
		info.isPassword = true;
	}

	info.ownerId = GetOwnerId();
	info.ownerOccu = GetOwnerOccu();
	info.ownerSeasonLevel = GetOwnerSeasonLevel();
}

void Room::GetRoomInfo(RoomInfo& info)
{
	GetRoomSimpleInfo(info.roomSimpleInfo);
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

void Room::SyncRoomSimpleInfo()
{
	CLProtocol::WorldSyncRoomSimpleInfo sync;
	GetRoomSimpleInfo(sync.info);

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[i].GetPlayerId());
			if (player != NULL)
			{
				player->SendProtocol(sync);
			}
		}

		++iterGroup;
	}
}

void Room::SendMatchCencelNotify(WSPlayer* player, UInt32 notifyId)
{
	if (player == NULL) return;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (player->GetID() == iterGroup->second[i].GetPlayerId() || IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			WSPlayer* targetPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[i].GetPlayerId());
			if (targetPlayer != NULL)
			{
				targetPlayer->SendNotify(notifyId, player->GetName());
			}
		}

		++iterGroup;
	}
}

void Room::Broadcast(Avalon::Protocol& protocol)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			WSPlayer* targetPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[i].GetPlayerId());
			if (targetPlayer != NULL)
			{
				targetPlayer->SendProtocol(protocol);
			}
		}
		++iterGroup;
	}
}

void Room::SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
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
				WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[i].GetPlayerId());
				if (player != NULL)
				{
					player->SendProtocol(sync);
				}
			}
		}
		++iterGroup;
	}
}

void Room::GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize)
{
	playerSize = 0;
	playerMaxSize = 0;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

bool Room::IsFull()
{
	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	GetPlayerSize(playerSize, playerMaxSize);
	if (playerSize >= playerMaxSize)
		return true;
	return false;
}

RoomSlotGroup Room::GetRoomSlotGroup(UInt64 playerId)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

UInt32 Room::GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

UInt32 Room::InvitePlayer(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	for (UInt32 i = 0; i < m_RoomInviteList.size(); ++i)
	{
		if (m_RoomInviteList[i].GetBeInvitePlayerId() == beInvitePlayerId && m_RoomInviteList[i].GetInvitePlayerId() == invitePlayerId)
		{
			return ErrorCode::ROOM_SYSTEM_INVITE_IN_INVITE_LIST;
		}
	}

	RoomInvite invite(invitePlayerId, beInvitePlayerId, (UInt32)CURRENT_TIME.Sec());
	m_RoomInviteList.push_back(invite);
	return ErrorCode::SUCCESS;
}

UInt64 Room::GetInvitePlayerId(UInt64 beInvitePlayerId)
{
	UInt64 invitePlayerId = 0;

	for (UInt32 i = 0; i < m_RoomInviteList.size(); ++i)
	{
		if (m_RoomInviteList[i].GetBeInvitePlayerId() == beInvitePlayerId)
		{
			invitePlayerId = m_RoomInviteList[i].GetInvitePlayerId();
			break;
		}
	}

	return invitePlayerId;
}

void Room::DeleteInviteInfo(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	std::vector<RoomInvite>::iterator iter = m_RoomInviteList.begin();

	while (iter != m_RoomInviteList.end())
	{
		if (iter->GetBeInvitePlayerId() == beInvitePlayerId && iter->GetInvitePlayerId() == invitePlayerId)
		{
			m_RoomInviteList.erase(iter);
			return;
		}
		++iter;
	}
}

UInt32 Room::CheckInviteTimeout(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	std::vector<RoomInvite>::iterator iter = m_RoomInviteList.begin();
	while (iter != m_RoomInviteList.end())
	{
		if (iter->GetBeInvitePlayerId() == beInvitePlayerId && iter->GetInvitePlayerId() == invitePlayerId)
		{
			if ((UInt32)CURRENT_TIME.Sec() > iter->GetInviteTime() + m_InviteTimeInterval)
			{
				return ErrorCode::ROOM_SYSTEM_INVITE_INFO_TIMEOUT;
			}
			else
			{
				return ErrorCode::SUCCESS;
			}
		}
		++iter;
	}
	return ErrorCode::ROOM_SYSTEM_INVITE_INFO_NOT_FOUND;
}

void Room::GetMatchPlayerIds(std::vector<UInt64>& matchPlayerIds)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			matchPlayerIds.push_back(iterGroup->second[i].GetPlayerId());
		}
		++iterGroup;
	}
}

void Room::ClearQuitSwapInfo(RoomSlot* roomSlot, RoomSlotGroup slotGroup, UInt32 slotIndex)
{
	if (roomSlot == NULL) return;
	if (!roomSlot->IsSwapStatus()) return;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(roomSlot->GetTargetSlotGroup());
	if (iterGroup != m_RoomSlotGroup.end() && iterGroup->second.size() > roomSlot->GetTargetSlotIndex())
	{
		if (iterGroup->second[roomSlot->GetTargetSlotIndex()].CheckSwapInfo(slotGroup, slotIndex, m_SwapSlotTimeInterval, (UInt32)CURRENT_TIME.Sec()))
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(iterGroup->second[roomSlot->GetTargetSlotIndex()].GetPlayerId());
			if (player != NULL)
			{
				CLProtocol::WorldSyncRoomSwapResultInfo sync;
				sync.result = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_CANCEL;
				sync.playerId = roomSlot->GetPlayerId();
				sync.playerName = roomSlot->GetPlayerName();
				player->SendProtocol(sync);
			}
			iterGroup->second[roomSlot->GetTargetSlotIndex()].ClearSwapInfo();
		}
	}

	roomSlot->ClearSwapInfo();
}

void Room::SyncKickOutInfo(WSPlayer* kickOutPlayer, WSPlayer* player, RoomQuitReason reason)
{
	if (player == NULL) return;

	CLProtocol::WorldSyncRoomKickOutInfo sync;
	sync.reason = reason;
	sync.kickPlayerId = 0;
	sync.kickPlayerName = "";

	if (kickOutPlayer != NULL)
	{
		sync.kickPlayerId = kickOutPlayer->GetID();
		sync.kickPlayerName = kickOutPlayer->GetName();
	}

	sync.roomId = GetID();
	player->SendProtocol(sync);

	m_RoomKickOutPlayers.push_back(RoomKickOutPlayerInfo(player->GetID(), (UInt32)CURRENT_TIME.Sec()));
}

UInt32 Room::CheckKickOutInfo(WSPlayer* player)
{
	if (player == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	std::vector<RoomKickOutPlayerInfo>::iterator iter = m_RoomKickOutPlayers.begin();
	while (iter != m_RoomKickOutPlayers.end())
	{
		if (iter->GetPlayerId() == player->GetID())
		{
			return ErrorCode::ROOM_SYSTEM_KICKOUT_JOIN_ERROR;
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

void Room::UpdateAvatar(WSPlayer* player)
{
	if (player == NULL) return;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player->GetID())
			{
				iter->second[i].SetPlayerAvatar(player->GetAvatar());
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}
}

void Room::UpdateVipLevel(WSPlayer* player)
{
	if (player == NULL) return;

	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player->GetID())
			{
				iter->second[i].SetPlayerVipLevel(player->GetVipLvl());
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}
}

void Room::OnPlayerOnline(WSPlayer* player)
{
	if (player == NULL) return;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player->GetID())
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				player->SendProtocol(sync);
				SyncRoomSlotInfo(iterGroup->first, i);
				return;
			}
		}
		++iterGroup;
	}
}

void Room::OnPlayerOffline(WSPlayer* player)
{
}

void Room::OnDisconnect(WSPlayer* player)
{
	if (player == NULL) return;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player->GetID())
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE);
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				player->SendProtocol(sync);
				SyncRoomSlotInfo(iterGroup->first, i);

				if (player->GetID() == GetOwnerId())
				{
					SetOwnerOfflineTime((UInt32)CURRENT_TIME.Sec());
				}

				return;
			}
		}
		++iterGroup;
	}
	player->SetRoomId(0);
}

void Room::OnReconnectSuccess(WSPlayer* player)
{
	if (player == NULL) return;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player->GetID())
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				player->SendProtocol(sync);
				SyncRoomSlotInfo(iterGroup->first, i);
				return;
			}
		}
		++iterGroup;
	}
	player->SetRoomId(0);
}

void Room::OnTick(const Avalon::Time& now)
{
	if (GetRoomStatus() == RoomStatus::ROOM_STATUS_READY)
	{
		//准备时间
		UInt32 nowSec = (UInt32)now.Sec();
		if (nowSec > m_ReadyStartTime + m_ReadyTimeInterval)
		{
			BattleReadySlotTimeOut();
		}
	}
	else if (IsRoomOpen(GetRoomStatus()))
	{
		//检查交换位置时间 , 检查是否需要变更房主
		CheckSwapSlotInfos();
	}

	UInt32 nowSec = (UInt32)now.Sec();
	std::vector<RoomKickOutPlayerInfo>::iterator iterKickout = m_RoomKickOutPlayers.begin();
	while (iterKickout != m_RoomKickOutPlayers.end())
	{
		if (iterKickout->GetKickOutTime() + m_KickOutTimeInterval < nowSec)
		{
			iterKickout = m_RoomKickOutPlayers.erase(iterKickout);
		}
		else
		{
			++iterKickout;
		}
	}

	std::vector<RoomInvite>::iterator inviteIter = m_RoomInviteList.begin();
	while (inviteIter != m_RoomInviteList.end())
	{
		if (inviteIter->GetInviteTime() + m_InviteTimeInterval < nowSec)
		{
			inviteIter = m_RoomInviteList.erase(inviteIter);
		}
		else
		{
			++inviteIter;
		}
	}

}

void Room::OnStartRoomMatch()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_MATCH)
	{
		RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_MATCH);
		SyncRoomSimpleInfo();
	}
}

void Room::OnCancleRoomMatch()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_OPEN)
	{
		RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
		SyncRoomSimpleInfo();
	}
}

void Room::OnStartRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_BATTLE)
	{
		RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_BATTLE);
		SyncRoomSimpleInfo();
	}
}

void Room::OnEndRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_OPEN)
	{
		RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
		SyncRoomSimpleInfo();
	}
}

UInt32 Room::GetReadyStartTime()
{
	return m_ReadyStartTime;
}

void Room::ResetReadyStartTime()
{
	m_ReadyStartTime = (UInt32)CURRENT_TIME.Sec();
}

void Room::BattleReadySlotTimeOut()
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iter->second[i].GetStatus()))
			{
				continue;
			}

			if (IsSlotReadyInvalid(iter->second[i].GetReadyStatus()))
			{
				iter->second[i].SetReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_TIMEOUT);
				SyncRoomSlotInfo(iter->first, i);
			}
		}
		++iter;
	}

	RoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
	SyncRoomSimpleInfo();

}

void Room::SenceSyncRoomInfo(WSPlayer* player, RoomEvent roomEvent)
{
	if (player == NULL) return;
	CLProtocol::SceneRoomSyncInfo sync;
	sync.playerId = player->GetID();
	sync.roomEvent = (UInt8)roomEvent;
	sync.roomId = GetID();
	player->SendToScene(sync);
}

void Room::CheckSwapSlotInfos()
{
	UInt32 nowTime = (UInt32)CURRENT_TIME.Sec();
	bool isDismiss = true;
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (!iter->second[i].CheckSwapInfoInvaild(m_SwapSlotTimeInterval, nowTime))
			{
				iter->second[i].ClearSwapInfo();
			}

			if (iter->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE 
				&& iter->second[i].GetPlayerId() == GetOwnerId())
			{
				if (GetOwnerOfflineTime() + ROOM_OWNER_OFFLINE_INTERVAL < nowTime)
				{
					ChangeOwner(0);
				}
			}

			if (IsSlotExistPlayer(iter->second[i].GetStatus()))
			{
				isDismiss = false;
			}
		}
		++iter;
	}

	if (isDismiss && (GetOwnerOfflineTime() + ROOM_OWNER_OFFLINE_INTERVAL < nowTime))
	{
		RoomMgr::Instance()->DismissRoom(NULL, GetID());
	}
}

WSPlayer* Room::GetPlayerBySlot(RoomSlotGroup slotGroup, UInt32 slotIndex, bool isNormal)
{
	std::map<RoomSlotGroup, std::vector<RoomSlot>>::iterator iter = m_RoomSlotGroup.find(slotGroup);
	if (iter != m_RoomSlotGroup.end())
	{
		if (iter->second.size() > slotIndex)
		{
			if (iter->second[slotIndex].GetPlayerId() == 0) return NULL;
			
			WSPlayer* player = NULL;
			if (isNormal)
			{
				player = WSPlayerMgr::Instance()->FindNormalPlayer(iter->second[slotIndex].GetPlayerId());
			}
			else
			{
				player = WSPlayerMgr::Instance()->FindPlayer(iter->second[slotIndex].GetPlayerId());
			}
			return player;
		}
	}
	return NULL;
}

