#ifndef _CL_ROOM_PROTOCOL_H_
#define _CL_ROOM_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRoomDefine.h"
#include <AvalonPacket.h>

namespace CLProtocol
{
	/**
	*@brief server->client 同步房间信息
	*/
	class WorldSyncRoomInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}
	public:
		RoomInfo info;
	};

	/**
	*@brief server->client 同步房间简单信息
	*/
	class WorldSyncRoomSimpleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_SIMPLE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}
	public:
		RoomSimpleInfo info;
	};

	/**
	*@brief server->client 同步房间玩家信息
	*/
	class WorldSyncRoomSlotInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_SLOT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & slotInfo;
		}
	public:
		RoomSlotInfo slotInfo;
	};

	/**
	*@brief server->client 通知被邀请玩家 邀请信息
	*/
	class WorldSyncRoomInviteInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_INVITE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & roomName & roomType & inviterId & inviterName & inviterOccu & inviterAwaken & inviterLevel & playerSize & playerMaxSize & group;
		}
	public:
		//房间号
		UInt32 roomId;
		//房间名
		std::string roomName;
		//房间类型
		UInt8 roomType;
		//邀请者id
		UInt64 inviterId;
		//邀请者名字
		std::string inviterName;
		//邀请者职业
		UInt8 inviterOccu;
		//邀请者觉醒
		UInt8 inviterAwaken;
		//邀请者等级
		UInt16 inviterLevel;
		//房间人数
		UInt32 playerSize;
		//房间最大人数
		UInt32 playerMaxSize;
		//边
		UInt8 group;
	};

	/**
	*@brief server->client 通知被踢出玩家
	*/
	class WorldSyncRoomKickOutInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_KICK_OUT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & kickPlayerId & kickPlayerName & roomId;
		}
	public:
		//踢出房间原因
		UInt32 reason;
		//踢出的玩家ID
		UInt64 kickPlayerId;
		//踢出的玩家名字
		std::string kickPlayerName;
		//踢出的房间ID
		UInt32 roomId;

	};

	/**
	*@brief server->client 通知邀请者,被邀请玩家的返回
	*/
	class WorldSyncRoomBeInviteInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_BE_INVITE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & isAccept;
		}

	public:
		//玩家ID
		UInt64 playerId;
		//是否接受
		UInt8 isAccept;
	};

	/**
	*@brief server->client 通知玩家交换位置信息
	*/
	class WorldSyncRoomSwapSlotInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_SWAP_SLOT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & playerName & playerLevel & playerOccu & playerAwaken & slotGroup & slotIndex;
		}

	public:
		//玩家ID
		UInt64 playerId;

		//玩家名
		std::string playerName;

		//玩家等级
		UInt16 playerLevel;

		//玩家职业
		UInt8 playerOccu;

		//玩家觉醒
		UInt8 playerAwaken;

		//队伍
		UInt8 slotGroup;

		//位置
		UInt8 slotIndex;
	};

	/**
	*@brief server->client 通知玩家交换位置返回信息
	*/
	class WorldSyncRoomSwapResultInfo :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_SWAP_SLOT_RESULT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & playerId & playerName;
		}
	public:
		//返回结果
		UInt8 result;
		//响应者ID
		UInt64 playerId;
		//响应者名字
		std::string playerName;
	};

	/**
	*@brief server->client 通知玩家密码信息
	*/
	class WorldSyncRoomPasswordInfo :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROOM_PASSWORD_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & password;
		}
	public:

		//房间id
		UInt32 roomId;

		//密码
		std::string password;
	};

	/**
	*@brief server->client 同步战斗结束信息
	*/
	class SceneRoomMatchPkRaceEnd : public Avalon::Protocol
	{

		AVALON_DEFINE_PROTOCOLID(SCENE_ROOM_MATCH_PK_RACE_END)
	public:
		SceneRoomMatchPkRaceEnd()
		{
			pkType = 0;
			raceId = 0;
			result = 0;
			oldPkValue = 0;
			newPkValue = 0;
			oldMatchScore = 0;
			newMatchScore = 0;
			oldPkCoin = 0;
			addPkCoinFromRace = 0;
			addPkCoinFromActivity = 0;
			oldSeasonLevel = 0;
			newSeasonLevel = 0;
			oldSeasonStar = 0;
			newSeasonStar = 0;
			oldSeasonExp = 0;
			newSeasonExp = 0;
			changeSeasonExp = 0;
			getHonor = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pkType & raceId & result & oldPkValue & newPkValue & oldMatchScore & newMatchScore & oldPkCoin & addPkCoinFromRace &
				totalPkCoinFromRace & isInPvPActivity & addPkCoinFromActivity & totalPkCoinFromActivity & oldSeasonLevel & 
				newSeasonLevel & oldSeasonStar & newSeasonStar & oldSeasonExp & newSeasonExp & changeSeasonExp & slotInfos & getHonor;
		}

	public:
		// PK类型，对应枚举(PkType)
		UInt8		pkType;
		UInt64		raceId;
		UInt8		result;
		UInt32		oldPkValue;
		UInt32		newPkValue;
		UInt32		oldMatchScore;
		UInt32		newMatchScore;
		// 初始决斗币数量
		UInt32		oldPkCoin;
		// 战斗获得的决斗币
		UInt32		addPkCoinFromRace;
		// 今日战斗获得的全部决斗币
		UInt32		totalPkCoinFromRace;
		// 是否在PVP活动期间
		UInt8		isInPvPActivity;
		// 活动额外获得的决斗币
		UInt32		addPkCoinFromActivity;
		// 今日活动获得的全部决斗币
		UInt32		totalPkCoinFromActivity;
		// 原段位
		UInt32		oldSeasonLevel;
		// 现段位
		UInt32		newSeasonLevel;
		//原星
		UInt32		oldSeasonStar;
		//现星
		UInt32		newSeasonStar;
		//原经验
		UInt32		oldSeasonExp;
		//现经验
		UInt32		newSeasonExp;
		//改变的经验
		Int32		changeSeasonExp;
		//队伍信息
		std::vector<RoomSlotBattleEndInfo> slotInfos;
		//获得荣誉
		UInt32		getHonor;
	};

	/**
	*@brief World->Sence 通知Scene匹配开始
	*/
	class SceneRoomStartMatch :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROOM_START_MATCH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & playerIds & pkType;
		}

	public:
		UInt32 roomId;
		std::vector<UInt64> playerIds;
		UInt8 pkType;
	};

	/**
	*@brief World->Sence 通知Scene房间信息
	*/
	class SceneRoomSyncInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROOM_SYNC_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & roomId & roomEvent;
		}

	public:
		UInt64 playerId;

		UInt32 roomId;
		
		UInt8 roomEvent;
	};

	/**
	*@brief client->server 请求房间列表
	*/
	class WorldRoomListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & limitPlayerLevel & limitPlayerSeasonLevel & roomStatus & roomType & isPassword & startIndex & count;
		}

	public:

		//玩家等级
		UInt16 limitPlayerLevel;

		//玩家段位
		UInt32 limitPlayerSeasonLevel;

		//房间状态
		UInt8 roomStatus;

		//房间类型
		UInt8 roomType;

		//是否有密码
		UInt8 isPassword;

		//开始位置
		UInt32 startIndex;

		//个数
		UInt32 count;
	};

	/**
	*@brief server->client 请求房间列表返回
	*/
	class WorldRoomListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & roomList;
		}

	public:
		UInt32 result;
		RoomListInfo roomList;
	};

	/**
	*@brief client->server 请求创建或更新房间
	*/
	class WorldUpdateRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & roomType & name & password & isLimitPlayerLevel & limitPlayerLevel & isLimitPlayerSeasonLevel & limitPlayerSeasonLevel;
		}
	public:
		//房间号
		UInt32 roomId;
		//房间类型
		UInt8 roomType;
		//房间名
		std::string name;
		//房间密码
		std::string password;
		//是否启用房间限制等级
		UInt8 isLimitPlayerLevel;
		//房间限制等级
		UInt16 limitPlayerLevel;
		//是否启用房间限制段位
		UInt8 isLimitPlayerSeasonLevel;
		//房间限制段位
		UInt32 limitPlayerSeasonLevel;

	};

	/**
	*@brief server->client 请求创建或更新房间返回
	*/
	class WorldUpdateRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & info;
		}
	public:
		UInt32 result;
		//房间信息
		RoomInfo info;
	};

	/**
	*@brief client->server 请求进入房间
	*/
	class WorldJoinRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_JOIN_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & roomType & password & createTime;
		}
	public:
		UInt32 roomId;
		//房间类型
		UInt8 roomType;
		//密码
		std::string password;
		//创建时间
		UInt32 createTime;
	};

	/**
	*@brief server->client 请求进入房间返回
	*/
	class WorldJoinRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_JOIN_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & info;
		}
	public:
		UInt32 result;
		//房间信息
		RoomInfo info;
	};

	/**
	*@brief client->server 请求退出房间
	*/
	class WorldQuitRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUIT_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 请求退出房间返回
	*/
	class WorldQuitRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUIT_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求踢出房间
	*/
	class WorldKickOutRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_KICK_OUT_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

	public:
		//被踢出的玩家id
		UInt64 playerId;
	};

	/**
	*@brief server->client 请求踢出房间返回
	*/
	class WorldKickOutRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_KICK_OUT_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求解散房间
	*/
	class WorldDismissRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISMISS_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 请求解散房间返回
	*/
	class WorldDismissRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISMISS_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求邀请进入房间
	*/
	class WorldInviteJoinRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INVITE_JOIN_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}
	public:
		//被邀请的玩家id
		UInt64 playerId;
	};

	/**
	*@brief server->client 请求邀请进入房间返回
	*/
	class WorldInviteJoinRoomRes :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INVITE_JOIN_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求转让房主
	*/
	class WorldChangeRoomOwnerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGE_ROOM_OWNER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}
	public:
		UInt64 playerId;
	};

	/**
	*@brief server->client 请求转让房主返回
	*/
	class WorldChangeRoomOwnerRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGE_ROOM_OWNER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 被邀请请求
	*/
	class WorldBeInviteRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BE_INVITE_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & invitePlayerId & isAccept & slotGroup;
		}

	public:
		//房间号
		UInt32 roomId;

		//邀请人ID
		UInt64 invitePlayerId;

		//是否接受
		UInt8 isAccept;

		//队伍
		UInt8 slotGroup;
	};

	/**
	*@brief server->client 被邀请请求返回
	*/
	class WorldBeInviteRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BE_INVITE_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & info;
		}

	public:
		UInt32 result;
		RoomInfo info;

	};

	/**
	*@brief client->server 请求关闭打开位置
	*/
	class WorldRoomCloseSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_CLOSE_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & slotGroup & index;
		}

	public:
		//队伍
		UInt8 slotGroup;
		//索引
		UInt8 index;
	};
	
	/**
	*@brief server->client 请求关闭打开位置返回
	*/
	class WorldRoomCloseSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_CLOSE_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求房间交换位置
	*/
	class WorldRoomSwapSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_SWAP_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & slotGroup & index;
		}

	public:
		//房间号
		UInt32 roomId;

		//队伍
		UInt8 slotGroup;

		//索引
		UInt8 index;

	};

	/**
	*@brief server->client 请求房间交换位置返回
	*/
	class WorldRoomSwapSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_SWAP_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & playerId;
		}

	public:
		UInt32 result;

		UInt64 playerId;
	};

	/**
	*@brief client->server 请求响应房间交换位置
	*/
	class WorldRoomResponseSwapSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_RESPONSE_SWAP_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isAccept & slotGroup & slotIndex;
		}

	public:
		UInt8 isAccept;
		UInt8 slotGroup;
		UInt8 slotIndex;
	};

	
	/**
	*@brief server->client 请求响应房间交换位置返回
	*/
	class WorldRoomResponseSwapSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_RESPONSE_SWAP_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求开始匹配战斗
	*/
	class WorldRoomBattleStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:

	};

	/**
	*@brief server->client 请求开始匹配战斗返回
	*/
	class WorldRoomBattleStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求取消匹配战斗
	*/
	class WorldRoomBattleCancelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_CANCEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:

	};

	/**
	*@brief server->client 请求取消匹配战斗返回
	*/
	class WorldRoomBattleCancelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_CANCEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求战斗准备
	*/
	class WorldRoomBattleReadyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_READY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & slotStatus;
		}

	public:
		UInt8 slotStatus;
	};

	/**
	*@brief server->client 请求战斗准备返回
	*/
	class WorldRoomBattleReadyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_BATTLE_READY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 发送邀请信息
	*/
	class WorldRoomSendInviteLinkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_SEND_INVITE_LINK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & channel;
		}
	public:
		UInt32 roomId;
		UInt8 channel;
	};

	/**
	*@brief server->client 发送邀请信息返回
	*/
	class WorldRoomSendInviteLinkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ROOM_SEND_INVITE_LINK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief CROSS->WORLD 同步玩家数据
	*/
	class CrossSyncRoomPlayerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & roomId & quitRoomId & quitRoomTime;
		}
	public:
		ObjID_t playerId;
		UInt32 roomId;
		UInt32 quitRoomId;
		UInt32 quitRoomTime;
	};

	/**
	*@brief CROSS->WORLD 同步房间日志
	*/
	class CrossSyncRoomLogInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_LOG_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & opType & roomId & roomType & reason & createTime & playerSize;
		}
	public:
		ObjID_t playerId;
		UInt32 opType;
		UInt32 roomId;
		UInt32 roomType;
		std::string reason;
		UInt32 createTime;
		UInt32 playerSize;
	};

	/**
	*@brief CROSS->WORLD 发送notify消息
	*/
	class CrossSyncRoomNotifyInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_NOTIFY_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & notifyId & param1;
		}

	public:
		ObjID_t playerId;
		UInt32 notifyId;
		std::string param1;
	};

	/**
	*@brief WORLD->CROSS 通知房间战斗开始
	*/
	class CrossSyncRoomStartRace : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_START_RACE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player;
		}
	public:
		RoomPlayerInfo player;
	};

	/**
	*@brief WORLD->CROSS 通知玩家上线
	*/
	class CrossSyncRoomPlayerOnline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_ONLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player;
		}

	public:
		RoomPlayerInfo player;
	};

	/**
	*@brief WORLD->CROSS 通知玩家离线
	*/
	class CrossSyncRoomPlayerOffline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_OFFLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player;
		}

	public:
		RoomPlayerInfo player;
	};

	/**
	*@brief WORLD->CROSS 通知玩家断线
	*/
	class CrossSyncRoomPlayerDisconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_DISCONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player;
		}

	public:
		RoomPlayerInfo player;
	};

	/**
	*@brief WORLD->CROSS 通知玩家断线重连
	*/
	class CrossSyncRoomPlayerReconnect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_RECONNECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player;
		}

	public:
		RoomPlayerInfo player;
	};

	/**
	*@brief WORLD->CROSS 通知玩家改变场景
	*/
	class CrossSyncRoomPlayerChangeScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_SYNC_ROOM_PLAYER_CHANGE_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & player & sceneId;
		}

	public:
		RoomPlayerInfo player;
		UInt32 sceneId;
	};


	/**
	*@brief WORLD->CROSS 请求房间列表
	*/
	class CrossWorldRoomListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_WORLD_ROOM_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & limitPlayerLevel & limitPlayerSeasonLevel & roomStatus & roomType & isPassword & startIndex & count;
		}

	public:

		//玩家ID
		ObjID_t playerId;

		//玩家等级
		UInt16 limitPlayerLevel;

		//玩家段位
		UInt32 limitPlayerSeasonLevel;

		//房间状态
		UInt8 roomStatus;

		//房间类型
		UInt8 roomType;

		//是否有密码
		UInt8 isPassword;

		//开始位置
		UInt32 startIndex;

		//个数
		UInt32 count;
	};

	/**
	*@brief CROSS->WORLD 请求房间列表返回
	*/
	class CrossWorldRoomListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_WORLD_ROOM_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result & roomList;
		}

	public:
		ObjID_t playerId;
		UInt32 result;
		RoomListInfo roomList;
	};

	/**
	*@brief WORLD->CROSS 请求创建或更新房间
	*/
	class CrossUpdateRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_UPDATE_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & roomType & name & password & isLimitPlayerLevel & limitPlayerLevel & isLimitPlayerSeasonLevel & limitPlayerSeasonLevel & playerInfo;
		}
	public:
		//房间号
		UInt32 roomId;
		//房间类型
		UInt8 roomType;
		//房间名
		std::string name;
		//房间密码
		std::string password;
		//是否启用房间限制等级
		UInt8 isLimitPlayerLevel;
		//房间限制等级
		UInt16 limitPlayerLevel;
		//是否启用房间限制段位
		UInt8 isLimitPlayerSeasonLevel;
		//房间限制段位
		UInt32 limitPlayerSeasonLevel;
		//创建房间的角色信息
		RoomPlayerInfo playerInfo;
	};

	/**
	*@brief CROSS->WORLD 请求创建或更新房间返回
	*/
	class CrossUpdateRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_UPDATE_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & ownerId & isCreate & info;
		}
	public:
		UInt32 result;
		//房主ID
		ObjID_t ownerId;
		//是否创建房间
		UInt8 isCreate;
		//房间信息
		RoomInfo info;
	};

	/**
	*@brief WORLD->CROSS 请求进入房间
	*/
	class CrossJoinRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_JOIN_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & roomType & password & createTime & playerInfo;
		}
	public:
		UInt32 roomId;
		//房间类型
		UInt8 roomType;
		//密码
		std::string password;
		//创建时间
		UInt32 createTime;
		//角色信息
		RoomPlayerInfo playerInfo;
	};

	/**
	*@brief CROSS->WORLD 请求进入房间返回
	*/
	class CrossJoinRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_JOIN_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & ownerId & info;
		}
	public:
		UInt32 result;
		//房主ID
		ObjID_t ownerId;
		//房间信息
		RoomInfo info;
	};

	/**
	*@brief WORLD->CROSS 请求退出房间
	*/
	class CrossQuitRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_QUIT_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		RoomPlayerInfo info;
	};

	/**
	*@brief CROSS->WORLD 请求退出房间返回
	*/
	class CrossQuitRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_QUIT_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & ownerId;
		}

	public:
		UInt32 result;
		ObjID_t ownerId;
	};

	/**
	*@brief WORLD->CROSS 请求踢出房间
	*/
	class CrossKickOutRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_KICK_OUT_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & playerId;
		}

	public:
		RoomPlayerInfo info;

		//被踢出的玩家id
		UInt64 playerId;
	};

	/**
	*@brief CROSS->WORLD 请求踢出房间返回
	*/
	class CrossKickOutRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_KICK_OUT_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}

	public:
		ObjID_t playerId;

		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求解散房间
	*/
	class CrossDismissRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_DISMISS_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		RoomPlayerInfo info;
	};

	/**
	*@brief CROSS->WORLD 请求解散房间返回
	*/
	class CrossDismissRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_DISMISS_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求邀请进入房间
	*/
	class CrossInviteJoinRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_INVITE_JOIN_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & playerId;
		}
	public:
		RoomPlayerInfo info;

		//被邀请的玩家id
		UInt64 playerId;
	};

	/**
	*@brief CROSS->WORLD 请求邀请进入房间返回
	*/
	class CrossInviteJoinRoomRes :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_INVITE_JOIN_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief CROSS->WORLD 邀请玩家进入房间
	*/
	class CrossWorldInviteRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_WORLD_INVITE_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId & invitePlayer & playerId & (*packet);
		}

		CrossWorldInviteRoomReq(){ packet = Avalon::Packet::Create(); }
		CrossWorldInviteRoomReq(Avalon::Packet* pack){ packet = pack; }
		~CrossWorldInviteRoomReq(){ Avalon::Packet::Destroy(packet); }

		void Detach(){ packet = NULL; }

	public:
		//房间ID
		UInt32 roomId;
		//邀请人ID
		ObjID_t invitePlayer;
		//玩家ID
		ObjID_t playerId;
		//消息包
		Avalon::Packet* packet;

	};

	/**
	*@brief WORLD->CROSS 邀请玩家进入房间返回
	*/
	class CrossWorldInviteRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_WORLD_INVITE_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & roomId & playerId & invitePlayerId;
		}

	public:
		UInt32 result;

		//房间ID
		UInt32 roomId;
		
		ObjID_t playerId;

		//邀请人ID
		ObjID_t invitePlayerId;
	};

	/**
	*@brief WORLD->CROSS 请求转让房主
	*/
	class CrossChangeRoomOwnerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_CHANGE_ROOM_OWNER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & playerId;
		}
	public:
		RoomPlayerInfo info;

		UInt64 playerId;
	};

	/**
	*@brief CROSS->WORLD 请求转让房主返回
	*/
	class CrossChangeRoomOwnerRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_CHANGE_ROOM_OWNER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 被邀请请求
	*/
	class CrossBeInviteRoomReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_BE_INVITE_ROOM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & roomId & invitePlayerId & isAccept & slotGroup;
		}

	public:
		RoomPlayerInfo info;

		//房间号
		UInt32 roomId;

		//邀请人ID
		UInt64 invitePlayerId;

		//是否接受
		UInt8 isAccept;

		//队伍
		UInt8 slotGroup;
	};

	/**
	*@brief CROSS->WORLD 被邀请请求返回
	*/
	class CrossBeInviteRoomRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_BE_INVITE_ROOM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result & info;
		}

	public:
		ObjID_t playerId;
		UInt32 result;
		RoomInfo info;

	};

	/**
	*@brief WORLD->CROSS 请求关闭打开位置
	*/
	class CrossRoomCloseSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_CLOSE_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & slotGroup & index;
		}

	public:
		RoomPlayerInfo info;

		//队伍
		UInt8 slotGroup;
		//索引
		UInt8 index;
	};

	/**
	*@brief CROSS->WORLD 请求关闭打开位置返回
	*/
	class CrossRoomCloseSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_CLOSE_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求房间交换位置
	*/
	class CrossRoomSwapSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_SWAP_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & roomId & slotGroup & index;
		}

	public:
		RoomPlayerInfo info;

		//房间号
		UInt32 roomId;

		//队伍
		UInt8 slotGroup;

		//索引
		UInt8 index;

	};

	/**
	*@brief CROSS->WORLD 请求房间交换位置返回
	*/
	class CrossRoomSwapSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_SWAP_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & playerId;
		}

	public:
		UInt32 result;

		UInt64 playerId;
	};

	/**
	*@brief WORLD->CROSS 请求响应房间交换位置
	*/
	class CrossRoomResponseSwapSlotReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_RESPONSE_SWAP_SLOT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & isAccept & slotGroup & slotIndex;
		}

	public:
		RoomPlayerInfo info;
		UInt8 isAccept;
		UInt8 slotGroup;
		UInt8 slotIndex;
	};


	/**
	*@brief CROSS->WORLD 请求响应房间交换位置返回
	*/
	class CrossRoomResponseSwapSlotRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_RESPONSE_SWAP_SLOT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}

	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求开始匹配战斗
	*/
	class CrossRoomBattleStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}
	public:
		RoomPlayerInfo info;

	};

	/**
	*@brief CROSS->WORLD 请求开始匹配战斗返回
	*/
	class CrossRoomBattleStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求取消匹配战斗
	*/
	class CrossRoomBattleCancelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_CANCEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}
	public:
		RoomPlayerInfo info;

	};

	/**
	*@brief CROSS->WORLD 请求取消匹配战斗返回
	*/
	class CrossRoomBattleCancelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_CANCEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 请求战斗准备
	*/
	class CrossRoomBattleReadyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_READY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & slotStatus;
		}

	public:
		RoomPlayerInfo info;
		UInt8 slotStatus;
	};

	/**
	*@brief CROSS->WORLD 请求战斗准备返回
	*/
	class CrossRoomBattleReadyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_BATTLE_READY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief WORLD->CROSS 发送邀请信息
	*/
	class CrossRoomSendInviteLinkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_SEND_INVITE_LINK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & roomId & channel;
		}
	public:
		RoomPlayerInfo info;

		UInt32 roomId;

		UInt8 channel;
	};

	/**
	*@brief CROSS->WORLD 发送邀请信息返回
	*/
	class CrossRoomSendInviteLinkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_SEND_INVITE_LINK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result;
		}
	public:
		ObjID_t playerId;
		UInt32 result;
	};

	/**
	*@brief CROSS->WORLD 发送邀请信息返回
	*/
	class CrossRoomMatchInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_MATCH_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roomId  & playerId;
		}
	public:
		UInt32 roomId;
		ObjID_t playerId;
	};

	class CrossRoomMatchInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_ROOM_MATCH_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & roomId & accId & playerId & loseStreak & winStreak & matchScore & seasonLevel & seasonStar & seasonExp & battleCount;
		}
	public:
		UInt32 result;
		UInt32 roomId;
		UInt32 accId;
		ObjID_t playerId;
		UInt32 loseStreak;
		UInt32 winStreak;
		UInt32 matchScore;
		UInt32 seasonLevel;
		UInt32 seasonStar;
		UInt32 seasonExp;
		UInt32 battleCount;
	};

}

#endif //_CL_ROOM_PROTOCOL_H_
