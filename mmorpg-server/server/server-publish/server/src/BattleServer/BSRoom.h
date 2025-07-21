#ifndef _BS_ROOM_H__
#define _BS_ROOM_H__

#include <CLDefine.h>
#include <CLRoomDefine.h>
#include <CLObject.h>
#include "GameZoneMgr.h"


class BSRoomSlot
{
public:
	BSRoomSlot();
	~BSRoomSlot();

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

	UInt8 GetPlayerVipLevel() { return m_PlayerVipLevel; }
	void SetPlayerVipLevel(UInt8 vipLevel) { m_PlayerVipLevel = vipLevel; }

	UInt8 GetPlayerOccu() { return m_PlayerOccu; }
	void SetPlayerOccu(UInt8 occu) { m_PlayerOccu = occu; }

	PlayerAvatar GetPlayerAvatar() { return m_PlayerAvatar; }
	void SetPlayerAvatar(PlayerAvatar avatar) { m_PlayerAvatar = avatar; }

	RoomSlotStatus GetStatus() { return m_Status; }
	void SetStatus(RoomSlotStatus status) { m_Status = status; }

	UInt32 GetSceneId() { return m_SceneId; }
	void SetSceneId(UInt32 sceneId) { m_SceneId = sceneId; }

	UInt32 GetConnId() { return m_ConnId; }
	void SetConnId(UInt32 connId) { m_ConnId = connId; }

	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	void GetRoomSlotInfo(RoomSlotInfo& info, UInt32 index, RoomSlotGroup status);

	void GetRoomPlayerInfo(RoomPlayerInfo& info);

private:
	UInt32 m_AccId;
	//玩家ID
	UInt64 m_PlayerId;
	//玩家名字
	std::string m_PlayerName;
	//玩家等级
	UInt16 m_PlayerLevel;
	//vip等级
	UInt8 m_PlayerVipLevel;
	//玩家职业
	UInt8 m_PlayerOccu;
	//玩家avatar
	PlayerAvatar m_PlayerAvatar;
	//槽位状态
	RoomSlotStatus m_Status;
	//所在场景
	UInt32 m_SceneId;

	//ConnId
	UInt32 m_ConnId;
};

typedef CLVisitor<BSRoomSlot> CSRoomSlotVisitor;

class BSRoom
{
public:
	BSRoom();
	BSRoom(UInt32 id, RoomType roomType);
	~BSRoom();

	//初始化房间
	void InitSlot();

public:
	UInt32 GetID() { return m_ID; }
	void SetID(UInt32 id) { m_ID = id; }

	RoomType GetRoomType() { return m_RoomType; }
	void SetRoomType(RoomType type) { m_RoomType = type; }

	RoomStatus GetRoomStatus() { return m_RoomStatus; }
	void SetRoomStatus(RoomStatus status) { m_RoomStatus = status; }

	UInt32 GetCreateTime() { return m_CreateTime; }
	void SetCreateTime(UInt32 createTime) { m_CreateTime = createTime; }

	UInt32 GetOpenTime() { return m_OpenTime; }
	void SetOpenTime(UInt32 openTime) { m_OpenTime = openTime; }

public:

	//进入房间
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);

	//退出房间
	void QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	//解散房间
	UInt32 DismissRoom();

	//战斗准备开始
	UInt32 BattleReadyStart();

public:

	//简单房间信息
	void GetRoomSimpleInfo(RoomSimpleInfo& info);

	//房间信息
	void GetRoomInfo(RoomInfo& info);

	//给房间成员同步房间信息
	void SyncRoomSimpleInfo();

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
	void SendRoomBattleStartUdpLog(GameZone* zone, ObjID_t playerId, UInt32 roomPlayerSize);

	void SendUdpLog(GameZone* zone, ObjID_t playerId, LogRoomOperationType opType, UInt32 roomPlayerSize);

public: //事件
	
	//玩家上线事件
	void OnPlayerOnline(RoomPlayerInfo& player);

	//玩家离线事件
	void OnPlayerOffline(RoomPlayerInfo& player);

	//tick
	void OnTick(const Avalon::Time& now);

	//开始房间战斗事件
	void OnStartRoomRace();

	//战斗结束事件
	void OnEndRoomRace();

private:

	void SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent);

private:
	//房间ID
	UInt32 m_ID;

	//房间类型
	RoomType m_RoomType;

	//房间状态
	RoomStatus m_RoomStatus;

	//所有玩家
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>> m_RoomSlotGroup;

	//一边队伍槽位数
	UInt32 m_RoomSlotMaxSize;

	//房间创建时间
	UInt32 m_CreateTime;

	//房间打开时间
	UInt32 m_OpenTime;
};

#endif //_BS_ROOM_H__