#ifndef __ROOM_MGR_H__
#define __ROOM_MGR_H__

#include <AvalonSingleton.h>
#include "Room.h"

class RoomMgr : public Avalon::Singleton<RoomMgr>
{
public:
	RoomMgr();
	~RoomMgr();

public:
	//�����б� startIndex��0��ʼ
	UInt32 GetRoomList(RoomListInfo& info, UInt32 startIndex, UInt32 count, UInt32 playerLevel, UInt32 playerSeasonLevel, RoomStatus status, RoomType type, bool isPassword);

	//��������
	UInt32 CreateRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//�������÷���
	UInt32 UpdateRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//��ɢ����
	UInt32 DismissRoom(WSPlayer* player, UInt32 roomId);

	//���뷿��
	UInt32 JoinRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, std::string password, UInt32 createTime);

	//���ټ��뷿��
	UInt32 QuickJoinRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType);

	//�˳�����
	UInt32 QuitRoom(WSPlayer* player);

	//�߳�����
	UInt32 KickOutRoom(WSPlayer* player, UInt64 kickPlayerId);

	//�������
	UInt32 InviteJoin(WSPlayer* player, UInt64 joinPlayerId);

	//��������ҽ��뷿��
	UInt32 BeInviteJoin(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, UInt64 invitePlayerId, bool isAccept);

	//ת�÷���
	UInt32 ChangeOwner(WSPlayer* player, UInt64 newOwnerId);

	//�ر�λ��
	UInt32 CloseSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 SwapSlot(WSPlayer* player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//����λ��
	UInt32 ResponseSwapSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//ս��׼����ʼ
	UInt32 BattleReadyStart(WSPlayer* player);

	//ս��׼��ȡ��
	UInt32 BattleMatchCencel(WSPlayer* player);

	//ս��׼�����
	UInt32 BattleReadyResult(WSPlayer* player, RoomSlotReadyStatus slotStatus);

	//������������
	UInt32 SendInviteLink(WSPlayer* player, UInt32 roomId, UInt8 channel);

	//�ı�λ��״̬
	//UInt32 ChangeSlotStatus(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index);

	
public:

	UInt32 CrossSetRoomInfo(RoomInfo& info, std::string password, UInt32 createTime);

	UInt32 CrossSetRoomSimpleInfo(RoomSimpleInfo& info, std::string password);

	UInt32 CrossCreateRoom(WSPlayer* player, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	UInt32 CrossCreateRoomHandler(WSPlayer* player, UInt32 roomId);

	UInt32 CrossUpdateRoom(WSPlayer* player, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	UInt32 CrossUpdateRoomHandler(WSPlayer* player, UInt32 roomId);

	UInt32 CrossJoinRoom(UInt32 roomId, RoomSlotInfo& info);
	
public:
	Room* GetRoom(UInt32 roomId);

	void ChangeRoomStatus(Room* room, RoomStatus roomStatus);

	void UpdateAvatar(WSPlayer* player);

	void UpdateVipLevel(WSPlayer* player);

	bool IsCross() { return m_IsCross; }

	void GetRoomPlayerInfo(WSPlayer* player, RoomPlayerInfo& info);

public:

	void OnPlayerOnline(WSPlayer* player);

	void OnPlayerOffline(WSPlayer* player);

	void OnTick(const Avalon::Time& now);

	void OnStartRoomRace(WSPlayer* player);

	void OnCancleRoomMatch(WSPlayer* player);

	void OnDisconnect(WSPlayer* player);

	void OnReconnectSuccess(WSPlayer* player);

	void OnChangeScene(WSPlayer* player, UInt32 sceneId);

private:

	void RecycleRoom(Room* room);

	void RemoveStatusRoom(RoomStatus status, Room* room);

	void InsertStatusRoom(Room* room);

	bool IsStrValid(const std::string& str, UInt32 maxWidth, bool isName);

	UInt32 IsNameValid(const std::string& name);

	UInt32 IsPasswordValid(const std::string& password);

private:

	//������ʹ�õķ���
	std::map<UInt32, Room*> m_Rooms;

	//����û��ʹ�õķ���
	std::list<Room*> m_NotUseRooms;

	std::map<RoomStatus, std::vector<Room*>> m_RoomByStatus;

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

	//�Ƿ���
	bool m_IsCross;

};


#endif //__ROOM_MGR_H__