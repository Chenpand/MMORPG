#ifndef _CL_ROOM_PROTOCOL_H_
#define _CL_ROOM_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRoomDefine.h"
#include <AvalonPacket.h>

namespace CLProtocol
{
	/**
	*@brief server->client ͬ��������Ϣ
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
	*@brief server->client ͬ���������Ϣ
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
	*@brief server->client ͬ�����������Ϣ
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
	*@brief server->client ֪ͨ��������� ������Ϣ
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
		//�����
		UInt32 roomId;
		//������
		std::string roomName;
		//��������
		UInt8 roomType;
		//������id
		UInt64 inviterId;
		//����������
		std::string inviterName;
		//������ְҵ
		UInt8 inviterOccu;
		//�����߾���
		UInt8 inviterAwaken;
		//�����ߵȼ�
		UInt16 inviterLevel;
		//��������
		UInt32 playerSize;
		//�����������
		UInt32 playerMaxSize;
		//��
		UInt8 group;
	};

	/**
	*@brief server->client ֪ͨ���߳����
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
		//�߳�����ԭ��
		UInt32 reason;
		//�߳������ID
		UInt64 kickPlayerId;
		//�߳����������
		std::string kickPlayerName;
		//�߳��ķ���ID
		UInt32 roomId;

	};

	/**
	*@brief server->client ֪ͨ������,��������ҵķ���
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
		//���ID
		UInt64 playerId;
		//�Ƿ����
		UInt8 isAccept;
	};

	/**
	*@brief server->client ֪ͨ��ҽ���λ����Ϣ
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
		//���ID
		UInt64 playerId;

		//�����
		std::string playerName;

		//��ҵȼ�
		UInt16 playerLevel;

		//���ְҵ
		UInt8 playerOccu;

		//��Ҿ���
		UInt8 playerAwaken;

		//����
		UInt8 slotGroup;

		//λ��
		UInt8 slotIndex;
	};

	/**
	*@brief server->client ֪ͨ��ҽ���λ�÷�����Ϣ
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
		//���ؽ��
		UInt8 result;
		//��Ӧ��ID
		UInt64 playerId;
		//��Ӧ������
		std::string playerName;
	};

	/**
	*@brief server->client ֪ͨ���������Ϣ
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

		//����id
		UInt32 roomId;

		//����
		std::string password;
	};

	/**
	*@brief server->client ͬ��ս��������Ϣ
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
		// PK���ͣ���Ӧö��(PkType)
		UInt8		pkType;
		UInt64		raceId;
		UInt8		result;
		UInt32		oldPkValue;
		UInt32		newPkValue;
		UInt32		oldMatchScore;
		UInt32		newMatchScore;
		// ��ʼ����������
		UInt32		oldPkCoin;
		// ս����õľ�����
		UInt32		addPkCoinFromRace;
		// ����ս����õ�ȫ��������
		UInt32		totalPkCoinFromRace;
		// �Ƿ���PVP��ڼ�
		UInt8		isInPvPActivity;
		// ������õľ�����
		UInt32		addPkCoinFromActivity;
		// ���ջ��õ�ȫ��������
		UInt32		totalPkCoinFromActivity;
		// ԭ��λ
		UInt32		oldSeasonLevel;
		// �ֶ�λ
		UInt32		newSeasonLevel;
		//ԭ��
		UInt32		oldSeasonStar;
		//����
		UInt32		newSeasonStar;
		//ԭ����
		UInt32		oldSeasonExp;
		//�־���
		UInt32		newSeasonExp;
		//�ı�ľ���
		Int32		changeSeasonExp;
		//������Ϣ
		std::vector<RoomSlotBattleEndInfo> slotInfos;
		//�������
		UInt32		getHonor;
	};

	/**
	*@brief World->Sence ֪ͨSceneƥ�俪ʼ
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
	*@brief World->Sence ֪ͨScene������Ϣ
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
	*@brief client->server ���󷿼��б�
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

		//��ҵȼ�
		UInt16 limitPlayerLevel;

		//��Ҷ�λ
		UInt32 limitPlayerSeasonLevel;

		//����״̬
		UInt8 roomStatus;

		//��������
		UInt8 roomType;

		//�Ƿ�������
		UInt8 isPassword;

		//��ʼλ��
		UInt32 startIndex;

		//����
		UInt32 count;
	};

	/**
	*@brief server->client ���󷿼��б���
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
	*@brief client->server ���󴴽�����·���
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
		//�����
		UInt32 roomId;
		//��������
		UInt8 roomType;
		//������
		std::string name;
		//��������
		std::string password;
		//�Ƿ����÷������Ƶȼ�
		UInt8 isLimitPlayerLevel;
		//�������Ƶȼ�
		UInt16 limitPlayerLevel;
		//�Ƿ����÷������ƶ�λ
		UInt8 isLimitPlayerSeasonLevel;
		//�������ƶ�λ
		UInt32 limitPlayerSeasonLevel;

	};

	/**
	*@brief server->client ���󴴽�����·��䷵��
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
		//������Ϣ
		RoomInfo info;
	};

	/**
	*@brief client->server ������뷿��
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
		//��������
		UInt8 roomType;
		//����
		std::string password;
		//����ʱ��
		UInt32 createTime;
	};

	/**
	*@brief server->client ������뷿�䷵��
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
		//������Ϣ
		RoomInfo info;
	};

	/**
	*@brief client->server �����˳�����
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
	*@brief server->client �����˳����䷵��
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
	*@brief client->server �����߳�����
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
		//���߳������id
		UInt64 playerId;
	};

	/**
	*@brief server->client �����߳����䷵��
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
	*@brief client->server �����ɢ����
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
	*@brief server->client �����ɢ���䷵��
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
	*@brief client->server ����������뷿��
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
		//����������id
		UInt64 playerId;
	};

	/**
	*@brief server->client ����������뷿�䷵��
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
	*@brief client->server ����ת�÷���
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
	*@brief server->client ����ת�÷�������
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
	*@brief client->server ����������
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
		//�����
		UInt32 roomId;

		//������ID
		UInt64 invitePlayerId;

		//�Ƿ����
		UInt8 isAccept;

		//����
		UInt8 slotGroup;
	};

	/**
	*@brief server->client ���������󷵻�
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
	*@brief client->server ����رմ�λ��
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
		//����
		UInt8 slotGroup;
		//����
		UInt8 index;
	};
	
	/**
	*@brief server->client ����رմ�λ�÷���
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
	*@brief client->server ���󷿼佻��λ��
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
		//�����
		UInt32 roomId;

		//����
		UInt8 slotGroup;

		//����
		UInt8 index;

	};

	/**
	*@brief server->client ���󷿼佻��λ�÷���
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
	*@brief client->server ������Ӧ���佻��λ��
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
	*@brief server->client ������Ӧ���佻��λ�÷���
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
	*@brief client->server ����ʼƥ��ս��
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
	*@brief server->client ����ʼƥ��ս������
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
	*@brief client->server ����ȡ��ƥ��ս��
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
	*@brief server->client ����ȡ��ƥ��ս������
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
	*@brief client->server ����ս��׼��
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
	*@brief server->client ����ս��׼������
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
	*@brief client->server ����������Ϣ
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
	*@brief server->client ����������Ϣ����
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
	*@brief CROSS->WORLD ͬ���������
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
	*@brief CROSS->WORLD ͬ��������־
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
	*@brief CROSS->WORLD ����notify��Ϣ
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
	*@brief WORLD->CROSS ֪ͨ����ս����ʼ
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
	*@brief WORLD->CROSS ֪ͨ�������
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
	*@brief WORLD->CROSS ֪ͨ�������
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
	*@brief WORLD->CROSS ֪ͨ��Ҷ���
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
	*@brief WORLD->CROSS ֪ͨ��Ҷ�������
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
	*@brief WORLD->CROSS ֪ͨ��Ҹı䳡��
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
	*@brief WORLD->CROSS ���󷿼��б�
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

		//���ID
		ObjID_t playerId;

		//��ҵȼ�
		UInt16 limitPlayerLevel;

		//��Ҷ�λ
		UInt32 limitPlayerSeasonLevel;

		//����״̬
		UInt8 roomStatus;

		//��������
		UInt8 roomType;

		//�Ƿ�������
		UInt8 isPassword;

		//��ʼλ��
		UInt32 startIndex;

		//����
		UInt32 count;
	};

	/**
	*@brief CROSS->WORLD ���󷿼��б���
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
	*@brief WORLD->CROSS ���󴴽�����·���
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
		//�����
		UInt32 roomId;
		//��������
		UInt8 roomType;
		//������
		std::string name;
		//��������
		std::string password;
		//�Ƿ����÷������Ƶȼ�
		UInt8 isLimitPlayerLevel;
		//�������Ƶȼ�
		UInt16 limitPlayerLevel;
		//�Ƿ����÷������ƶ�λ
		UInt8 isLimitPlayerSeasonLevel;
		//�������ƶ�λ
		UInt32 limitPlayerSeasonLevel;
		//��������Ľ�ɫ��Ϣ
		RoomPlayerInfo playerInfo;
	};

	/**
	*@brief CROSS->WORLD ���󴴽�����·��䷵��
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
		//����ID
		ObjID_t ownerId;
		//�Ƿ񴴽�����
		UInt8 isCreate;
		//������Ϣ
		RoomInfo info;
	};

	/**
	*@brief WORLD->CROSS ������뷿��
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
		//��������
		UInt8 roomType;
		//����
		std::string password;
		//����ʱ��
		UInt32 createTime;
		//��ɫ��Ϣ
		RoomPlayerInfo playerInfo;
	};

	/**
	*@brief CROSS->WORLD ������뷿�䷵��
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
		//����ID
		ObjID_t ownerId;
		//������Ϣ
		RoomInfo info;
	};

	/**
	*@brief WORLD->CROSS �����˳�����
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
	*@brief CROSS->WORLD �����˳����䷵��
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
	*@brief WORLD->CROSS �����߳�����
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

		//���߳������id
		UInt64 playerId;
	};

	/**
	*@brief CROSS->WORLD �����߳����䷵��
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
	*@brief WORLD->CROSS �����ɢ����
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
	*@brief CROSS->WORLD �����ɢ���䷵��
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
	*@brief WORLD->CROSS ����������뷿��
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

		//����������id
		UInt64 playerId;
	};

	/**
	*@brief CROSS->WORLD ����������뷿�䷵��
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
	*@brief CROSS->WORLD ������ҽ��뷿��
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
		//����ID
		UInt32 roomId;
		//������ID
		ObjID_t invitePlayer;
		//���ID
		ObjID_t playerId;
		//��Ϣ��
		Avalon::Packet* packet;

	};

	/**
	*@brief WORLD->CROSS ������ҽ��뷿�䷵��
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

		//����ID
		UInt32 roomId;
		
		ObjID_t playerId;

		//������ID
		ObjID_t invitePlayerId;
	};

	/**
	*@brief WORLD->CROSS ����ת�÷���
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
	*@brief CROSS->WORLD ����ת�÷�������
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
	*@brief WORLD->CROSS ����������
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

		//�����
		UInt32 roomId;

		//������ID
		UInt64 invitePlayerId;

		//�Ƿ����
		UInt8 isAccept;

		//����
		UInt8 slotGroup;
	};

	/**
	*@brief CROSS->WORLD ���������󷵻�
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
	*@brief WORLD->CROSS ����رմ�λ��
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

		//����
		UInt8 slotGroup;
		//����
		UInt8 index;
	};

	/**
	*@brief CROSS->WORLD ����رմ�λ�÷���
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
	*@brief WORLD->CROSS ���󷿼佻��λ��
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

		//�����
		UInt32 roomId;

		//����
		UInt8 slotGroup;

		//����
		UInt8 index;

	};

	/**
	*@brief CROSS->WORLD ���󷿼佻��λ�÷���
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
	*@brief WORLD->CROSS ������Ӧ���佻��λ��
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
	*@brief CROSS->WORLD ������Ӧ���佻��λ�÷���
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
	*@brief WORLD->CROSS ����ʼƥ��ս��
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
	*@brief CROSS->WORLD ����ʼƥ��ս������
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
	*@brief WORLD->CROSS ����ȡ��ƥ��ս��
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
	*@brief CROSS->WORLD ����ȡ��ƥ��ս������
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
	*@brief WORLD->CROSS ����ս��׼��
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
	*@brief CROSS->WORLD ����ս��׼������
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
	*@brief WORLD->CROSS ����������Ϣ
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
	*@brief CROSS->WORLD ����������Ϣ����
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
	*@brief CROSS->WORLD ����������Ϣ����
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
