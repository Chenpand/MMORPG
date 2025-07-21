#ifndef __SERVER_STATE_MESSAGE_BODY_H__
#define __SERVER_STATE_MESSAGE_BODY_H__

#include "gameplatform_types.h"
#include "public/server_message.h"


namespace KingNet { namespace Server {

/************************************************************************/
/* Hall Server VS State Server                                          */
/************************************************************************/

//SS_HALL_NOTIFY_INIT
//notify hall server --> state server
class CNotifyHallServerInit : public CMessageBody
{
public:
	CNotifyHallServerInit();
	~CNotifyHallServerInit();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iLocalIP;
	int16_t m_nLocalPort;
};

//SS_HALL_STATE_NOTIFY_KICKPLAYER
//notify state server --> hall server 
class CNotifyKickPlayerToHallServer : public CMessageBody
{
public:
	CNotifyKickPlayerToHallServer();
	~CNotifyKickPlayerToHallServer();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_iVictimUin;		//�����û�
	TAccount m_szVictimAccount;
	int32_t  m_iActorUin;		//�����û�
	TAccount m_szActorAccount;
	int16_t m_nReasonID;		//ȡֵgameplatfrom_message.h��result_id_kick_player_multilogin��
	TReason m_szReasonMessage;
};

#define MAX_REQUEST_PLAYER_COUNT	100
#define MAX_RESPONSE_PLAYER_COUNT   10

//SS_HALL_STATE_REQUEST_PLAYER_STATE
//request hall server --> state server
class CRequestGetPlayerStatusFromStateServer : public CMessageBody
{
public:
	CRequestGetPlayerStatusFromStateServer();
	~CRequestGetPlayerStatusFromStateServer();
	
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iMyUin;
	int16_t m_nCount;
	int32_t m_aiUin[MAX_REQUEST_PLAYER_COUNT];
};

//SS_GM_GET_ALL_PLAYER_STATE
class CRequestGetAllPlayerState : public CMessageBody
{
public:
	CRequestGetAllPlayerState();
	~CRequestGetAllPlayerState();
	virtual void dump();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);

	int8_t m_cFlag;
};

class CResponseGetAllPlayerState : public CMessageBody
{
public:
	CResponseGetAllPlayerState();
	~CResponseGetAllPlayerState();
	virtual void dump();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);

	int16_t m_nResultID;
};

#define MAX_GAME_PATH_LENGTH 256
typedef struct stUserStatus
{
	int16_t m_iGameID;
	int32_t m_nServerID;
	int32_t m_nRoomID;
	int32_t m_nTableID;
	int8_t m_iSeatID;
	int8_t m_iState;
	char m_szPath[MAX_GAME_PATH_LENGTH];
} stUserStatus;

int32_t encode_user_status(char** pszOut, stUserStatus& userStatus);
int32_t decode_user_status(char **pszOut, stUserStatus& userStatus);

//
#define  MAX_ROOM_SESSION_COUNT		8

enum enmState
{
	player_status_idle      = 0,	    //�ڷ����ڿ���
	player_status_sitting   = 1,	    //������Ϸ���ϣ���û�а���ʼ��Ϸ
	player_status_beready   = 2,	    //������Ϸ���ϣ������˿�ʼ��Ϸ
	player_status_ingame    = 3,	    //һ����Ϸ��
	player_status_onobserve = 4,        //��ĳ��Ϸ���Թ�
	player_status_offline   = 5,	    //���߲��ȴ�����

};

enum enmStateClass
{
	HALL_SERVER_STATUE_DATA_TYPE = 0,
	LOGIC_SERVER_STATUE_DATA_TYPE = 1,
	MAX_CLASS_COUNT
};


//ʹ��ÿһ��bit�Ƿ���1����ʾ״̬
struct stHallServerState
{
	int32_t m_iState;
};

struct stLogicServerState
{
	int8_t m_cRoomCount;
	stUserStatus m_astStatus[MAX_ROOM_SESSION_COUNT];
};

struct stStateData
{
	int8_t m_cClass;
	union
	{
		stHallServerState  hallState;
		stLogicServerState logicState;
	};
};

struct stPlayerStatusInfo
{
	int32_t  m_nUin;
	TAccount m_szAccount;
	int8_t   m_byClassCount;
	stStateData m_stStateData[MAX_CLASS_COUNT]; 
};

//ʹ��ÿһ��bit�Ƿ���1����ʾ״̬
struct stHallServerState_GM
{
	int32_t m_iState;
	int32_t m_nServerId;	//����HallServer Id
};

struct stStateData_GM
{
	int8_t m_cClass;
	union
	{
		stHallServerState_GM  hallState;
		stLogicServerState logicState;
	};
};

struct stPlayerStatusInfo_GM
{
	int32_t  m_nUin;
	TAccount m_szAccount;
	int8_t   m_byClassCount;
	stStateData_GM m_stStateData[MAX_CLASS_COUNT]; 
};

int32_t encode_state_data(char** pszOut, stStateData& data);
int32_t decode_state_data(char** pszOut, stStateData& data);

int32_t encode_playerstatus_info(char** pszOut, stPlayerStatusInfo& playerstatus);
int32_t decode_playerstatus_info(char** pszOut, stPlayerStatusInfo& playerstatus);


//SS_HALL_STATE_REQUEST_PLAYER_STATE
//response state server --> hall server
class CResponseGetPlayerStatusFromStateServer : public CMessageBody
{
public:
    CResponseGetPlayerStatusFromStateServer();
    ~CResponseGetPlayerStatusFromStateServer();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

    int16_t m_nResultID;
    int32_t m_iMyUin;

    int16_t m_nCount;
    stPlayerStatusInfo m_astPlayerStatus[MAX_RESPONSE_PLAYER_COUNT];
};

int32_t encode_state_data_For_GM(char** pszOut, stStateData_GM& data);
int32_t decode_state_data_For_GM(char** pszOut, stStateData_GM& data);

int32_t encode_playerstatus_info_For_GM(char** pszOut, stPlayerStatusInfo_GM& playerstatus);
int32_t decode_playerstatus_info_For_GM(char** pszOut, stPlayerStatusInfo_GM& playerstatus);

//response state server --> hall server
class CResponseGetPlayerStateForGMServer : public CMessageBody
{
public:
	CResponseGetPlayerStateForGMServer();
	~CResponseGetPlayerStateForGMServer();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;

	int16_t m_nCount;
	stPlayerStatusInfo_GM m_astPlayerStatus[MAX_RESPONSE_PLAYER_COUNT];
};

//SS_HALL_STATE_UPDATE_PLAYER_PROFILE
class CNotifyPlayerProfile : public CMessageBody
{
public:

	CNotifyPlayerProfile();
	~CNotifyPlayerProfile();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_iUin;
	TAccount m_szAccount;
	int8_t   m_cSex;	
};

//SS_HALL_STATE_REPORT_GAME_STATE
class CReportGameState : public CMessageBody
{
public: 
	CReportGameState();
	~CReportGameState();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_nUin;
	TAccount m_szAccount;
	int8_t   m_byClassCount;
	stStateData m_stStateData[MAX_CLASS_COUNT]; 
};

//�������꣬��С������to do
#define MAX_DEST_UIN_NUMS     1024

//�������꣬��Ҫ��server_router_head.h��ö��ֵ�滻��
#define  transfer_type_p2p			0
#define  transfer_type_multicast	1		
#define  transfer_type_broadcast	2

//hall server����state server��ת��Ϣ
class CRequestTransferMessage : public CMessageBody
{
public:
	CRequestTransferMessage();
	~CRequestTransferMessage();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_nSourceUin;
	int8_t  m_iTransferType;
	int16_t m_iDestUinCount;
	int32_t m_DestUins[MAX_DEST_UIN_NUMS];
	int16_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];
};

//state server��Ӧhall server��ת��Ϣ������
class CResponseTransferMessage : public CMessageBody
{
public:
	CResponseTransferMessage();
	~CResponseTransferMessage();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_iResultID; //�ɹ�����0��ʧ�ܷ���1
	int16_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];

	//ʧ��ʱ����������Ķ������ɹ�ʱ��Ҫ
	TReason m_szReasonMessage; //ʧ��ʱ������ʧ��ԭ��
};

//֪ͨĳ��hall server����ת��Ϣ
class CNotifyTransferMessage : public CMessageBody
{
public:
	CNotifyTransferMessage();
	~CNotifyTransferMessage();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_nSourceUin;
	int8_t  m_iTransferType;
	int32_t m_nDestUin;
	int16_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];
};

//֪ͨ�û��ķ�����ѣ��û�״̬�и���
class CNotifyStatusUpdateToFriend : public CMessageBody
{
public:
	CNotifyStatusUpdateToFriend();
	~CNotifyStatusUpdateToFriend();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	
	int32_t  m_nDstUin;
	TAccount m_szDstAccount;
	stPlayerStatusInfo m_stPlayerStatus;
};


//�㲥��ĳ�����û����ڵķ����ĳ������Ϸ�Ĺ�����Ϣ
class CNotifyBroadcastNoticeInfo : public CMessageBody
{
public:
	enum 
	{
		notice_info_type_by_uin		= 1,	//�����UIN���ڵķ���
		notice_info_type_by_game_id	= 2,	//�����game id����Ӧ�ķ���
		notice_info_type_all_hall	= 3,	//��������е�hall
	
		max_notice_uin_count	= 5,		//��๫���5��UIN���ڵķ���
		max_notice_game_id_count= 3,		//��๫���3����Ϸ�����з���
	};

public:
	CNotifyBroadcastNoticeInfo();
	~CNotifyBroadcastNoticeInfo();
	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int8_t m_cNoticeType;		//�������ͣ�����ö��
	union _U
	{
		struct _UINS 
		{
			int8_t  m_cCount;
			int32_t m_aiUIN[max_notice_uin_count];
		}m_stUIN;

		struct _IDS
		{
			int8_t  m_cCount;
			int16_t m_anGameID[max_notice_game_id_count];
		}m_stGame;

	}m_stDstInfo;				//����Ŀ��

	int16_t m_nBuffSize;
	char m_szNoticeBuff[max_transparent_data_size];
};

class CNotifyBroadcastNoticeToDstServer : public CMessageBody
{
public:
	enum 
	{
		notify_to_hall	= 1,		//�����HallServer�ϵ��������
		notify_to_logic	= 2,		//�����LogicServer�ϵ����з���
		notify_to_logic_room = 3,	//�����LogicServer�ϵ�ĳЩ����

		max_notify_room_id_count= 20,		//���ͬʱ�����20����
	};

public:
	CNotifyBroadcastNoticeToDstServer();
	~CNotifyBroadcastNoticeToDstServer();
	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int8_t m_cNotifyType;		//���浽������������
	union _U
	{
		struct _ROOMS 
		{
			int8_t  m_cCount;
			int32_t m_aiRoom[max_notify_room_id_count];
		}m_stRooms;
	}m_stDstInfo;				//����Ŀ��

	int16_t m_nBuffSize;
	char m_szNoticeBuff[max_transparent_data_size];
};

class CRequestRegisterToStateServer : public CMessageBody
{
public:
	CRequestRegisterToStateServer();
	~CRequestRegisterToStateServer();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int32_t m_iServerID;
	int8_t m_cServerType;
};

class CResponseRegisterToStateServer : public CMessageBody
{
public:
	CResponseRegisterToStateServer();
	~CResponseRegisterToStateServer();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int16_t m_iResultID; //�ɹ�����0��ʧ�ܷ���1
	int32_t m_iServerID;
	int8_t m_cServerType;
};


}}//namespace KingNet { namespace Server {



#endif //__SERVER_STATE_MESSAGE_BODY_H__

