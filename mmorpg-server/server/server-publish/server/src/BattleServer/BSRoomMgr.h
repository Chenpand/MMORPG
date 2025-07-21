#ifndef _BS_ROOM_MGR_H__
#define _BS_ROOM_MGR_H__

#include <AvalonSingleton.h>
#include "BSRoom.h"
#include "GameZone.h"

class BSRoomMgr : public Avalon::Singleton<BSRoomMgr>
{
public:
	BSRoomMgr();
	~BSRoomMgr();

public:
	//��������
	UInt32 CreateRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, RoomType roomType);

	//��ɢ����
	UInt32 DismissRoom(GameZone* zone, ObjID_t playerId, UInt32 roomId);

	//���뷿��
	UInt32 JoinRoom(GameZone* zone, RoomPlayerInfo& player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group);

	//�˳�����
	UInt32 QuitRoom(GameZone* zone, RoomPlayerInfo& player);

public:
	BSRoom* GetRoom(UInt32 roomId);

	void ChangeRoomStatus(BSRoom* room, RoomStatus roomStatus);

public:

	void OnPlayerOnline(GameZone* zone, RoomPlayerInfo& player);

	void OnPlayerOffline(GameZone* zone, RoomPlayerInfo& player);

	void OnTick(const Avalon::Time& now);

	void OnStartRoomRace(GameZone* zone, RoomPlayerInfo& player);

	void OnDisconnect(UInt32 connId);

	void OnDisconnect(GameZone* zone, RoomPlayerInfo& player);

	void OnReconnectSuccess(GameZone* zone, RoomPlayerInfo& player);

private:

	void RecycleRoom(BSRoom* room);

private:

	//������ʹ�õķ���
	std::map<UInt32, BSRoom*> m_Rooms;

	//����û��ʹ�õķ���
	std::list<BSRoom*> m_NotUseRooms;

	//��ǰ���Է����ID
	UInt32 m_CurrentID;

	//��󷿼��
	UInt32 m_MaxID;

	//��С�����
	UInt32 m_MinID;
};


#endif //_BS_ROOM_MGR_H__