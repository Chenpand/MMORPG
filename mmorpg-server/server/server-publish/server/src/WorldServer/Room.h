#ifndef __ROOM_H__
#define __ROOM_H__

#include <CLDefine.h>
#include <CLRoomDefine.h>
#include <CLObject.h>
#include <CLProtocolDefine.h>

class WSPlayer;
struct PkRoomMatchPlayerInfo;

class RoomKickOutPlayerInfo
{
public:
	RoomKickOutPlayerInfo(UInt64 playerId, UInt32 kickoutTime);

	~RoomKickOutPlayerInfo() {}

	UInt64 GetPlayerId() { return m_PlayerId; }

	UInt32 GetKickOutTime() { return m_KickOutTime; }

public:
	UInt64 m_PlayerId;
	UInt32 m_KickOutTime;
};

class RoomInvite
{
public:

	RoomInvite(UInt64 invitePlayerId, UInt64 beInvitePlayerId, UInt32 inviteTime) 
		: m_InvitePlayerId(invitePlayerId), m_BeInvitePlayerId(beInvitePlayerId), m_InviteTime(inviteTime) { }

	~RoomInvite() { };

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

class RoomSlot
{
public:
	RoomSlot();
	~RoomSlot();

	void Init(WSPlayer* player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	void Init(RoomSlotInfo& slotInfo);

public:

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

	void GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup status);

	RoomSlotGroup GetTargetSlotGroup() { return m_TargetSlotGroup; }
	void SetTargetSlotGroup(RoomSlotGroup slotGroup) { m_TargetSlotGroup = slotGroup; }

	UInt32 GetTargetSlotIndex() { return m_TargetSlotIndex; }
	void SetTargetSlotIndex(UInt32 slotIndex) { m_TargetSlotIndex = slotIndex; }

	UInt32 GetSwapStartTime() { return m_SwapStartTime; }
	void SetSwapStartTime(UInt32 startTime) { m_SwapStartTime = startTime; }

	void SetSwapSlotInfo(RoomSlotGroup slotGroup, UInt32 slotIndex, UInt32 startTime);

	bool IsSwapStatus();

	bool CheckSwapInfo(RoomSlotGroup targetSlotGroup, UInt32 targetSlotIndex, UInt32 timeInterval, UInt32 nowTime);

	bool CheckSwapInfoInvaild(UInt32 timeInterval, UInt32 nowTime);

	void ClearSwapInfo();

private:
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

	//交换位置
	RoomSlotGroup m_TargetSlotGroup;
	//对方的位置索引
	UInt32 m_TargetSlotIndex;
	//交换位置时间
	UInt32 m_SwapStartTime;

};

typedef CLVisitor<RoomSlot> RoomSlotVisitor;

class Room
{
public:
	Room();
	Room(UInt32 id, RoomType roomType, std::string name, UInt8 isLimitPlayerLevel, UInt16 limitPlayerLevel, UInt8 isLimitPlayerSeasonLevel, UInt32 limitPlayerSeasonLevel);
	~Room();

	//初始化房间
	void InitSlot();

	void SetSlots(std::vector<RoomSlotInfo> roomSlots, bool isSync = true);

	void SetSlot(RoomSlotInfo& slotInfo);

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
	UInt32 JoinRoom(WSPlayer* player, RoomSlotGroup group = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);

	//退出房间
	void QuitRoom(WSPlayer* player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	//解散房间
	UInt32 DismissRoom();

	//更改房主
	void ChangeOwner(UInt64 newOwnerId = 0);

	//更改房间
	UInt32 CloseSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 SwapSlot(WSPlayer* player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 ResponseSwapSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//是否能匹配
	UInt32 CheckStartReady();

	//战斗准备开始
	UInt32 BattleReadyStart();

	//战斗状态返回
	UInt32 BattleReadyResult(WSPlayer* player, RoomSlotReadyStatus slotStatus);

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

	//发送取消匹配消息
	void SendMatchCencelNotify(WSPlayer* player, UInt32 notifyId);

	//发送消息
	void Broadcast(Avalon::Protocol& protocol);

	//给房间成员同步队伍信息
	void SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf = true);

	//获取玩家数量和房间最大人数
	void GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize);

	bool IsFull();

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

	//清空交换信息
	void ClearQuitSwapInfo(RoomSlot* roomSlot, RoomSlotGroup slotGroup, UInt32 slotIndex);

	//同步踢人信息
	void SyncKickOutInfo(WSPlayer* kickOutPlayer, WSPlayer* player, RoomQuitReason reason);

	//检查是否刚刚被踢出
	UInt32 CheckKickOutInfo(WSPlayer* player);

	//更新avator信息
	void UpdateAvatar(WSPlayer* player);

	//更新vip等级
	void UpdateVipLevel(WSPlayer* player);

public: //事件
	
	//玩家上线事件
	void OnPlayerOnline(WSPlayer* player);

	//玩家离线事件
	void OnPlayerOffline(WSPlayer* player);

	//断开连接事件
	void OnDisconnect(WSPlayer* player);

	//重连事件
	void OnReconnectSuccess(WSPlayer* player);

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

private:
	UInt32 GetReadyStartTime();

	void ResetReadyStartTime();
	
	void BattleReadySlotTimeOut();

	void SenceSyncRoomInfo(WSPlayer* player, RoomEvent roomEvent);

	void CheckSwapSlotInfos();

	WSPlayer* GetPlayerBySlot(RoomSlotGroup slotGroup, UInt32 slotIndex, bool isNormal = true);

private:
	//房间ID
	UInt32 m_ID;

	//房间名
	std::string m_Name;

	//房间密码
	std::string m_Password;

	//房间类型
	RoomType m_RoomType;

	//房间名称
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
	std::map<RoomSlotGroup, std::vector<RoomSlot>> m_RoomSlotGroup;
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
	std::vector<RoomInvite> m_RoomInviteList;

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
	std::vector<RoomKickOutPlayerInfo> m_RoomKickOutPlayers;

};

#endif //__ROOM_H__