#ifndef _CS_ROOM_MGR_H__
#define _CS_ROOM_MGR_H__

#include <AvalonSingleton.h>
#include "CSRoom.h"
#include "GameZone.h"

class CSRoomMgr : public Avalon::Singleton<CSRoomMgr>
{
public:
	CSRoomMgr();
	~CSRoomMgr();

public:
	//�����б� startIndex��0��ʼ
	UInt32 GetRoomList(RoomListInfo& info, UInt32 startIndex, UInt32 count, UInt32 playerLevel, UInt32 playerSeasonLevel, RoomStatus status, RoomType type, bool isPassword);

	//��������
	UInt32 CreateRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//�������÷���
	UInt32 UpdateRoom(RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//��ɢ����
	UInt32 DismissRoom(GameZone* zone, ObjID_t playerId, UInt32 roomId);

	//���뷿��
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, std::string password, UInt32 createTime);

	//���ټ��뷿��
	UInt32 QuickJoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType);

	//�˳�����
	UInt32 QuitRoom(GameZone* zone, RoomPlayerInfo& player);

	//�߳�����
	UInt32 KickOutRoom(GameZone* zone, RoomPlayerInfo& player, UInt64 kickPlayerId);
	void KickOutRoomByPlayerId(UInt64 playerId);

	//�������
	UInt32 InviteJoin(RoomPlayerInfo& player, UInt64 joinPlayerId);

	//��������ҽ��뷿��
	UInt32 BeInviteJoin(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, UInt64 invitePlayerId, bool isAccept);

	//ת�÷���
	UInt32 ChangeOwner(RoomPlayerInfo& player, UInt64 newOwnerId);

	//�ر�λ��
	UInt32 CloseSlot(GameZone* zone, RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 SwapSlot(RoomPlayerInfo& player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 ResponseSwapSlot(RoomPlayerInfo& player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//ս��׼����ʼ
	UInt32 BattleReadyStart(GameZone* zone, RoomPlayerInfo& player);

	//ս��׼��ȡ��
	UInt32 BattleMatchCencel(GameZone* zone, RoomPlayerInfo& player);

	//ս��׼�����
	UInt32 BattleReadyResult(GameZone* zone, RoomPlayerInfo& player, RoomSlotReadyStatus slotStatus);

	//������������
	UInt32 SendInviteLink(RoomPlayerInfo& player, UInt32 roomId, UInt8 channel);

	//���·���ƥ����Ϣ
	UInt32 UpdateRoomMatchInfo(UInt32 roomId, ObjID_t playerId, UInt32 loseStreak, UInt32 winStreak, UInt32 matchScore, UInt32 battleCount);

	//�ı�λ��״̬
	//UInt32 ChangeSlotStatus(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index);

public:
	CSRoom* GetRoom(UInt32 roomId);

	void ChangeRoomStatus(CSRoom* room, RoomStatus roomStatus);

	void StopMatch(RoomType type);

	void AddPlayerMap(ObjID_t playerId, UInt32 roomId);
	void DelPlayerMap(ObjID_t playerId);
	UInt32 GetPlayerRoomID(ObjID_t playerId);

public:

	void OnPlayerOnline(GameZone* zone, RoomPlayerInfo& player);

	void OnPlayerOffline(GameZone* zone, RoomPlayerInfo& player);

	void OnTick(const Avalon::Time& now);

	void OnStartRoomRace(GameZone* zone, RoomPlayerInfo& player);

	void OnCancleRoomMatch(RoomPlayerInfo& player);

	void OnDisconnect(UInt32 connId);

	void OnDisconnect(GameZone* zone, RoomPlayerInfo& player);

	void OnReconnectSuccess(GameZone* zone, RoomPlayerInfo& player);

	void OnChangeScene(ObjID_t playerId, UInt32 sceneId);

private:

	void RecycleRoom(CSRoom* room);

	void RemoveStatusRoom(RoomStatus status, CSRoom* room);

	void InsertStatusRoom(CSRoom* room);

	bool IsStrValid(const std::string& str, UInt32 maxWidth, bool isName);

	UInt32 IsNameValid(const std::string& name);

	UInt32 IsPasswordValid(const std::string& password);

private:

	//������ʹ�õķ���
	std::map<UInt32, CSRoom*> m_Rooms;

	//����û��ʹ�õķ���
	std::list<CSRoom*> m_NotUseRooms;

	std::map<RoomStatus, std::vector<CSRoom*>> m_RoomByStatus;

	std::map<ObjID_t, UInt32> m_PlayerMap;

	//�Ѿ���,��δ��ս�ķ���
	//std::vector<Room*> m_OpenRooms;

	//�Ѿ���ս�ķ���
	//std::vector<Room*> m_BattleRooms;

	//��ǰ���Է����ID
	UInt32 m_CurrentID;

	//��󷿼��
	UInt32 m_MaxID;

	//��С�����
	UInt32 m_MinID;
};


#endif //__ROOM_MGR_H__