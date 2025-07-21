#include "CSRoom.h"
#include "CLSystemValueDataEntry.h"
#include "CLRoomProtocol.h"
#include "CLMatchProtocol.h"
#include "CSRoomMgr.h"
#include "CLErrorCode.h"
#include "CLSceneDataEntry.h"
#include "CLSysNotify.h"
#include "CLChatProtocol.h"

CSRoomKickOutPlayerInfo::CSRoomKickOutPlayerInfo(UInt64 playerId, UInt32 kickoutTime)
{
	m_PlayerId = playerId;
	m_KickOutTime = kickoutTime;
}

CSRoomSlot::CSRoomSlot()
{
}

CSRoomSlot::~CSRoomSlot()
{
}

void CSRoomSlot::Init(RoomPlayerInfo player, RoomSlotStatus status)
{
	SetConnId(player.connId);
	SetPlayerId(player.id);
	SetPlayerName(player.name);
	SetPlayerLevel(player.level);
	SetPlayerSeasonLevel(player.seasonLevel);
	SetPlayerOccu(player.occu);
	SetPlayerAwaken(player.awaken);
	SetPlayerAvatar(player.avatar);
	SetPlayerVipLevel(player.vipLevel);
	SetTargetSlotGroup(RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);
	SetTargetSlotIndex(0);
	SetSwapStartTime(0);
	SetReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
	m_Status = status;

	SetAccId(player.accId);
	SetLoseStreak(player.loseStreak);
	SetWinStreak(player.winStreak);
	SetMatchScore(player.matchScore);
	SetSeasonExp(player.seasonExp);
	SetSeasonStar(player.seasonStar);
	SetBattleCount(player.battleCount);
	SetHeadFrame(player.headFrame);
	//SetSceneId(player.sceneId);
}

void CSRoomSlot::GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup group)
{
	info.group = (UInt8)group;
	info.index = (UInt8)index;
	info.playerId = GetPlayerId();
	info.playerName = GetPlayerName();
	info.playerLevel = GetPlayerLevel();
	info.playerSeasonLevel = GetPlayerSeasonLevel();
	info.playerOccu = GetPlayerOccu();
	info.playerAwake = GetPlayerAwaken();
	info.status = (UInt8)GetStatus();
	info.readyStatus = GetReadyStatus();
	info.avatar = GetPlayerAvatar();
	info.playerVipLevel = GetPlayerVipLevel();
	info.accId = GetAccId();
	info.winStreak = GetWinStreak();
	info.loseStreak = GetLoseStreak();
	info.matchScore = GetMatchScore();
	info.battleCount = GetBattleCount();
	info.playerLabelInfo.headFrame = GetHeadFrame();
}

void CSRoomSlot::GetRoomPlayerInfo(RoomPlayerInfo& info)
{
	info.avatar = GetPlayerAvatar();
	info.awaken = GetPlayerAwaken();
	info.connId = GetConnId();
	info.id = GetPlayerId();
	info.level = GetPlayerLevel();
	info.name = GetPlayerName();
	info.occu = GetPlayerOccu();
	info.vipLevel = GetPlayerVipLevel();
	//info.quitRoomId = 
	//info.quitRoomTime
	//info.roomId = 
	info.seasonLevel = GetPlayerSeasonLevel();
	info.seasonExp = GetSeasonExp();
	info.seasonStar = GetSeasonStar();
	info.matchScore = GetMatchScore();
	info.battleCount = GetBattleCount();
	info.sceneId = GetSceneId();
}

void CSRoomSlot::SetSwapSlotInfo(RoomSlotGroup slotGroup, UInt32 slotIndex, UInt32 startTime)
{
	m_TargetSlotGroup = slotGroup;
	m_TargetSlotIndex = slotIndex;
	m_SwapStartTime = startTime;
}

bool CSRoomSlot::IsSwapStatus()
{
	return m_TargetSlotGroup != RoomSlotGroup::ROOM_SLOT_GROUP_INVALID && m_SwapStartTime != 0;
}

bool CSRoomSlot::CheckSwapInfo(RoomSlotGroup targetSlotGroup, UInt32 targetSlotIndex, UInt32 timeInterval, UInt32 nowTime)
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

bool CSRoomSlot::CheckSwapInfoInvaild(UInt32 timeInterval, UInt32 nowTime)
{
	if (m_SwapStartTime + timeInterval >= nowTime)
	{
		return true;
	}
	return false;
}

void CSRoomSlot::ClearSwapInfo()
{
	m_TargetSlotGroup = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID;
	m_TargetSlotIndex = 0;
	m_SwapStartTime = 0;
}

void CSRoomSlot::UpdateMatchInfo(UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount)
{
	SetLoseStreak(loseStreak);
	SetWinStreak(winStreak);
	SetMatchScore(matchScore);
	SetBattleCount(battleCount);
}


CSRoom::CSRoom()
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

CSRoom::CSRoom(UInt32 id, RoomType roomType, std::string name, UInt8 isLimitPlayerLevel, UInt16 limitPlayerLevel, UInt8 isLimitPlayerSeasonLevel, UInt32 limitPlayerSeasonLevel)
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

CSRoom::~CSRoom()
{
}

void CSRoom::InitSlot()
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

	// 3v3乱斗模式改成2v2
	if (m_RoomType == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		m_RoomSlotMaxSize = 2;
	}
	else
	{
		m_RoomSlotMaxSize = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_ROOM_SLOT_MAX_SIZE);
	}

	for (UInt32 i = start; i <= end; ++i)
	{
		std::vector<CSRoomSlot> slots;
		for (UInt32 j = 0; j < m_RoomSlotMaxSize; ++j)
		{
			CSRoomSlot slot;
			slot.Init(RoomPlayerInfo(), RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);
			slots.push_back(slot);
		}
		m_RoomSlotGroup.insert(std::make_pair((RoomSlotGroup)i, slots));
	}
}

UInt32 CSRoom::CheckPassword(std::string& password)
{
	if (!GetPassword().empty())
	{
		if (password.empty()) return ErrorCode::ROOM_SYSTEM_PASSWORD_NOT_EMPTY;
		if (GetPassword().compare(password) != 0) return ErrorCode::ROOM_SYSTEM_PASSWORD_ERROR;
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSRoom::JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group)
{
	if (!IsRoomOpen(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_ROOM_STATUS_ERROR;
	if (GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR && player.level < SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SCORE_WAR_LEVEL)) return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_LEVEL;
	//邀请进来的跟邀请者同一队
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
	if (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == 0 
				&& iterGroup->second[i].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
			{
				player.roomId = GetID();
				CSRoomMgr::Instance()->AddPlayerMap(player.id, GetID());
				iterGroup->second[i].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				SyncRoomSlotInfo(iterGroup->first, i, false);
				SyncRoomPlayerInfoToWorld(zone, player.id, player.roomId, player.quitRoomId, player.quitRoomTime);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_JOIN);

				CLProtocol::MatchServerMatchCancelReq msReq;
				msReq.id = player.id;
				msReq.needReturn = true;
				zone->TransmitToWorld(msReq);

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
				player.roomId = GetID();
				CSRoomMgr::Instance()->AddPlayerMap(player.id, GetID());
				iterGroup->second[i].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				SyncRoomSlotInfo(iterGroup->first, i, false);
				SyncRoomPlayerInfoToWorld(zone, player.id, player.roomId, player.quitRoomId, player.quitRoomTime);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_JOIN);

				CLProtocol::MatchServerMatchCancelReq msReq;
				msReq.id = player.id;
				msReq.needReturn = true;
				zone->TransmitToWorld(msReq);

				return ErrorCode::SUCCESS;
			}
		}
		++iterGroup;
	}

	return ErrorCode::ROOM_SYSTEM_JOIN_PLAYER_MAX;
}

void CSRoom::QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				ClearQuitSwapInfo(&(iterGroup->second[i]), iterGroup->first, i);
				CSRoomMgr::Instance()->DelPlayerMap(iterGroup->second[i].GetPlayerId());
				iterGroup->second[i].Init(RoomPlayerInfo(), status);
				//房主退出
				if (player.id == GetOwnerId())
				{
					ChangeOwner();
				}

				SyncRoomSlotInfo(iterGroup->first, i);

				SyncRoomPlayerInfoToWorld(zone, player.id, 0, GetID(), (UInt32)CURRENT_TIME.Sec());
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_QUIT);
			}
		}
		++iterGroup;
	}
}

UInt32 CSRoom::DismissRoom()
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

				ClearQuitSwapInfo(&(iterGroup->second[i]), iterGroup->first, i);
				CSRoomMgr::Instance()->DelPlayerMap(iterGroup->second[i].GetPlayerId());
				iterGroup->second[i].Init(RoomPlayerInfo(), RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

				SyncRoomSlotInfo(iterGroup->first, i);
				SyncRoomPlayerInfoToWorld(zone, iterGroup->second[i].GetPlayerId(), 0, GetID(), (UInt32)CURRENT_TIME.Sec());
				SyncKickOutInfo(player, player, RoomQuitReason::ROOM_QUIT_DISMISS);
				SenceSyncRoomInfo(zone, player, RoomEvent::ROOM_EVENT_QUIT);
				
			}
		}
		++iterGroup;
	}
	return ErrorCode::SUCCESS;
}

void CSRoom::ChangeOwner(UInt64 newOwnerId)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

			RoomPlayerInfo newOwnerPlayer;

			if (!GetPlayerById(newOwnerPlayer, iterGroup->second[i].GetPlayerId()))
			{
				continue;
			}

			//newOwnerPlayer->SetRoomId(GetID());
			SetOwnerId(newOwnerPlayer.id);
			SetOwnerOccu(newOwnerPlayer.occu);
			if (GetRoomType() == ROOM_TYPE_THREE_FREE || GetRoomType() == ROOM_TYPE_THREE_SCORE_WAR || GetRoomType() == ROOM_TYPE_THREE_TUMBLE)
			{
				std::string room_suffix = SysNotifyMgr::Instance()->MakeString(9933);
				SetName(newOwnerPlayer.name.append(room_suffix));
			}
			
			SetOwnerSeasonLevel(newOwnerPlayer.seasonLevel);
			SyncRoomSimpleInfo();

			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iterGroup->second[i].GetConnId());
			if (zone != NULL)
			{
				CLProtocol::WorldSyncRoomPasswordInfo sync;
				sync.roomId = GetID();
				sync.password = GetPassword();
				zone->SendToPlayer(newOwnerPlayer.id, sync);
			}

			return;
		}
		++iterGroup;
	}
}

UInt32 CSRoom::CloseSlot(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(slotGroup);
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
			RoomPlayerInfo quitPlayer;

			if (!GetPlayerById(quitPlayer, iterGroup->second[index].GetPlayerId()))
			{
				return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
			}
			GameZone* quitZone = GameZoneMgr::Instance()->FindGameZone(quitPlayer.connId);
			if (quitZone != NULL)
			{
				QuitRoom(quitZone, quitPlayer, RoomSlotStatus::ROOM_SLOT_STATUS_CLOSE);
			}

			SyncKickOutInfo(player, quitPlayer, RoomQuitReason::ROOM_QUIT_OWNER_KICK_OUT);
		}
	}
	else
	{
		return ErrorCode::ROOM_SYSTEM_SLOT_GROUP_ERROR;
	}
	return ErrorCode::SUCCESS;
}

UInt32 CSRoom::SwapSlot(RoomPlayerInfo& player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index)
{

	bool isFound = false;

	//位置索引
	UInt32 reqSlotIndex = 0;
	//位置迭代器
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (player.id == iterGroup->second[i].GetPlayerId())
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

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterFind = m_RoomSlotGroup.find(slotGroup);
	if (iterFind != m_RoomSlotGroup.end())
	{
		if (iterFind->second.size() <= index)
		{
			return ErrorCode::ROOM_SYSTEM_SLOT_INDEX_ERROR;
		}

		if (iterFind->second[index].GetStatus() == RoomSlotStatus::ROOM_SLOT_STATUS_OPEN)
		{
			/*
			//删除之前的位置信息
			iterGroup->second[reqSlotIndex].Init(RoomPlayerInfo(), ROOM_SLOT_STATUS_OPEN);
			//增加新的位置信息
			iterFind->second[index].Init(player, RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
			iterFind->second[index].SetSceneId(iterGroup->second[reqSlotIndex].GetSceneId());
			*/
			//交换位置
			CSRoomSlot tempRoomSlot = iterFind->second[index];
			iterFind->second[index] = iterGroup->second[reqSlotIndex];
			iterGroup->second[reqSlotIndex] = tempRoomSlot;

			SyncRoomSlotInfo(iterGroup->first, reqSlotIndex);
			SyncRoomSlotInfo(iterFind->first, index);

			playerId = 0;
		}
		else if (IsSlotExistPlayer(iterFind->second[index].GetStatus()))
		{
			if (player.id == iterFind->second[index].GetPlayerId()) return ErrorCode::ROOM_SYSTEM_SWAP_NOT_SELF;

			RoomPlayerInfo swapPlayer;
			if (!GetPlayerById(swapPlayer, iterFind->second[index].GetPlayerId()))
			{
				return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
			}

			GameZone* swapZone = GameZoneMgr::Instance()->FindGameZone(swapPlayer.connId);
			if (swapZone == NULL)
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
			sync.playerId = player.id;
			sync.playerLevel = player.level;
			sync.playerName = player.name;
			sync.playerOccu = player.occu;
			sync.playerAwaken = player.awaken;
			sync.slotGroup = (UInt8)iterGroup->first;
			sync.slotIndex = (UInt8)reqSlotIndex;

			swapZone->SendToPlayer(swapPlayer.id, sync);

			playerId = swapPlayer.id;

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

UInt32 CSRoom::ResponseSwapSlot(RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterRes, iterReq;
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	UInt32 resSlotIndex = 0;

	bool resSlotIsExist = false;
	bool reqSlotIsExist = false;

	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player.id)
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
			CSRoomSlot tempRoomSlot = iterRes->second[resSlotIndex];
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

	RoomPlayerInfo reqPlayer;
	if (GetPlayerById(reqPlayer, iterReq->second[index].GetPlayerId()))
	{
		UInt8 swapResult = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_REFUSE;
		if (isAccept)
		{
			swapResult = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_ACCEPT;
		}
		CLProtocol::WorldSyncRoomSwapResultInfo sync;
		sync.result = swapResult;
		sync.playerId = player.id;
		sync.playerName = player.name;
		GameZone* zone = GameZoneMgr::Instance()->FindGameZone(reqPlayer.connId);
		if (zone != NULL)
		{
			zone->SendToPlayer(reqPlayer.id, sync);
		}
	}

	return nRes;
}

UInt32 CSRoom::CheckStartReady()
{
	if (!IsRoomOpen(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_READY_STATUS_ERROR;

	// 乱斗模式，每只队伍必须两人以上才可以
	if (GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
	{
		static UInt32 min_player = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_3V3_TUMBLE_PLAYER_NUM);
		for (auto& iter : m_RoomSlotGroup)
		{
			UInt32 player_size = 0;
			for (auto &it : iter.second)
			{
				if (it.GetAccId() != 0)
					player_size++;
			}

			if (player_size < min_player)
				return ErrorCode::ROOM_SYSTEM_READY_PLAYER_SIZE;
		}
	}

	//每只队伍必须有人
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		bool isError = false;
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (IsSlotExistPlayer(iter->second[i].GetStatus()))
			{
				isError = true;

				RoomPlayerInfo player;
				if (GetPlayerById(player, iter->second[i].GetPlayerId()))
				{
					SceneDataEntry* sceneDataEntry = SceneDataEntryMgr::Instance()->FindEntry(player.sceneId);
					if (sceneDataEntry != NULL)
					{
						if (GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR)
						{
							if (sceneDataEntry->subType != SCENE_SUB_SCORE_WAR)
							{
								return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ROOM;
							}
						}
						else if (GetRoomType() == RoomType::ROOM_TYPE_THREE_FREE || 
							GetRoomType() == RoomType::ROOM_TYPE_THREE_MATCH ||
							GetRoomType() == RoomType::ROOM_TYPE_THREE_TUMBLE)
						{
							if (sceneDataEntry->subType != SCENE_SUB_ROOM)
							{
								return ErrorCode::ROOM_SYSTEM_READY_PLAYER_NOT_ROOM;
							}
						}
					}

					if (GetRoomType() == RoomType::ROOM_TYPE_THREE_SCORE_WAR && player.battleCount <= 0)
					{
						SendBattleCountNotify(player.name);
						return ErrorCode::SCORE_WAR_REWARD_BATTLE_COUNT_ERROR;
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

UInt32 CSRoom::BattleReadyStart()
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

	CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_READY);
	SyncRoomSimpleInfo();

	ResetReadyStartTime();

	return ErrorCode::SUCCESS;
}

UInt32 CSRoom::BattleReadyResult(RoomPlayerInfo& player, RoomSlotReadyStatus slotStatus)
{
	if (!IsRoomReadyStatus(GetRoomStatus())) return ErrorCode::ROOM_SYSTEM_READY_SLOT_STATUS_ERROR;

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();

	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].GetPlayerId() == player.id)
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

UInt32 CSRoom::UpdateMatchInfo(ObjID_t playerId, UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
	while (iter != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iter->second.size(); ++i)
		{
			if (IsSlotNotPlayer(iter->second[i].GetStatus()))
			{
				continue;
			}

			if (iter->second[i].GetPlayerId() == playerId)
			{
				iter->second[i].UpdateMatchInfo(loseStreak, winStreak, matchScore, battleCount);
				return ErrorCode::SUCCESS;
			}
		}
		++iter;
	}
	return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
}

UInt32 CSRoom::CheckBattleReadySlotStatus()
{
	//bool isMatch = true;
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

UInt32 CSRoom::RefuseBattle()
{


	return ErrorCode::SUCCESS;
}

UInt32 CSRoom::BattleMatchStart()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_READY) return ErrorCode::ROOM_SYSTEM_MATCH_ROOM_STATUS_ERROR;
	RoomPlayerInfo ownerPlayer;
	if (!GetPlayerById(ownerPlayer, GetOwnerId())) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(ownerPlayer.connId);
	if (zone == NULL) return ErrorCode::ROOM_SYSTEM_NOT_PLAYER;

	//CLProtocol::SceneRoomStartMatch pkReq;
	CLProtocol::MatchServerPkRoomReq pkReq;
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
	//GetMatchPlayerIds(pkReq.playerIds);
	GetMatchPlayerInfos(pkReq.players);
	zone->TransmitToWorld(pkReq);

	OnStartRoomMatch();

	return ErrorCode::SUCCESS;

	//Result  CLProtocol::MatchServerPkRoomRes
}

void CSRoom::GetRoomSimpleInfo(RoomSimpleInfo& info)
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

void CSRoom::GetRoomInfo(RoomInfo& info)
{
	GetRoomSimpleInfo(info.roomSimpleInfo);
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

void CSRoom::SyncRoomSimpleInfo()
{
	CLProtocol::WorldSyncRoomSimpleInfo sync;
	GetRoomSimpleInfo(sync.info);

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

void CSRoom::SyncRoomPlayerInfoToWorld(GameZone* zone, ObjID_t playerId, UInt32 roomId, UInt32 quitRoomId, UInt32 quitRoomTime)
{
	if (zone == NULL) return;
	CLProtocol::CrossSyncRoomPlayerInfo sync;
	sync.playerId = playerId;
	sync.roomId = roomId;
	sync.quitRoomId = quitRoomId;
	sync.quitRoomTime = quitRoomTime;
	zone->TransmitToWorld(sync);
}

void CSRoom::SendMatchCencelNotify(RoomPlayerInfo& player, UInt32 notifyId)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (player.id == iterGroup->second[i].GetPlayerId() || IsSlotNotPlayer(iterGroup->second[i].GetStatus()))
			{
				continue;
			}

			RoomPlayerInfo targetPlayer;

			if (GetPlayerById(targetPlayer, iterGroup->second[i].GetPlayerId()))
			{
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(targetPlayer.connId);
				if (zone != NULL)
				{
					CLProtocol::CrossSyncRoomNotifyInfo info;
					info.playerId = targetPlayer.id;
					info.notifyId = notifyId;
					info.param1 = player.name;
					zone->TransmitToWorld(info);
				}
			}
		}

		++iterGroup;
	}
}

void CSRoom::Broadcast(Avalon::Protocol& protocol)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

void CSRoom::SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(group);
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

void CSRoom::GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize)
{
	playerSize = 0;
	playerMaxSize = 0;

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

bool CSRoom::IsFull()
{
	UInt32 playerSize = 0;
	UInt32 playerMaxSize = 0;
	GetPlayerSize(playerSize, playerMaxSize);
	if (playerSize >= playerMaxSize)
		return true;
	return false;
}

bool CSRoom::IsInRoom(UInt64 playerId)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

RoomSlotGroup CSRoom::GetRoomSlotGroup(UInt64 playerId)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

UInt32 CSRoom::GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

UInt32 CSRoom::InvitePlayer(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	for (UInt32 i = 0; i < m_RoomInviteList.size(); ++i)
	{
		if (m_RoomInviteList[i].GetBeInvitePlayerId() == beInvitePlayerId && m_RoomInviteList[i].GetInvitePlayerId() == invitePlayerId)
		{
			return ErrorCode::ROOM_SYSTEM_INVITE_IN_INVITE_LIST;
		}
	}

	CSRoomInvite invite(invitePlayerId, beInvitePlayerId, (UInt32)CURRENT_TIME.Sec());
	m_RoomInviteList.push_back(invite);
	return ErrorCode::SUCCESS;
}

UInt64 CSRoom::GetInvitePlayerId(UInt64 beInvitePlayerId)
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

void CSRoom::DeleteInviteInfo(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	std::vector<CSRoomInvite>::iterator iter = m_RoomInviteList.begin();

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

UInt32 CSRoom::CheckInviteTimeout(UInt64 invitePlayerId, UInt64 beInvitePlayerId)
{
	std::vector<CSRoomInvite>::iterator iter = m_RoomInviteList.begin();
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

void CSRoom::GetMatchPlayerIds(std::vector<UInt64>& matchPlayerIds)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			matchPlayerIds.push_back(iterGroup->second[i].GetPlayerId());
		}
		++iterGroup;
	}
}

void CSRoom::GetMatchPlayerInfos(std::vector<PkRoomMatchPlayerInfo>& matchPlayerInfos)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	UInt32 index = 0;
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() != 0)
			{
				PkRoomMatchPlayerInfo matchPlayer;
				matchPlayer.accid = iterGroup->second[i].GetAccId();
				UInt32 zoneid = CL_GET_ZONEID_FROM_NODEID(iterGroup->second[i].GetConnId());
				matchPlayer.wsId = CL_MAKE_NODEID(zoneid, ST_WORLD, 1);
				matchPlayer.id = iterGroup->second[i].GetPlayerId();
				matchPlayer.level = iterGroup->second[i].GetPlayerLevel();
				matchPlayer.loseStreak = iterGroup->second[i].GetLoseStreak();
				matchPlayer.winStreak = iterGroup->second[i].GetWinStreak();
				matchPlayer.name = iterGroup->second[i].GetPlayerName();
				matchPlayer.score = iterGroup->second[i].GetMatchScore();
				matchPlayer.seasonLevel = iterGroup->second[i].GetPlayerSeasonLevel();
				matchPlayer.seasonStar = iterGroup->second[i].GetSeasonStar();
				matchPlayer.seasonExp = iterGroup->second[i].GetSeasonExp();
				matchPlayer.seat = index;
				matchPlayerInfos.push_back(matchPlayer);
			}
			index += 1;
		}
		++iterGroup;
	}
}

void CSRoom::ClearQuitSwapInfo(CSRoomSlot* roomSlot, RoomSlotGroup slotGroup, UInt32 slotIndex)
{
	if (roomSlot == NULL) return;
	if (!roomSlot->IsSwapStatus()) return;

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.find(roomSlot->GetTargetSlotGroup());
	if (iterGroup != m_RoomSlotGroup.end() && iterGroup->second.size() > roomSlot->GetTargetSlotIndex())
	{
		if (iterGroup->second[roomSlot->GetTargetSlotIndex()].CheckSwapInfo(slotGroup, slotIndex, m_SwapSlotTimeInterval, (UInt32)CURRENT_TIME.Sec()))
		{
			GameZone* zone = GameZoneMgr::Instance()->FindGameZone(iterGroup->second[roomSlot->GetTargetSlotIndex()].GetConnId());
			if (zone != NULL)
			{
				CLProtocol::WorldSyncRoomSwapResultInfo sync;
				sync.result = (UInt8)RoomSwapResult::ROOM_SWAP_RESULT_CANCEL;
				sync.playerId = roomSlot->GetPlayerId();
				sync.playerName = roomSlot->GetPlayerName();
				zone->SendToPlayer(iterGroup->second[roomSlot->GetTargetSlotIndex()].GetPlayerId(), sync);
			}
			iterGroup->second[roomSlot->GetTargetSlotIndex()].ClearSwapInfo();
		}
	}

	roomSlot->ClearSwapInfo();
}

void CSRoom::SyncKickOutInfo(RoomPlayerInfo& kickOutPlayer, RoomPlayerInfo& player, RoomQuitReason reason)
{
	CLProtocol::WorldSyncRoomKickOutInfo sync;
	sync.reason = reason;

	sync.kickPlayerId = kickOutPlayer.id;
	sync.kickPlayerName = kickOutPlayer.name;

	sync.roomId = GetID();

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
	if (zone != NULL)
	{
		zone->SendToPlayer(player.id, sync);
	}

	m_RoomKickOutPlayers.push_back(CSRoomKickOutPlayerInfo(player.id, (UInt32)CURRENT_TIME.Sec()));
}

UInt32 CSRoom::CheckKickOutInfo(RoomPlayerInfo& player)
{
	std::vector<CSRoomKickOutPlayerInfo>::iterator iter = m_RoomKickOutPlayers.begin();
	while (iter != m_RoomKickOutPlayers.end())
	{
		if (iter->GetPlayerId() == player.id)
		{
			return ErrorCode::ROOM_SYSTEM_KICKOUT_JOIN_ERROR;
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

void CSRoom::UpdateAvatar(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

void CSRoom::UpdateVipLevel(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

void CSRoom::UpdateSceneId(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

bool CSRoom::GetPlayerById(RoomPlayerInfo& info, ObjID_t playerId)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

void CSRoom::QuitPlayerByConnId(UInt32 connId, std::set<UInt32>& dissRoomSet)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

void CSRoom::SendRoomCreateUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_CREATE, 0);
}

void CSRoom::SendRoomDeleteUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_DELETE, 0);
}

void CSRoom::SendRoomJoinUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_JOIN, 0);
}

void CSRoom::SendRoomQuitUdpLog(GameZone* zone, ObjID_t playerId)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_QUIT, 0);
}

void CSRoom::SendRoomMatchStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_MATCH_START, roomPlayerSize);
}

void CSRoom::SendRoomBattleStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize)
{
	SendUdpLog(zone, playerId, LogRoomOperationType::ROOM_OPERATION_TYPE_BATTLE_START, roomPlayerSize);
}

void CSRoom::SendUdpLog(GameZone* zone, ObjID_t playerId, LogRoomOperationType opType, UInt32 roomPlayerSize)
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

void CSRoom::OnPlayerOnline(RoomPlayerInfo& player)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_WAIT);
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
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

void CSRoom::OnPlayerOffline(RoomPlayerInfo& player)
{
}

void CSRoom::OnDisconnect(RoomPlayerInfo& player)
{
	/*
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
	SyncRoomPlayerInfoToWorld(zone, player.id, 0, GetID(), (UInt32)CURRENT_TIME.Sec());
	*/

	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			if (iterGroup->second[i].GetPlayerId() == player.id)
			{
				iterGroup->second[i].SetStatus(RoomSlotStatus::ROOM_SLOT_STATUS_OFFLINE);
				/*
				CLProtocol::WorldSyncRoomInfo sync;
				GetRoomInfo(sync.info);
				if (zone != NULL)
				{
					zone->SendToPlayer(player.id, sync);
				}
				*/
				SyncRoomSlotInfo(iterGroup->first, i);

				if (player.id == GetOwnerId())
				{
					SetOwnerOfflineTime((UInt32)CURRENT_TIME.Sec());
				}

				return;
			}
		}
		++iterGroup;
	}
}

void CSRoom::OnReconnectSuccess(RoomPlayerInfo& player)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(player.connId);
	SyncRoomPlayerInfoToWorld(zone, player.id, 0, GetID(), (UInt32)CURRENT_TIME.Sec());
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
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

void CSRoom::OnTick(const Avalon::Time& now)
{
	if (GetRoomStatus() == RoomStatus::ROOM_STATUS_READY)
	{
		//准备时间
		UInt32 nowSec = (UInt32)now.Sec();
		if (nowSec > m_ReadyStartTime + m_ReadyTimeInterval)
		{
			BattleReadySlotTimeOut();
			UpdateSlotReadyStatus(RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID);
		}
	}
	else if (IsRoomOpen(GetRoomStatus()))
	{
		//检查交换位置时间 , 检查是否需要变更房主
		CheckSwapSlotInfos();
	}

	UInt32 nowSec = (UInt32)now.Sec();
	std::vector<CSRoomKickOutPlayerInfo>::iterator iterKickout = m_RoomKickOutPlayers.begin();
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

	std::vector<CSRoomInvite>::iterator inviteIter = m_RoomInviteList.begin();
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

void CSRoom::OnStartRoomMatch()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_MATCH)
	{
		CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_MATCH);
		SyncRoomSimpleInfo();
	}
}

void CSRoom::OnCancleRoomMatch()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_OPEN)
	{
		CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
		SyncRoomSimpleInfo();
	}
}

void CSRoom::OnStartRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_BATTLE)
	{
		CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_BATTLE);
		SyncRoomSimpleInfo();
	}
}

void CSRoom::OnEndRoomRace()
{
	if (GetRoomStatus() != RoomStatus::ROOM_STATUS_OPEN)
	{
		CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
		SyncRoomSimpleInfo();
	}
}

void CSRoom::UpdateSlotReadyStatus(RoomSlotReadyStatus slotStatus)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iterGroup = m_RoomSlotGroup.begin();
	while (iterGroup != m_RoomSlotGroup.end())
	{
		for (UInt32 i = 0; i < iterGroup->second.size(); ++i)
		{
			iterGroup->second[i].SetReadyStatus(slotStatus);
			SyncRoomSlotInfo(iterGroup->first, i);
		}
		++iterGroup;
	}
}

UInt32 CSRoom::GetReadyStartTime()
{
	return m_ReadyStartTime;
}

void CSRoom::ResetReadyStartTime()
{
	m_ReadyStartTime = (UInt32)CURRENT_TIME.Sec();
}

void CSRoom::BattleReadySlotTimeOut()
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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

	CSRoomMgr::Instance()->ChangeRoomStatus(this, RoomStatus::ROOM_STATUS_OPEN);
	SyncRoomSimpleInfo();

}

void CSRoom::SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent)
{
	if (zone == NULL) return;

	CLProtocol::SceneRoomSyncInfo sync;
	sync.playerId = player.id;
	sync.roomEvent = (UInt8)roomEvent;
	sync.roomId = GetID();
	zone->TransmitToScene(sync);
}

void CSRoom::SendBattleCountNotify(std::string name)
{
	std::string content = SysNotifyMgr::Instance()->MakeString(9936, name);

	CLProtocol::SysNotify notify;
	notify.type = 20;
	notify.channel = 0;
	notify.color = 0;
	notify.word = content;

	RoomPlayerInfo playerInfo;
	if (!GetPlayerById(playerInfo, GetOwnerId()))
	{
		return;
	}

	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(playerInfo.connId);
	if (zone == NULL) return;
	zone->SendToPlayer(playerInfo.id, notify);
}

void CSRoom::CheckSwapSlotInfos()
{
	UInt32 nowTime = (UInt32)CURRENT_TIME.Sec();
	bool isDismiss = true;
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.begin();
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
		CSRoomMgr::Instance()->DismissRoom(NULL, 0, GetID());
	}
}

bool CSRoom::GetPlayerBySlot(RoomPlayerInfo& info, RoomSlotGroup slotGroup, UInt32 slotIndex, bool isNormal)
{
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>>::iterator iter = m_RoomSlotGroup.find(slotGroup);
	if (iter != m_RoomSlotGroup.end())
	{
		if (iter->second.size() > slotIndex)
		{
			if (iter->second[slotIndex].GetPlayerId() == 0) return false;
			/*
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
			*/
		}
	}
	return NULL;
}

