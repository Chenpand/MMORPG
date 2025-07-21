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
	//���ID
	UInt64 m_PlayerId;
	//�������
	std::string m_PlayerName;
	//��ҵȼ�
	UInt16 m_PlayerLevel;
	//vip�ȼ�
	UInt8 m_PlayerVipLevel;
	//���ְҵ
	UInt8 m_PlayerOccu;
	//���avatar
	PlayerAvatar m_PlayerAvatar;
	//��λ״̬
	RoomSlotStatus m_Status;
	//���ڳ���
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

	//��ʼ������
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

	//���뷿��
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup group = RoomSlotGroup::ROOM_SLOT_GROUP_INVALID);

	//�˳�����
	void QuitRoom(GameZone* zone, RoomPlayerInfo& player, RoomSlotStatus status = RoomSlotStatus::ROOM_SLOT_STATUS_OPEN);

	//��ɢ����
	UInt32 DismissRoom();

	//ս��׼����ʼ
	UInt32 BattleReadyStart();

public:

	//�򵥷�����Ϣ
	void GetRoomSimpleInfo(RoomSimpleInfo& info);

	//������Ϣ
	void GetRoomInfo(RoomInfo& info);

	//�������Աͬ��������Ϣ
	void SyncRoomSimpleInfo();

	void Broadcast(Avalon::Protocol& protocol);

	//�������Աͬ��������Ϣ
	void SyncRoomSlotInfo(RoomSlotGroup group, UInt32 index, bool isSyncSelf = true);

	//��ȡ��������ͷ����������
	void GetPlayerSize(UInt32& playerSize, UInt32& playerMaxSize);

	bool IsFull();

	//�Ƿ��Ѿ��ڷ���
	bool IsInRoom(UInt64 playerId);

	//��ȡ����
	RoomSlotGroup GetRoomSlotGroup(UInt64 playerId);

	//��ȡ����λ��
	UInt32 GetRoomSlot(UInt64 playerId, RoomSlotGroup& slotGroup, UInt32& slotIndex);

	//����avator��Ϣ
	void UpdateAvatar(RoomPlayerInfo& player);

	//����vip�ȼ�
	void UpdateVipLevel(RoomPlayerInfo& player);

	//�������ڳ���
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

public: //�¼�
	
	//��������¼�
	void OnPlayerOnline(RoomPlayerInfo& player);

	//��������¼�
	void OnPlayerOffline(RoomPlayerInfo& player);

	//tick
	void OnTick(const Avalon::Time& now);

	//��ʼ����ս���¼�
	void OnStartRoomRace();

	//ս�������¼�
	void OnEndRoomRace();

private:

	void SenceSyncRoomInfo(GameZone* zone, RoomPlayerInfo& player, RoomEvent roomEvent);

private:
	//����ID
	UInt32 m_ID;

	//��������
	RoomType m_RoomType;

	//����״̬
	RoomStatus m_RoomStatus;

	//�������
	std::map<RoomSlotGroup, std::vector<BSRoomSlot>> m_RoomSlotGroup;

	//һ�߶����λ��
	UInt32 m_RoomSlotMaxSize;

	//���䴴��ʱ��
	UInt32 m_CreateTime;

	//�����ʱ��
	UInt32 m_OpenTime;
};

#endif //_BS_ROOM_H__