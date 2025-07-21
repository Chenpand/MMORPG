#ifndef _CS_ROOM_H__
#define _CS_ROOM_H__

#include <CLDefine.h>
#include <CLRoomDefine.h>
#include <CLObject.h>
#include "GameZoneMgr.h"

struct PkRoomMatchPlayerInfo;

class CSRoomKickOutPlayerInfo
{
public:
	CSRoomKickOutPlayerInfo(UInt64 playerId, UInt32 kickoutTime);

	~CSRoomKickOutPlayerInfo() {}

	UInt64 GetPlayerId() { return m_PlayerId; }

	UInt32 GetKickOutTime() { return m_KickOutTime; }

public:
	UInt64 m_PlayerId;
	UInt32 m_KickOutTime;
};

class CSRoomInvite
{
public:

	CSRoomInvite(UInt64 invitePlayerId, UInt64 beInvitePlayerId, UInt32 inviteTime) 
		: m_InvitePlayerId(invitePlayerId), m_BeInvitePlayerId(beInvitePlayerId), m_InviteTime(inviteTime) { }

	~CSRoomInvite() { };

public:
	UInt64 GetInvitePlayerId() { return m_InvitePlayerId; }

	UInt64 GetBeInvitePlayerId() { return m_BeInvitePlayerId; }

	UInt32 GetInviteTime() { return m_InviteTime; }

public:
	//邀请人ID
	UInt64 m_InvitePlayerId;
	//被邀请人ID
	UInt64 m_BeInvitePlayerId;
	//邀请时间
	UInt32 m_InviteTime;
};

class CSRoomSlot
{
public:
	CSRoomSlot();
	~CSRoomSlot();

	void Init(RoomPlayerInfo player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

public:
	UInt32 GetPlayerAccId() { return m_AccId; }
	void SetPlayerAccId(UInt32 accId) { m_AccId = accId; }

	UInt64 GetPlayerId() { return m_PlayerId; }
	void SetPlayerId(UInt64 id) { m_PlayerId = id; }

	std::string GetPlayerName() { return m_PlayerName; }
	void SetPlayerName(std::string name) { m_PlayerName = name; }

	UInt16 GetPlayerLevel() { return m_PlayerLevel; }
	void SetPlayerLevel(UInt16 level) { m_PlayerLevel = level; }

	UInt32 GetPlayerSeasonLevel() { return m_PlayerSeasonLevel; }
	void SetPlayerSeasonLevel(UInt32 seasonLevel) { m_PlayerSeasonLevel = seasonLevel; }

	UInt8 GetPlayerVipLevel() { return m_PlayerVipLevel; }
	void SetPlayerVipLevel(UInt8 vipLevel) { m_PlayerVipLevel = vipLevel; }

	UInt8 GetPlayerOccu() { return m_PlayerOccu; }
	void SetPlayerOccu(UInt8 occu) { m_PlayerOccu = occu; }

	UInt8 GetPlayerAwaken() { return m_PlayerAwaken; }
	void SetPlayerAwaken(UInt8 awaken) { m_PlayerAwaken = awaken; }

	PlayerAvatar GetPlayerAvatar() { return m_PlayerAvatar; }
	void SetPlayerAvatar(PlayerAvatar avatar) { m_PlayerAvatar = avatar; }

	RoomSlotStatus GetStatus() { return m_Status; }
	void SetStatus(RoomSlotStatus status) { m_Status = status; }

	RoomSlotReadyStatus GetReadyStatus() { return m_ReadyStatus; }
	void SetReadyStatus(RoomSlotReadyStatus status) { m_ReadyStatus = status; }

	UInt32 GetSceneId() { return m_SceneId; }
	void SetSceneId(UInt32 sceneId) { m_SceneId = sceneId; }

	UInt32 GetConnId() { return m_ConnId; }
	void SetConnId(UInt32 connId) { m_ConnId = connId; }

	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	UInt32 GetLoseStreak() { return m_LoseStreak; }
	void SetLoseStreak(UInt32 loseStreak) { m_LoseStreak = loseStreak; }

	UInt32 GetWinStreak() { return m_WinStreak; }
	void SetWinStreak(UInt32 winStreak) { m_WinStreak = winStreak; }

	UInt32 GetMatchScore() { return m_MatchScore; }
	void SetMatchScore(UInt32 matchScore) { m_MatchScore = matchScore; }

	UInt32 GetSeasonStar() { return m_SeasonStar; }
	void SetSeasonStar(UInt32 star) { m_SeasonStar = star; }

	UInt32 GetSeasonExp() { return m_SeasonExp; }
	void SetSeasonExp(UInt32 exp) { m_SeasonExp = exp; }

	void GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup status);

	void GetRoomPlayerInfo(RoomPlayerInfo& info);

	RoomSlotGroup GetTargetSlotGroup() { return m_TargetSlotGroup; }
	void SetTargetSlotGroup(RoomSlotGroup slotGroup) { m_TargetSlotGroup = slotGroup; }

	UInt32 GetTargetSlotIndex() { return m_TargetSlotIndex; }
	void SetTargetSlotIndex(UInt32 slotIndex) { m_TargetSlotIndex = slotIndex; }

	UInt32 GetSwapStartTime() { return m_SwapStartTime; }
	void SetSwapStartTime(UInt32 startTime) { m_SwapStartTime = startTime; }

	UInt32 GetBattleCount() { return m_BattleCount; }
	void SetBattleCount(UInt32 count) { m_BattleCount = count; }

	UInt32 GetHeadFrame() { return m_HeadFrame; }
	void SetHeadFrame(UInt32 id) { m_HeadFrame = id; }

	void SetSwapSlotInfo(RoomSlotGroup slotGroup, UInt32 slotIndex, UInt32 startTime);

	bool IsSwapStatus();

	bool CheckSwapInfo(RoomSlotGroup targetSlotGroup, UInt32 targetSlotIndex, UInt32 timeInterval, UInt32 nowTime);

	bool CheckSwapInfoInvaild(UInt32 timeInterval, UInt32 nowTime);

	void ClearSwapInfo();

	void UpdateMatchInfo(UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount);

private:
	UInt32 m_AccId;
	//玩家ID
	UInt64 m_PlayerId;
	//玩家名字
	std::string m_PlayerName;
	//玩家等级
	UInt16 m_PlayerLevel;
	//玩家段位
	UInt32 m_PlayerSeasonLevel;
	//vip等级
	UInt8 m_PlayerVipLevel;
	//玩家职业
	UInt8 m_PlayerOccu;
	//玩家觉醒
	UInt8 m_PlayerAwaken;
	//玩家avatar
	PlayerAvatar m_PlayerAvatar;
	//槽位状态
	RoomSlotStatus m_Status;
	//战斗准备状态
	RoomSlotReadyStatus m_ReadyStatus;
	//所在场景
	UInt32 m_SceneId;


	//匹配信息状态
	//RoomMatchInfoStatus m_MatchInfoStatus;
	//连败数
	UInt32 m_LoseStreak;
	//连胜数
	UInt32 m_WinStreak;
	//匹配分
	UInt32 m_MatchScore;
	//赛季星星
	UInt32 m_SeasonStar;
	//赛季经验
	UInt32 m_SeasonExp;
	//战斗次数
	UInt32 m_BattleCount;
	// 头像框
	UInt32 m_HeadFrame;

	//ConnId
	UInt32 m_ConnId;

	//交换位置
	RoomSlotGroup m_TargetSlotGroup;
	//对方的位置索引
	UInt32 m_TargetSlotIndex;
	//交换位置时间
	UInt32 m_SwapStartTime;

};

typedef CLVisitor<CSRoomSlot> CSRoomSlotVisitor;

class CSRoom
{
public:
	CSRoom();
	CSRoom(UInt32 id, RoomType roomType, std::string name, UInt8 isLimitPlayerLevel, UInt16 limitPlayerLevel, UInt8 isLimitPlayerSeasonLevel, UInt32 limitPlayerSeasonLevel);
	~CSRoom();

	//初始化房间
	void InitSlot();

public:
	UInt32 GetID() { return m_ID; }
	void SetID(UInt32 id) { m_ID = id; }

	std::string GetName() { return m_Name; }
	void SetName(std::string name) { m_Name = name; }

	std::string GetPassword() { return m_Password; }
	void SetPassword(std::string password) { m_Password = password; }

	RoomType GetRoomType() { return m_RoomType; }
	void SetRoomType(RoomType type) { m_RoomType = type; }

	RoomStatus GetRoomStatus() { return m_RoomStatus; }
	void SetRoomStatus(RoomStatus status) { m_RoomStatus = status; }

	UInt8 GetIsLimitPlayerLevel() { return m_IsLimitPlayerLevel; }
	void SetIsLimitPlayerLevel(UInt8 isLimitPlayerLevel) { m_IsLimitPlayerLevel = isLimitPlayerLevel; }

	UInt16 GetLimitPlayerLevel() { return m_LimitPlayerLevel; }
	void SetLimitPlayerLevel(UInt16 level) { m_LimitPlayerLevel = level; }

	UInt8 GetIsLimitPlayerSeasonLevel() { return m_IsLimitPlayerSeasonLevel; }
	void SetIsLimitPlayerSeasonLevel(UInt8 isLimitPlayerSeasonLevel) { m_IsLimitPlayerSeasonLevel = isLimitPlayerSeasonLevel; }

	UInt32 GetLimitPlayerSeasonLevel() { return m_LimitPlayerSeasonLevel; }
	void SetLimitPlayerSeasonLevel(UInt32 level) { m_LimitPlayerSeasonLevel = level; }

	//WSPlayer* GetOwner() { return m_Owner; }
	//void SetOwner(WSPlayer* owner) { m_Owner = owner; }

	UInt64 GetOwnerId() { return m_OwnerId; }
	void SetOwnerId(UInt64 ownerId) { m_OwnerId = ownerId; }

	UInt8 GetOwnerOccu() { return m_OwnerOccu; }
	void SetOwnerOccu(UInt8 occu) { m_OwnerOccu = occu; }

	UInt32 GetOwnerSeasonLevel() { return m_OwnerSeasonLevel; }
	void SetOwnerSeasonLevel(UInt32 seasonLevel) { m_OwnerSeasonLevel = seasonLevel; }

	UInt32 GetCreateTime() { return m_CreateTime; }
	void SetCreateTime(UInt32 createTime) { m_CreateTime = createTime; }

	UInt32 GetOpenTime() { return m_OpenTime; }
	void SetOpenTime(UInt32 openTime) { m_OpenTime = openTime; }

	UInt32 GetOwnerOfflineTime() { return m_OwnerOfflineTime; }
	void SetOwnerOfflineTime(UInt32 offlineTime){ m_OwnerOfflineTime = offlineTime; }

public:

	UInt32 CheckPassword(std::string& password);

	//进入房间
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);

	//退出房间
	void QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	//解散房间
	UInt32 DismissRoom();

	//更改房主
	void ChangeOwner(UInt64 newOwnerId = 0);

	//更改房间
	UInt32 CloseSlot(GameZone* zone, RoomPlayerInfo& player,RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 SwapSlot(RoomPlayerInfo& player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 ResponseSwapSlot(RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//是否能匹配
	UInt32 CheckStartReady();

	//战斗准备开始
	UInt32 BattleReadyStart();

	//战斗状态返回
	UInt32 BattleReadyResult(RoomPlayerInfo& player, RoomSlotReadyStatus slotStatus);

	//匹配需要的信息
	UInt32 UpdateMatchInfo(ObjID_t playerId, UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount);

	//检查战斗状态
	UInt32 CheckBattleReadySlotStatus();

	//拒绝进入战斗
	UInt32 RefuseBattle();

	//开始匹配
	UInt32 BattleMatchStart();

public:

	//简单房间信息
	void GetRoomSimpleInfo(RoomSimpleInfo& info);

	//房间信息
	void GetRoomInfo(RoomInfo& info);

	//给房间成员同步房间信息
	void SyncRoomSimpleInfo();

	//同步玩家信息
	void SyncRoomPlayerInfoToWorld(GameZone* zone, ObjID_t playerId, UInt32 roomId, UInt32 quitRoomId, UInt32 quitRoomTime);

	//发送取消匹配消息
	void SendMatchCencelNotify(RoomPlayerInfo& player, UInt32 notifyId);

	void Broadcast(Avalon::Protocol& protocol);

	//给房间成员同步队伍信息
	void SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf = true);

	//获取玩家数量和房间最大人数
	void GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize);

	bool IsFull();

	//是否已经在房间
	bool IsInRoom(UInt64 playerId);

	//获取队伍
	RoomSlotGroup GetRoomSlotGroup(UInt64 playerId);

	//获取队伍位置
	UInt32 GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex);

	//邀请玩家
	UInt32 InvitePlayer(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//根据被邀请玩家获取邀请玩家
	UInt64 GetInvitePlayerId(UInt64 beInvitePlayerId);

	//删除邀请信息
	void DeleteInviteInfo(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//检查邀请信息是否超时
	UInt32 CheckInviteTimeout(UInt64 invitePlayerId, UInt64 beInvitePlayerId);

	//获取匹配需要的玩家信息
	void GetMatchPlayerIds(std::vector<UInt64>& matchPlayerIds);

	//获取匹配需要的玩家信息
	void GetMatchPlayerInfos(std::vector<PkRoomMatchPlayerInfo>& matchPlayerInfos);

	//清空交换信息
	void ClearQuitSwapInfo(CSRoomSlot* roomSlot, RoomSlotGroup slotGroup, UInt32 slotIndex);

	//同步踢人信息
	void SyncKickOutInfo(RoomPlayerInfo& kickOutPlayer, RoomPlayerInfo& player, RoomQuitReason reason);

	//检查是否刚刚被踢出
	UInt32 CheckKickOutInfo(RoomPlayerInfo& player);

	//更新avator信息
	void UpdateAvatar(RoomPlayerInfo& player);

	//更新vip等级
	void UpdateVipLevel(RoomPlayerInfo& player);

	//更新所在场景
	void UpdateSceneId(RoomPlayerInfo& player);

	bool GetPlayerById(RoomPlayerInfo& info, ObjID_t playerId);

	void QuitPlayerByConnId(UInt32 connId, std::set<UInt32>& dissRoomSet);

public:
	void SendRoomCreateUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomDeleteUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomJoinUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomQuitUdpLog(GameZone* zone, ObjID_t playerId);
	void SendRoomMatchStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize);
	void SendRoomBattleStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize);

	void SendUdpLog(GameZone* zone, ObjID_t playerId, LogRoomOperationType opType, UInt32 roomPlayerSize);

public: //事件
	
	//玩家上线事件
	void OnPlayerOnline(RoomPlayerInfo& player);

	//玩家离线事件
	void OnPlayerOffline(RoomPlayerInfo& player);

	//断开连接事件
	void OnDisconnect(RoomPlayerInfo& player);

	//重连事件
	void OnReconnectSuccess(RoomPlayerInfo& player);

	//tick
	void OnTick(const Avalon::Time& now);

	//开始房间匹配事件
	void OnStartRoomMatch();

	//取消房间匹配事件
	void OnCancleRoomMatch();

	//开始房间战斗事件
	void OnStartRoomRace();

	//战斗结束事件
	void OnEndRoomRace();

	//修改房间成员的准备状态
	void UpdateSlotReadyStatus(RoomSlotReadyStatus slotStatus);

private:
	UInt32 GetReadyStartTime();

	void ResetReadyStartTime();
	
	void BattleReadySlotTimeOut();

	void SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent);

	void SendBattleCountNotify(std::string name);

	void CheckSwapSlotInfos();

	bool GetPlayerBySlot(RoomPlayerInfo& info, RoomSlotGroup slotGroup, UInt32 slotIndex, bool isNormal = true);

private:
	//房间ID
	UInt32 m_ID;

	//房间名
	std::string m_Name;

	//房间密码
	std::string m_Password;

	//房间类型
	RoomType m_RoomType;

	//房间状态
	RoomStatus m_RoomStatus;

	//是否启用玩家等级限制
	UInt8 m_IsLimitPlayerLevel;

	//玩家等级限制
	UInt16 m_LimitPlayerLevel;

	//是否启用玩家段位限制
	UInt8 m_IsLimitPlayerSeasonLevel;

	//玩家段位限制
	UInt32 m_LimitPlayerSeasonLevel;

	//所有玩家
	std::map<RoomSlotGroup, std::vector<CSRoomSlot>> m_RoomSlotGroup;
	//std::vector<RoomSlot> m_RoomSlots;

	//一边队伍槽位数
	UInt32 m_RoomSlotMaxSize;

	//房主ID
	UInt64 m_OwnerId;
	//WSPlayer* m_Owner;

	//房主职业
	UInt8 m_OwnerOccu;

	//房主段位
	UInt32 m_OwnerSeasonLevel;

	//邀请玩家列表
	std::vector<CSRoomInvite> m_RoomInviteList;

	//战斗准备状态开始时间
	UInt32 m_ReadyStartTime;

	//战斗准备状态持续时间
	UInt32 m_ReadyTimeInterval;

	//邀请持续时间
	UInt32 m_InviteTimeInterval;

	//交换位置持续时间
	UInt32 m_SwapSlotTimeInterval;

	//被踢出房间再次进入时间间隔
	UInt32 m_KickOutTimeInterval;

	//房间创建时间
	UInt32 m_CreateTime;

	//房间打开时间
	UInt32 m_OpenTime;

	//房主离线时间
	UInt32 m_OwnerOfflineTime;

	//被踢出房间的人
	std::vector<CSRoomKickOutPlayerInfo> m_RoomKickOutPlayers;

};

#endif //__ROOM_H__