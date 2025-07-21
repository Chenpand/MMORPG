#ifndef __CL_ROOM_DEFINE_H__
#define __CL_ROOM_DEFINE_H__

#include <CLDefine.h>
#include <CLGameDefine.h>

//����������(���)
const static UInt32 ROOM_NAME_MAX_WIDTH = 20;

//��������涨����
const static UInt32 ROOM_PASSWORD_SIZE = 4;

//һ�������������
const static UInt32 GROUP_SLOT_SIZE = 3;

//�����Զ����뷿����
const static UInt32 ROOM_QUIT_TIME_INTERVAL = 10;

//��������ʱ����
const static UInt32 ROOM_OWNER_OFFLINE_INTERVAL = 60;

//�������ϵ��
const static UInt32 ROOM_MAX_SEASON_RATIO = 80;
const static UInt32 ROOM_MIDDLE_SEASON_RATIO = 15;
const static UInt32 ROOM_MIN_SEASON_RATIO = 5;
const static UInt32 ROOM_SUM_RATIO = 100;

enum RoomType
{
	ROOM_TYPE_INVALID = 0,

	//3V3����ģʽ
	ROOM_TYPE_THREE_FREE = 1,

	//3V3ƥ��ģʽ
	ROOM_TYPE_THREE_MATCH = 2,

	//3V3����������ģʽ
	ROOM_TYPE_THREE_SCORE_WAR = 3,

	//3V3�Ҷ�ģʽ
	ROOM_TYPE_THREE_TUMBLE = 4,  

	ROOM_TYPE_MAX,
};

enum RoomStatus
{
	//��Ч
	ROOM_STATUS_INVALID = 0,

	//��״̬
	ROOM_STATUS_OPEN = 1,

	//ս��׼��
	ROOM_STATUS_READY = 2,

	//ƥ��׶�
	ROOM_STATUS_MATCH = 3,

	//ս��״̬
	ROOM_STATUS_BATTLE = 4,

	ROOM_STATUS_NUM,
};

enum RoomSlotStatus
{
	ROOM_SLOT_STATUS_INVALID = 0,

	//��
	ROOM_SLOT_STATUS_OPEN = 1,
	//�ر�
	ROOM_SLOT_STATUS_CLOSE = 2,
	//�ȴ�
	ROOM_SLOT_STATUS_WAIT = 3,
	//����
	ROOM_SLOT_STATUS_OFFLINE = 4,

	ROOM_SLOT_STATUS_NUM,
};


enum RoomSlotReadyStatus
{
	ROOM_SLOT_READY_STATUS_INVALID = 0,

	//����
	ROOM_SLOT_READY_STATUS_ACCEPT,

	//�ܾ�
	ROOM_SLOT_READY_STATUS_REFUSE,

	//��ʱ
	ROOM_SLOT_READY_STATUS_TIMEOUT,

	ROOM_SLOT_READY_STATUS_NUM,
};

enum RoomMatchInfoStatus
{
	RMIS_INVALID = 0,

	//������
	RMIS_REQUEST = 1,
	//����
	RMIS_RESULT = 2,

	RMIS_NUM,
};

enum RoomSlotGroup
{
	ROOM_SLOT_GROUP_INVALID = 0,

	//���
	ROOM_SLOT_GROUP_RED = 1,

	//����
	ROOM_SLOT_GROUP_BLUE = 2,

	ROOM_SLOT_GROUP_NUM,
};

enum RoomQuitReason
{
	ROOM_QUIT_REASON_INVALID = 0,

	ROOM_QUIT_SELF = 1,				//�Լ��˳�

	ROOM_QUIT_OWNER_KICK_OUT = 2,	//�������߳�

	ROOM_QUIT_DISMISS = 3,			//��ɢ

	ROOM_QUIT_NUM = 4,

};

enum RoomEvent
{
	ROOM_EVENT_JOIN = 1,
	ROOM_EVENT_QUIT = 2,
};

enum RoomSwapResult
{
	ROOM_SWAP_RESULT_INVALID = 0,
	ROOM_SWAP_RESULT_ACCEPT = 1,
	ROOM_SWAP_RESULT_REFUSE = 2,
	ROOM_SWAP_RESULT_TIMEOUT = 3,
	ROOM_SWAP_RESULT_CANCEL = 4,

};

enum LogRoomOperationType
{
	ROOM_OPERATION_TYPE_INVALID = 0,
	ROOM_OPERATION_TYPE_CREATE,
	ROOM_OPERATION_TYPE_DELETE,
	ROOM_OPERATION_TYPE_JOIN,
	ROOM_OPERATION_TYPE_QUIT,
	ROOM_OPERATION_TYPE_INVITE,
	ROOM_OPERATION_TYPE_SWAP_SLOT,
	ROOM_OPERATION_TYPE_MATCH_START,
	ROOM_OPERATION_TYPE_BATTLE_START,
	ROOM_OPERATION_TYPE_BATTLE_END,
};

inline bool IsRoomOpen(RoomStatus status)
{
	return status == RoomStatus::ROOM_STATUS_OPEN;
}

inline bool IsSlotExistPlayer(RoomSlotStatus status)
{
	return status == ROOM_SLOT_STATUS_WAIT;
}

inline bool IsSlotOfflinePlayer(RoomSlotStatus status)
{
	return status == ROOM_SLOT_STATUS_OFFLINE;
}

inline bool IsSlotNotPlayer(RoomSlotStatus status)
{
	return status == ROOM_SLOT_STATUS_CLOSE || status == ROOM_SLOT_STATUS_OPEN;
}

inline bool IsRoomReadyStatus(RoomStatus status)
{
	return status == ROOM_STATUS_READY;
}

inline bool IsSlotReadyInvalid(RoomSlotReadyStatus status)
{
	return status == RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_INVALID;
}

inline bool IsSlotReadyAccept(RoomSlotReadyStatus status)
{
	return status == RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_ACCEPT;
}

inline bool IsSlotReadyRefuse(RoomSlotReadyStatus status)
{
	return status == RoomSlotReadyStatus::ROOM_SLOT_READY_STATUS_REFUSE;
}

inline bool IsRoomBattleMatch(RoomSlotStatus status)
{
	return IsSlotExistPlayer(status) || IsSlotNotPlayer(status);
}

inline bool IsRoomMatchResult(RoomMatchInfoStatus status)
{
	return status == RoomMatchInfoStatus::RMIS_RESULT;
}

inline RoomSlotGroup GetRoomSlotGroup(UInt32 seat)
{
	return (RoomSlotGroup)(seat / GROUP_SLOT_SIZE + 1);
}

inline UInt32 GetRoomSumExpByExp(UInt32 maxExp, UInt32 middleExp, UInt32 minExp)
{
	UInt32 sumExp = (UInt32)(maxExp * ((ROOM_MAX_SEASON_RATIO * 1.00) / (ROOM_SUM_RATIO * 1.00)));
	sumExp += (UInt32)(middleExp * ((ROOM_MIDDLE_SEASON_RATIO * 1.00) / (ROOM_SUM_RATIO * 1.00)));
	sumExp += (UInt32)(minExp * ((ROOM_MIN_SEASON_RATIO * 1.00) / (ROOM_SUM_RATIO * 1.00)));
	return sumExp;
}

inline UInt32 GetRoomSumExp(std::vector<UInt32> exps)
{
	UInt32 vecExpLength = exps.size();
	if (vecExpLength <= 0) return 0;
	std::sort(exps.begin(), exps.end());
	if (vecExpLength >= 3)
	{
		return GetRoomSumExpByExp(exps[vecExpLength - 1], exps[vecExpLength - 2], exps[vecExpLength - 3]);
	}
	else if (vecExpLength >= 2)
	{
		return GetRoomSumExpByExp(exps[vecExpLength - 1], exps[vecExpLength - 2], exps[vecExpLength - 2]);
	}
	else if (vecExpLength >= 1)
	{
		return exps[vecExpLength - 1];
	}
	return 0;
}

struct RoomSlotInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & group & index & playerId & playerName & playerLevel & playerSeasonLevel & playerVipLevel
			& playerOccu & playerAwake & avatar & status & readyStatus & playerLabelInfo;
	}

	UInt8 group;
	UInt8 index;
	UInt32 accId;
	UInt64 playerId;
	std::string playerName;
	UInt16 playerLevel;
	UInt32 playerSeasonLevel;
	UInt8 playerVipLevel;
	UInt8 playerOccu;
	UInt8 playerAwake;
	PlayerAvatar avatar;
	UInt8 status;
	UInt8 readyStatus;
	UInt32 winStreak;
	UInt32 loseStreak;
	UInt32 matchScore;
	UInt32 battleCount;
	PlayerLabelInfo playerLabelInfo;
};

struct RoomSimpleInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & roomStatus & roomType & isLimitPlayerLevel & limitPlayerLevel & isLimitPlayerSeasonLevel & limitPlayerSeasonLevel
			& playerSize & playerMaxSize & isPassword & ownerId & ownerOccu & ownerSeasonLevel;
	}

	//����id
	UInt32 id;
	//������
	std::string name;
	//����״̬
	UInt8 roomStatus;
	//��������
	UInt8 roomType;
	//�Ƿ����÷������Ƶȼ�
	UInt8 isLimitPlayerLevel;
	//�������Ƶȼ�
	UInt16 limitPlayerLevel;
	//�Ƿ����÷������ƶ�λ
	UInt8 isLimitPlayerSeasonLevel;
	//�������ƶ�λ
	UInt32 limitPlayerSeasonLevel;
	//��������
	UInt8 playerSize;
	//��󷿼�����
	UInt8 playerMaxSize;
	//�Ƿ���Ҫ����
	UInt8 isPassword;
	//����ID
	UInt64 ownerId;
	//����ְҵ
	UInt8 ownerOccu;
	//������λ
	UInt32 ownerSeasonLevel;
};

struct RoomInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roomSimpleInfo & roomSlotInfos;
	}

	RoomSimpleInfo roomSimpleInfo;
	//�����λ
	std::vector<RoomSlotInfo> roomSlotInfos;
};

struct RoomListInfo
{
	RoomListInfo()
	{
		startIndex = 0;
		total = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & startIndex & total & rooms;
	}

	UInt32 startIndex;
	UInt32 total;
	std::vector<RoomSimpleInfo> rooms;
};

struct RoomSlotReadyInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & slotGroup & slotIndex & readyStatus;
	}

	UInt8 slotGroup;
	UInt8 slotIndex;
	UInt8 readyStatus;
};

struct RoomSlotBattleEndInfo
{
	RoomSlotBattleEndInfo()
	{
		getHonor = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & resultFlag & roomId & roomType & roleId & seat & seasonLevel & seasonStar 
			& seasonExp & scoreWarBaseScore & scoreWarContriScore & getHonor;
	}

	UInt8 resultFlag;
	UInt32 roomId;
	UInt8 roomType;
	UInt64 roleId;
	UInt8 seat;
	UInt32 seasonLevel;
	UInt32 seasonStar;
	UInt32 seasonExp;
	UInt32 scoreWarBaseScore;
	UInt32 scoreWarContriScore;
	UInt32 getHonor;
};

struct RoomPlayerInfo
{
	RoomPlayerInfo() : connId(0), accId(0), id(0), level(0), seasonLevel(0), occu(0), awaken(0), vipLevel(0), roomId(0), quitRoomId(0), quitRoomTime(0), sceneId(0), headFrame(0)
	{
		name = "";
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & connId & accId & id & name & level & seasonLevel & seasonExp & seasonStar & occu & awaken & vipLevel & avatar
			& roomId & quitRoomId & quitRoomTime & sceneId & loseStreak & winStreak & matchScore & battleCount & headFrame;
	}

	UInt32 connId;
	UInt32 accId;
	ObjID_t id;
	std::string name;
	UInt16 level;
	UInt32 seasonLevel;
	UInt32 seasonExp;
	UInt32 seasonStar;
	UInt8 occu;
	UInt8 awaken;
	UInt8 vipLevel;
	PlayerAvatar avatar;
	UInt32 roomId;
	UInt32 quitRoomId;
	UInt32 quitRoomTime;
	UInt32 sceneId;
	UInt32 loseStreak;
	UInt32 winStreak;
	UInt32 matchScore;
	UInt32 battleCount;
	UInt32 headFrame;
};

#endif //__CL_ROOM_DEFINE_H_