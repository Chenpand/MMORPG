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
	//房间列表 startIndex从0开始
	UInt32 GetRoomList(RoomListInfo& info, UInt32 startIndex, UInt32 count, UInt32 playerLevel, UInt32 playerSeasonLevel, RoomStatus status, RoomType type, bool isPassword);

	//创建房间
	UInt32 CreateRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//重新设置房间
	UInt32 UpdateRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomType roomType, std::string name, std::string password, UInt8 isPlayerLevel, UInt16 playerLevel, UInt8 isPlayerSeasonLevel, UInt32 playerSeasonLevel);

	//解散房间
	UInt32 DismissRoom(WSPlayer* player, UInt32 roomId);

	//进入房间
	UInt32 JoinRoom(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, std::string password, UInt32 createTime);

	//快速加入房间
	UInt32 QuickJoinRoom(WSPlayer* player, RoomInfo& roomInfo, RoomType roomType);

	//退出房间
	UInt32 QuitRoom(WSPlayer* player);

	//踢出房间
	UInt32 KickOutRoom(WSPlayer* player, UInt64 kickPlayerId);

	//邀请玩家
	UInt32 InviteJoin(WSPlayer* player, UInt64 joinPlayerId);

	//被邀请玩家进入房间
	UInt32 BeInviteJoin(WSPlayer* player, RoomInfo& roomInfo, UInt32 roomId, RoomSlotGroup group, UInt64 invitePlayerId, bool isAccept);

	//转让房主
	UInt32 ChangeOwner(WSPlayer* player, UInt64 newOwnerId);

	//关闭位置
	UInt32 CloseSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 SwapSlot(WSPlayer* player, UInt64& playerId, RoomSlotGroup slotGroup, UInt32 index);

	//交换位置
	UInt32 ResponseSwapSlot(WSPlayer* player, RoomSlotGroup slotGroup, UInt32 index, bool isAccept);

	//战斗准备开始
	UInt32 BattleReadyStart(WSPlayer* player);

	//战斗准备取消
	UInt32 BattleMatchCencel(WSPlayer* player);

	//战斗准备结果
	UInt32 BattleReadyResult(WSPlayer* player, RoomSlotReadyStatus slotStatus);

	//发送邀请链接
	UInt32 SendInviteLink(WSPlayer* player, UInt32 roomId, UInt8 channel);

	//改变位置状态
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

	//所有在使用的房间
	std::map<UInt32, Room*> m_Rooms;

	//所有没有使用的房间
	std::list<Room*> m_NotUseRooms;

	std::map<RoomStatus, std::vector<Room*>> m_RoomByStatus;

	//已经打开,但未开战的房间
	//std::vector<Room*> m_OpenRooms;

	//已经开战的房间
	//std::vector<Room*> m_BattleRooms;

	//当前可以分配的ID
	UInt32 m_CurrentID;

	//最大房间号
	UInt32 m_MaxID;

	//最小房间号
	UInt32 m_MinID;

	//是否跨服
	bool m_IsCross;

};


#endif //__ROOM_MGR_H__