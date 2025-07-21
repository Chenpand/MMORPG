#ifndef __SERVER_GAMEDB_MESSAGE_BODY_H__
#define __SERVER_GAMEDB_MESSAGE_BODY_H__
#include "server_message.h"


namespace KingNet { namespace Server { 


	enum enmVipType		//��������
	{
		vip_type_invalid		= 0,
		vip_type_normal  		= 1,//��ͨvip
		vip_type_mobile			= 2,//�ֻ�(����)vip
		vip_type_honorary		= 4,//����vip
	};
	
	enum enmLockType
	{
		lock_type_unlock            = 0,		//����
		lock_type_exclusive_lock    = 1,	    //���ӻ�����
		lock_type_force_unlock      = 2,        //ǿ�ƽ���(������GM����ǿ�ƽ���).
	};
	
	enum enmLockStrategy
	{
			lock_type_strategy_try = 1, //��������.
			lock_type_strategy_must =2,//������ȫֵ...
	};

	enum enmLockMoney
	{
		lock_money_all = -1,	//��ȫ���Ľ��
	};

	enum enmIconType
	{
		icon_type_default           = 0,		//Ĭ��ͷ��, δ�趨ͷ��
		icon_type_male_default      = 1,		//Ĭ������ͷ��
		icon_type_female_default    = 2,		//Ĭ��Ů��ͷ��

		icon_type_zone_photo        = 126,		//51�ռ�������
		icon_type_game_photo        = 127,		//��Ϸ������
	};

	enum 
	{
		max_game_other_data_count = 8,
		max_game_data_count = 256,
		max_game_ext_int_count =128,
		max_game_ext_data_size = 1024, //������չ�ڴ��С.
		max_web_order_id_size =30,//��վ��ֵ[�ض�]����ˮ�Ŵ�С
		max_web_qun_data_size = 1024,//��վȺ����Ϣ���С
		max_game_count_in_server = 10,//��server�����ɷ�10����Ϸ.
		
	};

	

	typedef enum
	{
		PCD_OP_Begin				= 0,
		PCD_OP_RejectAnyChat		= PCD_OP_Begin,     //�ܾ������κ�˽�� 
		PCD_OP_RejectAnyGameInvite	= 1,				//�������κ���Ϸ���� 
		PCD_OP_AddFriendLimit		= 2,				//����������
		PCD_OP_VedioInvitationLimit	= 3,				//��Ƶ���������
		PCD_OP_End = 31
	}PCD_OthersPosi;
	
	

	//	
	typedef enum
	{
		PCD_Type_Begin =0 ,        
		PCD_Charming        = PCD_Type_Begin,    //����ֵ
		PCD_Achievement     = 1,    //�ɾ�ֵ
		PCD_PunishMethod    = 2,    //�ͷ�
		PCD_OtherData = 3,         //�����ֶ�... //���ֶ�����
		PCD_LoginCount =4,         //ƽ̨��¼����,
		PCD_LastLoginTime =5,      //��һ�εĵ�¼ʱ��
		PCD_LastLoginIP =6,        //��һ�εĵ�¼IP
		PCD_WebQunData =7,         //Ⱥ����Ϣ
		PCD_VipData = 8,		   //VIP��Ϣ
		//add by luozhen,20090828
		PCD_IdCard =9,             //���֤��Ϣ
		PCD_Sex = 10,              //�Ա�
		PCD_Birthday = 11,         //��������
		
		PCD_Type_End,   
		PCD_MAX_DATA_NUMBER = 50   
	}PlayCommandDataType;


	
	enum enUpdateModes
	{
		update_mode_none	= 0,			
		update_mode_delta	= 1,
		update_mode_set		= 2,
		update_mode_bit_and	= 3,
		update_mode_bit_or	= 4,
		update_mode_bit_xor	= 5,
		update_mode_bit_non_and = 6, //�ȷ�����
		update_mode_bit_non_or = 7, //�ȷ��ٻ�
	};

	enum{
		TimeType_Relative =1 ,
		TimeType_Absolute ,
		TimeType_Subscribe,  //��������(�ֻ�,����)
		TimeType_UnSubscribe, //ȡ������(�ֻ�,����)
	};

	enum{
		max_request_register_service_number = 10,
	};

	typedef struct
	{
		int8_t m_nOtherDataIndex;
		enUpdateModes m_nUpdateMode;
		int32_t   m_iOtherDataValue;
	} TOtherDataUpdate;

	
	

	
	
	typedef struct _Int32DataChange
	{
		int32_t m_iDeltaValue;
		int32_t m_iCurrentValue;
	}stInt32DataChange;
	
	typedef struct _Int64DataChange
	{
		int64_t m_lDeltaValue;
		int64_t m_lCurrentValue;
	}stInt64DataChange;
	
	
	typedef struct _GameResultChange
	{
		int32_t m_iDeltaGamePoint;
		int32_t m_iDeltaWinRound;
		int32_t m_iDeltaLossRound;
		int32_t m_iDeltaDrawRound;
		int32_t m_iDeltaEscapeRound;
		int32_t m_iDeltaCostTime;
		
		int32_t m_iGamePoint;
		int32_t m_iWinRound;
		int32_t m_iLossRound;
		int32_t m_iDrawRound;
		int32_t m_iEscapeRound;
		int32_t m_iCostTime;

	}stGameResultChange;
	
	typedef enum
	{
		QPD_Type_Begin = 0,
		QPD_Type_Charming,
		QPD_Type_Achievement,
		QPD_Type_Money,
		QPD_Type_HappyBean,
		QPD_Type_GameResultChange,
		QPD_MAX_DATA_NUMBER = 10   
	}QunPlayerDataType;
	
	typedef struct _QunPlayerData
	{
		int8_t m_iDataType; //QunPlayerDataType
		union _U
		{
			stInt32DataChange m_stCharming;	
			stInt32DataChange m_stAchievement;	
			stInt32DataChange m_stMoney;
			stInt64DataChange m_stHappyBean;
			stGameResultChange m_stGameResultChange;
		}m_DataValue;	
	}stQunPlayerData;
	
	//�ͷ�����....
	enum enmPunishMethod
	{
		punish_method_none = 0,        //��
		punish_method_forbit_chat = 1, //��ֹ����

		punish_method_freeze_account = 5,//�����ʺ�
	};

	struct stWebQunData
	{
		int16_t m_nDataSize ;
		char m_szWebQunInfo[max_web_qun_data_size];
	};

	struct stVipData
	{
		int32_t m_iGameVipExpireTime;	   //��ϷVIP�Ĺ���ʱ��(���û�п�ͨ����ʱ��Ϊ0)
		int32_t m_iGameVIPScore;	   //��ϷVIP����
		int32_t m_iGameVIPLevel;	   //��ϷVIP�ȼ�
		int32_t m_iNextUpdateVIPLevelTime; //�´�������ʱ��
		char m_szVIPTips[max_system_message_length]; //������������ϵ���ʾ��Ϣ
		int32_t m_iGameVIPType ;           //��ϷVIP����,����ֵ����μ�  enmVIPType
	};

	struct stBirthday
	{
		int16_t m_iYear;
		int16_t m_iMonth;
		int32_t m_iDay;
	};
	//��һ�����Ϣ==>DB Table UserInfo
	struct stPlayerCommonInfo 
	{
		int32_t m_iCoin;
		int32_t m_iLoginCount;
		int32_t m_iOfflineCount;
		int32_t m_iFirstLoginDatetime;		//��һ�ε�½ƽ̨��ʱ��. added by aprilliu, 2009-1-1

		int32_t m_iCharming;
		int32_t m_iAchievement;
		int64_t m_lHappyBean;	//added by aprilliu, 2009-1-1

		//
		int32_t m_iBlackLevel;
		char m_szValidDate[max_date_string_length];
		int32_t m_iPunishMethod;		
		char m_szLastLoginDate[max_date_string_length];
		int32_t m_iLastLoginIP;		
		char m_szDescString[max_describe_string_length];		//���������������
		char m_szLastConsumeDate[max_date_string_length];
		

		int32_t m_iMiscFlag; //����ֵ��Ķ���,����.
		//ÿ�����ͻ��ֶ�enmHappyBeanPresentMode_daily_present
		int32_t m_iHappyBean_Daily_PresentedCount;//���������͵Ĵ���
		int32_t m_iHappyBean_Daily_LastPresentedTime; //���һ�����͵�ʱ��


		stWebQunData m_stWebQunData;
		//int16_t m_nOtherCount;
		int32_t m_aiOthers[player_data_other_count];			//Ԥ����Ϣ, �±����

		stVipData	m_stVipData;
		
		//add by luozhen,20090828
		TIDCard m_szIDCard;	
		int8_t m_nSex; 
		//int32_t m_nBirthday; 
		stBirthday m_stBirThday;
	};


	typedef struct _PlayerCommonData
	{
		int8_t  m_iDataType; //PlayCommandDataType
		union _U
		{
			int32_t m_iCharming;
			int32_t m_iAchievement;
			struct stPunishData{
				int8_t  m_iBlackLevel;
				char m_szValidDate[max_date_string_length];
				int32_t m_iPunishMethod;
				char m_szDescString[max_operate_description_length];
			}m_stPunish;
			TOtherDataUpdate m_stOtherData;
			int32_t m_iLoginCount;
			int32_t m_iLastLoginTime;
			int32_t m_iLastLoginIP;
			stWebQunData m_stWebQunData;
			stVipData	m_stVipData;
			//add by luozhen,20090828
			TIDCard m_szIDCard;	
			int8_t m_nSex; 
			//int32_t m_nBirthday;
			stBirthday m_stBirThday;
		}m_DataValue;
	}stPlayerCommonData;


	//�����Ϸ����===> DB Table:UserGameInfo�󲿷�����
	struct stPlayerGameConfig
	{
		int32_t m_iMinPoint;					//��С����
		int32_t m_iMaxPointGap;					//�����ֲ��

		uint8_t m_byMaxOfflineRate;				//��������
		uint8_t m_byIPBit;					//
		uint32_t m_uiBitMap;					//


		uint8_t m_byOtherSize;					//����
		int32_t m_aiOthers[max_game_other_data_count];

	};

	typedef struct 
	{
		int32_t m_aryExtInt[max_game_ext_int_count];
		int16_t m_nExtDataSize;
		char m_szExtData[max_game_ext_data_size];
	} TDBExtGameInfo;


	//�����Ϸ����===> DB Table:UserGameInfo
	typedef struct stDBGameData
	{
		int16_t m_nGameID;				//��Ϸ���ͱ��

		int8_t m_byLevel;				//���ֵȼ�(������ֶ�,����5.1���õ�...)

		int32_t m_iPoint;				//����

		int32_t m_iWinRound;			//Ӯ
		int32_t m_iLoseRound;			//��
		int32_t m_iDrawRound;			//ƽ��
		int32_t m_iEscapeRound;			//���ܾ���

		int32_t m_iOrgID;				//��������
		int16_t m_nPosition;			//����λ��

		uint32_t m_uiTotalSecs;				//��Ϸʱ��
		char m_szLastDate[max_date_string_length]; //������ʱ��.
		stPlayerGameConfig m_stGameConfig;		//��Ϸ����
		TDBExtGameInfo m_stExtGameInfo; //��Ϸ��չ����...
	}TDBGameData;



	enum enmPlayerCommonInfoMiscFlag
	{
		enmPlayerCommonInfoMiscFlag_FirstEnterPlatformPresentedBean=  (int32_t)0x00000001,  //��һ�ν�����Ϸƽ̨���ͻ��ֶ�
		enmPlayerCommonInfoMiscFlag_FirstEnterHappyGamePresentedBean= (int32_t)0x00000002, //ÿһ�ν��뻶����Ϸ���ͻ��ֶ�
	};

	enum
	{
		max_service_type_count = 32,//512,

		max_ss_shared_money_entry_count = 16, //�� logic server�е�max_shared_entry_count����һ��
	};


	struct stService 
	{
		int32_t m_iServiceID;
		int32_t m_iExpireTime;
	};

	struct stServiceData 
	{
		int16_t m_nCount;
		stService m_stService[max_service_type_count];
	};

	struct stServiceChg
	{
		int32_t m_iServiceID;
		int32_t m_iTimeChg;//��λ��S
		int32_t m_iExpireTime;
	};

	struct stServiceDataChg
	{
		int16_t m_nCount;
		stServiceChg m_stService[max_request_register_service_number];
	};

	//��*pOut����iByteCount���ֽ�
	//void skip_byte_of(char** pOut, int32_t iByteCount);


	int32_t encode_gameconfig( char** pOut, stPlayerGameConfig& config );
	int32_t decode_gameconfig(char** pOut, stPlayerGameConfig& config);

	int32_t encode_player_common_data(char** pOut, stPlayerCommonData& data);
	int32_t decode_player_common_data(char** pOut, stPlayerCommonData& data);

	//struct stPlayerCommonInfo
	int32_t encode_player_common_info(char** pOut,  stPlayerCommonInfo& common);
	int32_t decode_player_common_info(char** pOut, stPlayerCommonInfo& common);

	int32_t encode_player_db_gamedata(char** pOut, stDBGameData& gamedata);
	int32_t decode_player_db_gamedata(char** pOut, stDBGameData& gamedata);

	//
	int32_t encode_player_servicedata(char** pOut, stService& service);
	int32_t decode_player_servicedata(char** pOut, stService& service);

	//vip DATA
	int32_t encode_player_vipdata(char** pOut, stVipData& vipData,int32_t *pBecomeVIPTime=NULL);
	int32_t decode_player_vipdata(char** pOut, stVipData& vipData,int32_t *pBecomeVIPTime=NULL);

/*
class CBaseMessageBody :public CMessageBody
{
public:
	CBaseMessageBody(){return ;};
	~CBaseMessageBody(){return ;};

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength){return 0;};
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength){return 0;};
	virtual void dump(){return ;};
protected:
private:
};
*/
//SS_MSG_GETGAMEDATA
class CRequestGetGameData : public CMessageBody
{
public:
	CRequestGetGameData();
	~CRequestGetGameData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;			//Game DBҪ����iUin��֯Game Data��Cache
	TAccount m_szAccount;	//
	
	int16_t m_nGameID;
	int8_t m_cRequireBaseInfo;
	int8_t m_cAddRefrence;

protected:
private:
};

class CResponseGetGameData : public CMessageBody
{
public:
	CResponseGetGameData();
	~CResponseGetGameData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;
	int32_t m_iUin;
	TAccount m_szAccount;	//

	int8_t m_cHaveBaseInfo;
	
	stPlayerCommonInfo m_stBaseInfo;
	int8_t m_cIsFirstEnterGame;//�Ƿ��ǵ�һ�ε�¼��Ϸ����Ϸ,1=��,0=��,add by lz 2008.10.22
	stDBGameData m_stGameData;

	stServiceData m_stServiceData;

	 

protected:
private:
};

//SS_MSG_LOCKMONEY
class CRequestLockMoney : public CMessageBody
{
public:
	CRequestLockMoney();
	~CRequestLockMoney();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	
	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	int32_t m_iRoomID;
	int32_t m_iTableID;

	int16_t m_nGameID;
	int16_t m_nLockType;
	int32_t m_nLockMoney;// add by lz,2008.07.09
	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
	int8_t m_iLockStrategy;//����:��ֵ����m_nLockTypeΪ����ʱ���Ϊ��Ч...
protected:
private:
};

class CResponseLockMoney : public CMessageBody
{
public:
	CResponseLockMoney();
	~CResponseLockMoney();
	
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;

	int32_t m_iUin;
	char m_szAccount[max_account_string_length];

	int16_t m_nGameID;
	int16_t m_nLockType;

	int32_t m_iRoomID;
	int32_t m_iTableID;

	int32_t m_iCurrentMoney;			//��ǰ��Ϸ��
	int16_t m_iLockedServerType;			//��ǰռ����Ϸ�����ķ�����type
	int32_t m_iLockedServerID;			//��ǰռ����Ϸ�����ķ�����id
	int32_t m_iLockedRoomID;			//��ǰռ����Ϸ�����ķ���id
	int32_t m_iLockedTableID;			//��ǰռ����Ϸ��������Ϸ��id
	int32_t m_nLockMoney;				//��ǰռ����Ϸ������������� add by lz,2008.07.09
	
	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
	int8_t m_iLockStrategy;//����:��ֵ����m_nLockTypeΪ����ʱ���Ϊ��Ч...
protected:
private:
};


//SS_MSG_UPDATEPOINT request
class CRequestUpdateGamePoint : public CMessageBody
{
public:
	CRequestUpdateGamePoint();
	~CRequestUpdateGamePoint();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	
	int32_t m_iSourceIP;
	int16_t m_nPort;
	
	int16_t m_nGameID;
	int32_t m_iRoomID;
	int32_t m_iTableID;

	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;
	int64_t m_lDeltaHappyBean;

	int32_t m_iGameSeconds;					//��Ϸ��ʱ
	//int32_t m_iDeltaCharming;
	//int32_t m_iDeltaAchievement;
	int32_t m_iDeltaServiceFare;
	int64_t m_lDeltaServiceFareHappyBean;

	int32_t m_iACT;							//�첽��ɱ�ǣ�����Ϸ����ά������ƽ̨͸��
	char m_szGameTag[max_game_tag_length];	//��Ϸ�ֱ�ǩ�� Ψһ��ʾһ����Ϸ
	char m_szGamePara[max_game_para_length];	//��Ϸ����

	int32_t m_iRoomBillFlag;		//�˵�����: ��ͨ�����˵���VIP�����˵���
	int32_t m_iRoomActiveID;		//�����ڻID��Ĭ����0���޻



protected:
private:
};

//SS_MSG_UPDATEPOINT response
class CResponseUpdateGamePoint : public CMessageBody
{
public:
	CResponseUpdateGamePoint();
	~CResponseUpdateGamePoint();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;
	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	char m_szGameTag[max_game_tag_length];	
	int16_t m_nGameID;
	int32_t m_iRoomID;
	int32_t m_iTableID;

	//���º��(ȫ��)����
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	int32_t m_iMoneyChg;
	int64_t m_lHappyBean;    //������ֵ
	int64_t m_lHappyBeanChg; //ʵ�ʱ��ֵ
	//�������ݱ仯��
	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;
	int64_t m_iDeltaHappyBean;//������ֵ
	
	int32_t m_iACT;			//�첽��ɱ�ǣ�����Ϸ����ά������ƽ̨͸��
protected:
private:
};


//SS_MSG_REFRESH_GAMEPOINT notify
class CNotifyRefreshGamePoint : public CMessageBody
{
public:
	CNotifyRefreshGamePoint();
	~CNotifyRefreshGamePoint();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;
	int16_t m_nGameID;

	//���º��(ȫ��)����
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	int64_t m_lHappyBean; 
	
	int32_t m_iCharming;
	int32_t m_iAchievement;

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
protected:
private:
};

//������action serverʹ��
class CNotifyGameResult :public CMessageBody
{
public:
	CNotifyGameResult();
	~CNotifyGameResult();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	int16_t m_nGameID;
	int32_t m_iRoomID;
	int32_t m_iTableID;

	//���º��(ȫ��)����
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	int32_t m_iMoneyChg;
	int64_t m_lHappyBean;    //������ֵ
	int64_t m_lHappyBeanChg; //ʵ�ʱ��ֵ

	//�������ݱ仯��
	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;
	int64_t m_iDeltaHappyBean;//������ֵ

	//��������
	int32_t m_iDeltaGameSeconds;	//������Ϸ��ʱ
	int32_t m_iTotalGameSeconds;    //����Ϸ�ܹ���ʱ

	//HallServer
	int32_t m_iHallServerId;
	int32_t m_iHallServerPlayerId;

};

//SS_MSG_NOTIFY_LOCKMONEY notify
class CNotifyRefreshLockMoney : public CMessageBody
{
public:
	CNotifyRefreshLockMoney();
	~CNotifyRefreshLockMoney();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;
	int16_t m_nLockType;// add by lz 2008.04.24
	int32_t m_iServerID;
	int32_t m_iRoomID;
	int32_t m_iTableID;
	int32_t m_nLockMoney;// add by lz,2008.07.09

protected:
private:
};

//SS_MSG_UPDATE_GAME_CONFIG
class CRequestUpdateGameConfig : public CMessageBody//����'gameinfo_xx'
{
public:
	CRequestUpdateGameConfig();
	~CRequestUpdateGameConfig();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;
	int16_t m_nGameID;
	stPlayerGameConfig m_stGameConfig;
protected:
private:
};

class CResponseUpdateGameConfig : public CMessageBody
{
public:
	CResponseUpdateGameConfig();
	~CResponseUpdateGameConfig();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;

protected:
private:
};


//SS_MSG_CHANGEMONEY
enum enmChgMoneyStrategy
{
	money_change_strategy_must_be_all			= 1,			//�������۳�
	money_change_strategy_as_possible_as_unit	= 2,			//�����㣬�򾡿��ܿ۳����Ԫ��
	money_change_strategy_try_your_best_zero	= 3,			//�����㣬��۳�����
};

class CRequestChangeMoney : public CMessageBody
{
public:
	CRequestChangeMoney();
	~CRequestChangeMoney();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	
	UIN m_iUin;                                    //add by lz 2008.08.16 
	char m_szAccount[max_account_string_length]; //add by lz 2008.08.16
	int32_t m_iUnitMoneyChg; //ÿ���䶯�ĵ�Ԫ
	int16_t m_iUnitCount;	 //�䶯�ĵ�Ԫ��
	int8_t m_iStrategy;	//�䶯����,���� m_iUnitMoneyChg<0ʱ����Ч; ��ȡֵ: enmChgMoneyStrategy; =1:����Ҫȫ��۳�,=2:����û�����,�������ܿ۳�'�䶯��Ԫ'��������,3:����û�����,��۵�0.
	int32_t m_nServiceTag; //�μ�:server_public_define.h�ﶨ��� 'GET_SERVICE_TAG'
	int8_t m_cFlag;						//��ʶ�������ݿ���û��������ʱ���Ƿ�Ҫ����һ��������. 1 -- �ǣ� 0 -- ������
	char m_szDescription[max_string_message_length];
	char m_szTransTag[max_game_tag_length];			//�����ʶ....
	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//��GameDB͸��������
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
};

//add by lz
class CResponseChangeMoney : public CMessageBody
{
public:
	CResponseChangeMoney();
	~CResponseChangeMoney();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;

	UIN m_iUin;			

	int32_t m_iCurrentMoney;//���β�����󣬸��û���ǰ��Ǯ.

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//��GameDB͸��������

	int32_t m_iUnitMoneyChg;//����ʱ�Ĳ���
	int16_t m_iUnitCount;//����ʱ�Ĳ���
	int8_t m_iStrategy;

	int32_t m_iMoneyChg;
	char m_szResultStr[max_string_message_length]; //m_nResultID != result_id_success ��Ч
	char m_szTransTag[max_game_tag_length];		

	//int16_t m_nNofifyTransparentDataSize;
	//char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
};

//
struct stTSharedMoney 
{
	UIN m_iUin;
	char m_szAccount[max_account_string_length];
	int32_t m_iSharedMoney;

};
//SS_MSG_RECORD_SHARED_MONEY
class CRequestRecordSharedMoney : public CMessageBody
{
public:
	CRequestRecordSharedMoney();
	~CRequestRecordSharedMoney();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	UIN m_iUin;
	TAccount m_szAccount;

	char m_szGameTag[max_game_tag_length];	//��Ϸ�ֱ�ǩ�� Ψһ��ʾһ����Ϸ

	int32_t m_iSelfShared;
	int32_t m_iTotalShared;

	//int16_t m_nEntryCount;
	//stTSharedMoney m_astSharedMoney[max_ss_shared_money_entry_count];

protected:
private:
};

//SS_MSG_REFRESH_GAMECONFIG add by lz
class CNotifyRefreshGameConfig : public CMessageBody
{
public:
	CNotifyRefreshGameConfig();
	~CNotifyRefreshGameConfig();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_unUin;
	int32_t m_unTime;

	int32_t m_nGameId;
	stPlayerGameConfig  m_stGameCfg;

};


//SS_MSG_LOGICSERVER_INIT  notify . cs head�е�source id����Ϊ ServerID,
//typedef CBaseMessageBody CLogicServerInit;
//��Э��û��Э����

//SS_MSG_NOTIFY_DELETE_PLAYER_REFRENCE notify
class CNotifyDeletePlayerRefrence : public CMessageBody
{
public:
	CNotifyDeletePlayerRefrence();
	~CNotifyDeletePlayerRefrence();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;
	//ֻ��ʹ��stPlayerCommonInfo�е�m_iCoin�� m_iLoginCount��m_iOfflineCount�ȳ�Ա
	stPlayerCommonInfo m_stDeltaCommnonData;

protected:
private:
};


//SS_MSG_GETUSERINFO
class CRequestGetUserInfo :public CMessageBody
{
public :
	CRequestGetUserInfo();
	~CRequestGetUserInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	CRequestGetUserInfo& operator = (const CRequestGetUserInfo& original);
	int32_t m_unUin;

};


//
class CResponseGetUserInfo : public CMessageBody
{
public:
	CResponseGetUserInfo();
	~CResponseGetUserInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	CResponseGetUserInfo& operator = (const CResponseGetUserInfo& original);
	
	int16_t m_nResultID;
	int32_t m_iUin;

	int32_t m_nMoney;
	int32_t m_nCharming;
	int32_t m_nAchievement;
	int64_t m_lHappyBean;
	int32_t m_lLastOfflineCharming;

	//����
	stServiceData m_stServiceData;

	//add by lz, 2009-05-14
	TIDCard m_szIDCard;							//���֤��...
	char m_szUserName[max_player_name_length];//���֤�ϵ���ʵ����

	//add by lz,2009-06-30
	stVipData m_stVIP;

protected:
private:
};

//
typedef CRequestGetUserInfo CRequestGetUserProfile;
class CResponseGetUserProfile : public CResponseGetUserInfo
{
public: 
	CResponseGetUserProfile();
	~CResponseGetUserProfile();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int32_t m_iLastBecomeVIPTime;
	
};




//SS_MSG_GETOTHERUSERINFO
class CRequestGetOtherUserInfo :public CMessageBody
{

public :
	CRequestGetOtherUserInfo();
	~CRequestGetOtherUserInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int32_t m_unUin;
};

class CResponseGetOtherUserInfo : public CMessageBody
{
public:
	CResponseGetOtherUserInfo();
	~CResponseGetOtherUserInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nResultID;
	int32_t m_iUin;
	int32_t m_nCharming;
	int32_t m_nAchievement;
	char m_szResultStr[max_string_message_length];
protected:
private:
};


//SS_MSG_GET_PLAYER_ALL_GAME_DATA
class CRequestGetPlayerAllGameData : public CMessageBody
{
public:
			   CRequestGetPlayerAllGameData();
				~CRequestGetPlayerAllGameData();

				virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
				virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
				 virtual void dump();
				int32_t m_iUin;
protected:
private:
};
 
class CResponseGetPlayerAllGameData : public CMessageBody
{
public:
	CResponseGetPlayerAllGameData();
	~CResponseGetPlayerAllGameData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int16_t m_iResultId;
	int32_t m_iUin;
	int16_t m_nGameDataCount ;
	stDBGameData m_stGameData[max_game_data_count];
protected:
private:
};

class CRequestGetPlayCommonData : public CMessageBody
{
public :
	CRequestGetPlayCommonData();
	~CRequestGetPlayCommonData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int32_t m_nUin;
	int16_t m_nDataCount;
	int8_t  m_anDataType[PCD_MAX_DATA_NUMBER]; //PlayCommandDataType
	int8_t  m_anDataOtherPosi[PCD_MAX_DATA_NUMBER]; //������m_anDataType[i] = PCD_OtherDataʱ,m_nDataOtherPosi[i]��ֵ��������.
};

class CResponseGetPlayCommonData : public CMessageBody
{
public :
	CResponseGetPlayCommonData();
	~CResponseGetPlayCommonData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_nUin;
	int16_t m_nResultID;
	int16_t m_nDataCount;
	stPlayerCommonData m_astData[PCD_MAX_DATA_NUMBER];
};

//SS_MSG_UPDATE_PLAY_COMMON_DATA
class CRequestUpdatePlayCommonData : public CMessageBody
{
public:
	CRequestUpdatePlayCommonData();
	~CRequestUpdatePlayCommonData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_srcUin;
	char m_srcAccount[max_account_string_length];   
	int32_t m_dstUin;
	char m_dstAccount[max_account_string_length];   

	int16_t m_nDataCount;
	stPlayerCommonData m_DeltaData[PCD_MAX_DATA_NUMBER];
	 

	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
	char m_szTransTag[max_game_tag_length];		

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
protected:
private:

};


class CResponseUpdatePlayCommonData : public CMessageBody
{
public:
	CResponseUpdatePlayCommonData();
	~CResponseUpdatePlayCommonData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int16_t m_nResultID;
	int32_t m_srcUin;
	char m_srcAccount[max_account_string_length];   
	int32_t m_dstUin;
	char m_dstAccount[max_account_string_length];   
	int16_t m_nDataCount;
	stPlayerCommonData m_DeltaData[PCD_MAX_DATA_NUMBER];
	stPlayerCommonData m_CurrentData[PCD_MAX_DATA_NUMBER];
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
	char m_szTransTag[max_game_tag_length];		
protected:
private:

};

//SS_MSG_REFRESH_PLAY_COMMON_DATA
class CNotifyRefreshPlayCommonData : public CMessageBody
{
public:
	CNotifyRefreshPlayCommonData() ;
	~CNotifyRefreshPlayCommonData() ;

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

public:
	 int32_t m_iUin;
	 char m_szAccount[max_account_string_length];   
	 int16_t m_nDataCount;
	 stPlayerCommonData m_CurrentData[PCD_MAX_DATA_NUMBER];
	 int16_t m_nNofifyTransparentDataSize;
	 char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
protected:
private:

};


///////////////��Ϸ����չ��Ϣ /////////////////////////
enum UpdateMode
{
	upmd_null = 0,
	upmd_delta = 1,
	upmd_set = 2,
};
typedef struct
{
	int16_t m_nExtIndex;
	int8_t m_nUpdateMode;
	int32_t m_ExtVal;
} ExtGameDataUpdate;
 
class CRequestUpdateGameExtInfo : public CMessageBody
{
public:
	CRequestUpdateGameExtInfo();
	~CRequestUpdateGameExtInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nGameID;
	int16_t m_nUpdateIntCount;
	ExtGameDataUpdate m_astUpdateGameData[max_game_ext_int_count];
	int16_t m_nExtDataSize;
	char  m_szExtData[max_game_ext_data_size];
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};
class CResponseUpdateGameExtInfo : public CMessageBody
{
public:
	CResponseUpdateGameExtInfo();
	~CResponseUpdateGameExtInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nGameID;
	int16_t m_nResultID;
	TDBExtGameInfo m_stCurrentExtGameInfo;
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
	 
};

class CNotifyRefreshGameExtInfo : public CMessageBody
{
public:
	CNotifyRefreshGameExtInfo();
	~CNotifyRefreshGameExtInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nGameID;
	TDBExtGameInfo m_stCurrentExtGameInfo;
};

//�������֤��Ϣ...
class CRequestUpdatePlayerCardID:public CMessageBody
{
public:
	CRequestUpdatePlayerCardID();
	~CRequestUpdatePlayerCardID();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	CRequestUpdatePlayerCardID& operator = (const CRequestUpdatePlayerCardID& original);
	
	int32_t m_iUin;
	TAccount m_szAccount;
	TIDCard m_szIDCard;
	char m_szUserName[max_player_name_length];//�û����֤����ʵ����...
};

class CResponseUpdatePlayerCardID:public CMessageBody
{
public:
	CResponseUpdatePlayerCardID();
	~CResponseUpdatePlayerCardID();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	CResponseUpdatePlayerCardID& operator = (const CResponseUpdatePlayerCardID& original);

	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_nResultID;
};

//��ֵ,A��B��ֵ
class CRequestPlayerCharge:public CMessageBody
{
public:
	CRequestPlayerCharge();
	~CRequestPlayerCharge();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t m_iUin;//A
	TAccount m_szAccount;
	int32_t m_iOperUin;//B
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size];//
	int32_t m_iChannelId;// ����ID����4,��51�ҳ�ֵ,�����������ҳ�ֵ.
	int32_t m_iChannelAmout;//��������
	int32_t m_iPayAmount;//����Ϸ������
	int32_t m_iReqTimestamp;//����ʱʱ���

	char m_szTransTag[max_game_tag_length];	 
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

class CResponsePlayerCharge:public CMessageBody
{
public:
	CResponsePlayerCharge();
	~CResponsePlayerCharge();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t m_iUin;//֧����
	TAccount m_szAccount;
	int32_t m_iOperUin;//������
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size]; //֧��Ƶ������ˮ
	int32_t m_iChannelId;// ����ID����4,��51�ҳ�ֵ,�����������ҳ�ֵ
	int32_t m_iChannelAmout;//��������
	int32_t m_iPayAmount;//(��)��Ϸ������
	int32_t m_iReqTimestamp;//����ʱʱ���
	char m_szTransTag[max_game_tag_length];	//��ϷƵ����ˮ
	int32_t m_iResTimestamp;//����ʱʱ���
	int16_t m_iResultID;//���
	int32_t m_iMoney;   //���º�ĵ�ǰ��Ϸ�ҵ�����
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

//�ض�
class CRequestPlayerExchange:public CMessageBody
{
public:
	CRequestPlayerExchange();
	~CRequestPlayerExchange();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t m_iUin;//֧����
	TAccount m_szAccount;
	int32_t m_iOperUin;//������
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size];
	int32_t m_iExcid;//�һ��������ݼ�¼��id
	int32_t m_iPayAmount;//(��)��Ϸ������
	int32_t m_iReqTimestamp;//����ʱʱ���

	char m_szTransTag[max_game_tag_length];	
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

class CResponsePlayerExchange:public CMessageBody
{
public:
	CResponsePlayerExchange();
	~CResponsePlayerExchange();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;//֧����
	TAccount m_szAccount;
	int32_t m_iOperUin;//������
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size];
	int32_t m_iExcid;//�һ��������ݼ�¼��id
	int32_t m_iPayAmount;//(��)��Ϸ������
	int32_t m_iReqTimestamp;//����ʱʱ���
	char m_szTransTag[max_game_tag_length];	//��ϷƵ����ˮ
	int32_t m_iResTimestamp;//����ʱʱ���
	int16_t m_iResultID;//���,0=�ɹ�,1=��ȥ�ɹ�,2=ʧ��
	int32_t m_iMoney;   //���º�ĵ�ǰ��Ϸ�ҵ�����
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};


//SS_MSG_UPDATE_PLAYER_ENTHRALMENTDATA
class CRequestUpdatePlayerEnthralmentData:public CMessageBody
{
public:
	CRequestUpdatePlayerEnthralmentData();
	~CRequestUpdatePlayerEnthralmentData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int32_t m_iCumulativeOnLine;	//�ۼ�����ʱ��
	int32_t m_iCumulativeOffLine;	//�ۼ�����ʱ��
};

class CResponseUpdatePlayerEnthralmentData:public CMessageBody
{
public:
	CResponseUpdatePlayerEnthralmentData();
	~CResponseUpdatePlayerEnthralmentData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nResultID;      
};

 
//SS_MSG_GET_PLAYER_ENTHRALMENTDATA
class CRequestGetPlayerEnthralmentData:public CMessageBody
{
public:
	CRequestGetPlayerEnthralmentData();
	~CRequestGetPlayerEnthralmentData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
};
class CResponseGetPlayerEnthralmentData:public CMessageBody
{
public:
	CResponseGetPlayerEnthralmentData();
	~CResponseGetPlayerEnthralmentData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nResultID;  
	TIDCard m_szIDCard;				//���֤��
	int32_t m_iCumulativeOnLine;	//�ۼ�����ʱ��
	int32_t m_iCumulativeOffLine;	//�ۼ�����ʱ��
	time_t  m_tmLastOffline;		//��һ������ʱ��
};

enum
{
	//��ֵ...
	enum_StatLastHourChargeRMB=0,
	enum_StatLastDayChargeRMB=100,

	enum_StatLastHourCharge51=1,
	enum_StatLastDayCharge51=101,
	//�ض�...
	enum_StatLastHourExchange51=2,
	enum_StatLastDayExchange51=102,


	//ȫ��֧��:��������ֵ��ضҵ�����֧��....
	enum_StatLastDayOutgo2=103,

	//֧����֧:��Ϸ
	enum_StatLastHourGameOutgo=104,
	enum_StatLastDayGameOutgo=105,

	//֧����֧:����ƽ̨����
	enum_StatLast5MinBuyPlatformItem=106,
	enum_StatLastHourBuyPlatformItem=107 ,
	enum_StatLastDayBuyPlatformItem=108,

	//֧����֧:������������
	enum_StatLast5MinBuyCharmItem=109,
	enum_StatLastHourBuyCharmItem=110,
	enum_StatLastDayBuyCharmItem=111,

	//ȫ������:
	enum_StatLastDayIncome2=114,//��������ֵ������....
	//�����֧:game
	enum_StatLastHourGameIncome=115,
	enum_StatLastDayGameIncome=116,

	//�����֧:�һ�������Ʒ������.....
	enum_StatLast5MinCharmItemExchange=112,
	enum_StatLastHourCharmItemExchange=113, 
	enum_StatLastDayCharmItemExchange =6,

	//���ֶ�����
	enum_StatLast5MinHappyBeanPresentBuy = 200,
	enum_StatLastDayHappyBeanPresentSystem =201, //�û�1����ͨ��ϵͳ�������볬��50,000���ֶ�
	enum_StatLastDayHappyBeanOutgo =202,
	
	/*
	enum_StatLastHourIncome =3,
	enum_StatLastHourOutgo =4,
	enum_StatLastDayCharmItemConsume =5,
	enum_StatLastIncomeTimeStamp =7,
	enum_StatLastOutgoTimeStamp = 8,
	*/


	enum_StatMaxSize =10,
	enum_StatOneTimeChargeRMB=20,
    enum_StatOneTimeCharge51=21,
    enum_StatOneTimeExchange51=22,
    enum_StatOneTimeIncome=23,
    enum_StatOneTimeOutgo=24,
    enum_StatOneTimeGameIncome=25,
    enum_StatOneTimeGameOutgo=26,
    enum_StatServerHourCharge=30,
    enum_StatServerHourExchange=31,
    enum_StatServerHourShared=32,
    enum_StatServerHourGame =33,
    enum_StatServerHourItemConsume =34,
    enum_StatServerHourItemExchange=35,
    enum_StatServerDayCharge=40,
    enum_StatServerDayExchange=41,
    enum_StatServerDayShared=42,
    enum_StatServerDayGame =43,
    enum_StatServerDayItemConsume =44,
    enum_StatServerDayItemExchange=45,
    enum_StatServer5MinCharge=50,
    enum_StatServer5MinExchange=51,
    enum_StatServer5MinShared=52,
    enum_StatServer5MinGame =53,
    enum_StatServer5MinItemConsume =54,
    enum_StatServer5MinItemExchange=55,
};

typedef struct _tagStatData
{
	int16_t nStatIndex ;
	int32_t nStatValue;
}StatData;

//���
class CNotifyPlayerAccountChannge:public CMessageBody
{
public:
	CNotifyPlayerAccountChannge();
	~CNotifyPlayerAccountChannge();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	int32_t m_iChangeTime;//�����һ�α��ʱ��
	int32_t m_iChangeMoney;//�����һ�θı��ֵ,����ֵ
	int32_t m_iServiceType;//ҵ���ʶ
	char m_szTransTag[max_game_tag_length];//�����
	int32_t m_iSelfShared;//ϵͳ��ˮ,��ֵ,���=0,��ʾ��.
	
	int16_t m_stStatDataCount ;
	StatData m_aryStatData[enum_StatMaxSize];
};



//��ѯ��ǰDB�Ľ�ұ䶯���͵Ŀ���...
class CRequestQueryPlatformMoneyLockInfo:public CMessageBody
{
public:
	CRequestQueryPlatformMoneyLockInfo(){};
	~CRequestQueryPlatformMoneyLockInfo(){};
	CRequestQueryPlatformMoneyLockInfo& operator = (const CRequestQueryPlatformMoneyLockInfo& original)
	{return *this;};
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) { return success;};
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) {return success;};
	virtual void dump() {};
};


class CResponseQueryPlatformMoneyLockInfo:public CMessageBody
{
public:
	CResponseQueryPlatformMoneyLockInfo();
	~CResponseQueryPlatformMoneyLockInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
	int32_t m_iLockType; 
};

//���õ�ǰDB�Ľ�ұ䶯���͵Ŀ���
class CRequestUpdatePlatformMoneyLockInfo:public CMessageBody
{
public:
	CRequestUpdatePlatformMoneyLockInfo();
	~CRequestUpdatePlatformMoneyLockInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	CRequestUpdatePlatformMoneyLockInfo& operator = (const CRequestUpdatePlatformMoneyLockInfo& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iUpdateMode = original.m_iLockType;
		m_iLockType = original.m_iLockType;
		return *this;
	};
	virtual void dump() ;
	int8_t m_iUpdateMode;//ֵ��:(ö��)UpdateMode
	int32_t m_iLockType; 
};

class CResponseUpdatePlatformMoneyLockInfo:public CMessageBody
{
public:
	CResponseUpdatePlatformMoneyLockInfo();
	~CResponseUpdatePlatformMoneyLockInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
	int16_t m_nResultID; 
	int32_t m_iLockType; //����m_nResultID=successʱ,��ֵ��������,��ʾ��ǰƽ̨�����������.....
};

//SS_MSG_REFRESH_PLATFORM_MONEY_LOCK
typedef CResponseQueryPlatformMoneyLockInfo CNotifyPlatformMoneyLockInfo;

 

//ע�����

typedef struct {
	int32_t m_iServiceID;
    int8_t m_iTimeType;   //ʱ������:����ֵ,���ֵ
    int32_t m_iServiceTime; //ʱ��:TimeType_Relative,TimeType_Absolute
}TServiceRegisterInfo;

class CRequestRegisterService:public CMessageBody
{
public:
	CRequestRegisterService();
	~CRequestRegisterService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

    UIN m_iUin;
	TAccount m_szAccount;
    int16_t m_iServiceNumber;
    TServiceRegisterInfo m_aryServiceRegisterInfo[max_request_register_service_number];

	char m_szTransTag[max_game_tag_length];	
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
	
	int32_t m_nChannelId; //add by luozhen,2009-08-07
};

class CResponseRegisterService:public CMessageBody
{
public:
	CResponseRegisterService();
	~CResponseRegisterService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
    
    int16_t m_nResultID;
    UIN m_iUin;
    TAccount m_szAccount;
    struct stServiceDataChg  m_stServiceDataChg;

    char m_szTransTag[max_game_tag_length];	
    int16_t m_nTransparentDataSize;
    char m_TransparentData[max_transparent_data_size];
    int32_t m_nChannelId; //add by luozhen,2009-08-07
};

class CNotifyRefreshService:public CMessageBody
{
public:
	CNotifyRefreshService();
	~CNotifyRefreshService();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
	
	UIN m_iUin;
	struct stServiceData  m_stServiceData;//���ظ��û����α仯�ķ�����Ϣ.
};

//��ѯ�û���ϸ����
class CRequestGetUserDetailInfo : public CMessageBody
{
public:
public:
	CRequestGetUserDetailInfo();
	~CRequestGetUserDetailInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
	UIN m_iUin;
};


class CResponseGetUserDetailInfo : public CMessageBody
{
public:
	CResponseGetUserDetailInfo();
	~CResponseGetUserDetailInfo();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int16_t m_nResultID;
	UIN m_iUin;
 	stPlayerCommonInfo m_stBaseInfo;
	stServiceData m_stServiceData;

	//�����
	int16_t m_nLockType;
	int16_t m_iGameID;
	int16_t m_iLockedServerType;		//��ǰռ����Ϸ�����ķ�����type
	int32_t m_iLockedServerID;			//��ǰռ����Ϸ�����ķ�����id
	int32_t m_iLockedRoomID;			//��ǰռ����Ϸ�����ķ���id
	int32_t m_iLockedTableID;			//��ǰռ����Ϸ��������Ϸ��id
	int32_t m_nLockMoney;				//��ǰռ����Ϸ������������� add by lz,2008.07.09


	//���ֶ���
	int16_t m_nHappyBeanLockType;
	int16_t m_iHappyBeanGameID;
	int16_t m_iHappyBeanLockedServerType;		//��ǰռ����Ϸ�����ķ�����type
	int32_t m_iHappyBeanLockedServerID;			//��ǰռ����Ϸ�����ķ�����id
	int32_t m_iHappyBeanLockedRoomID;			//��ǰռ����Ϸ�����ķ���id
	int32_t m_iHappyBeanLockedTableID;			//��ǰռ����Ϸ��������Ϸ��id
};


class CRequestCheckUserIsExist:public CMessageBody
{
public:
	CRequestCheckUserIsExist();
	~CRequestCheckUserIsExist();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	UIN		m_iUin;
	int16_t m_nTransparentDataSize;
	char	m_TransparentData[max_transparent_data_size];
};

class CResponseCheckUserIsExist:public CMessageBody
{
public:
	CResponseCheckUserIsExist();
	~CResponseCheckUserIsExist();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	UIN		m_iUin;
	char	m_cIsExist; // 0=����,1=����
	int16_t m_nTransparentDataSize;
	char	m_TransparentData[max_transparent_data_size];
};


class CRequestRenewService:public CMessageBody
{
public:
	CRequestRenewService();
	~CRequestRenewService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	UIN		m_iUin;
	TAccount m_szAccount;

	int32_t m_nRenewPrice; //���Ѽ۸�,>=0
	TServiceRegisterInfo m_stServiceRegisterInfo;
	char m_szTransTag[max_game_tag_length];	
	int16_t m_nTransparentDataSize;
	char	m_TransparentData[max_transparent_data_size];
};

class CResponseRenewService:public CMessageBody
{
public:
	CResponseRenewService();
	~CResponseRenewService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int16_t m_nResultID;
	UIN m_iUin;
	TAccount m_szAccount;
	int32_t m_nCoin;//��ǰ��ʣ�Ľ����
	//struct stServiceDataChg  m_stServiceDataChg;
	stServiceChg m_stServiceChg;
	char m_szTransTag[max_game_tag_length];	
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

//SS_MSG_PRESENT_HAPPY_BEAN   request ���ͻ��ֶ�
class CSSRequestPresentHappyBean : public CMessageBody
{
public:
	CSSRequestPresentHappyBean();
	~CSSRequestPresentHappyBean();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int32_t m_iRoomID;
	int16_t m_nGameID;   //Ϊ�˵������ӵ�
	int16_t m_nPresentMode;
	int32_t m_iServiceTag;
	int64_t m_lDeltaHappyBean;
	int32_t m_iMaxPresentCount;
	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
	char m_szTransTag[max_game_tag_length];	
	
protected:
private:
};

//SS_MSG_PRESENT_HAPPY_BEAN  response
class CSSResponsePresentHappyBean : public CMessageBody
{
public:
	CSSResponsePresentHappyBean();
	~CSSResponsePresentHappyBean();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int16_t m_nResultID;

	int32_t m_iUin;
	int32_t m_iRoomID;
	int16_t m_nGameID;
	int16_t m_nPresentMode;
	int32_t m_iPresentedCount;
	int32_t m_iLastPresentedTime;
	int64_t m_lCurHappyBean;
	int64_t m_lDeltaHappyBean;
	int32_t m_iPlayerCommonInfoMiscFlag;   //stPlayerCommonInfo�е�misc flag
	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
	char m_szTransTag[max_game_tag_length];	

protected:
private:
};

//SS_MSG_PRESENT_HAPPY_BEAN   notify
class CSSNotifyPresentHappyBean: public CMessageBody
{
public:
	CSSNotifyPresentHappyBean();
	~CSSNotifyPresentHappyBean();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	int16_t m_nPresentMode;
	int32_t m_iPresentedCount;
	int32_t m_iLastPresentedTime;
	int64_t m_lCurHappyBean;
	int32_t m_iPlayerCommonInfoMiscFlag;
protected:
private:
};


//����ҵĻ��ֶ�
//SS_MSG_LOCKMONEY
class CRequestLockHappyBean : public CMessageBody
{
public:
	CRequestLockHappyBean();
	~CRequestLockHappyBean();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_iUin;
	int32_t m_iRoomID;
	int32_t m_iTableID;
	int16_t m_nGameID;
	int16_t m_nLockType;
	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
protected:
private:
};

class CResponseLockHappyBean : public CMessageBody
{
public:
	CResponseLockHappyBean();
	~CResponseLockHappyBean();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int16_t m_nResultID;
	int32_t m_iUin;
	int16_t m_nGameID;
	int32_t m_iRoomID;
	int32_t m_iTableID;
	int16_t m_nLockType;
	int64_t m_iCurrentHappyBean;			 
	int16_t m_iLockedServerType;			//��ǰռ����Ϸ�����ķ�����type
	int32_t m_iLockedServerID;			//��ǰռ����Ϸ�����ķ�����id
	int32_t m_iLockedRoomID;			//��ǰռ����Ϸ�����ķ���id
	int32_t m_iLockedTableID;			//��ǰռ����Ϸ��������Ϸ��id
 	int32_t m_iACT;			//�첽��ɱ�ǣ���ƽ̨͸�������ݣ�����Ϸ����ά��
protected:
private:
};


//SS_MSG_NOTIFY_LOCKMONEY notify
class CNotifyRefreshLockHappyBean : public CMessageBody
{
public:
	CNotifyRefreshLockHappyBean();
	~CNotifyRefreshLockHappyBean();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_iUin;
	int16_t m_nLockType;
	int32_t m_iServerID;
	int16_t m_nGameID;
	int32_t m_iRoomID;
	int32_t m_iTableID;

protected:
private:
};

//������ҵĻ��ֶ�
class CRequestChangeHappyBean:public CMessageBody
{
public:
	CRequestChangeHappyBean();
	~CRequestChangeHappyBean();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	UIN m_iUin;                                     
	char m_szAccount[max_account_string_length]; 
	int64_t m_lUnitHappyBeanChg; //ÿ���䶯�ĵ�Ԫ
	int16_t m_iUnitCount;	 //�䶯�ĵ�Ԫ��
	int8_t m_iStrategy;	//�䶯����,���� m_iUnitMoneyChg<0ʱ����Ч; ��ȡֵ: enmChgMoneyStrategy; =1:����Ҫȫ��۳�,=2:����û�����,�������ܿ۳�'�䶯��Ԫ'��������,3:����û�����,��۵�0.
	int32_t m_nServiceTag; //�μ�:server_public_define.h�ﶨ��� 'GET_SERVICE_TAG'
	int8_t m_cFlag;						//��ʶ�������ݿ���û��������ʱ���Ƿ�Ҫ����һ��������. 1 -- �ǣ� 0 -- ������
	char m_szTransTag[max_game_tag_length];			//�����ʶ....
	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//��GameDB͸��������
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
};

class CResponseChangeHappyBean : public CMessageBody
{
public:
	CResponseChangeHappyBean();
	~CResponseChangeHappyBean();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int16_t m_nResultID;
	UIN m_iUin;		
	char m_szAccount[max_account_string_length]; 
	int64_t m_lCurrentHappyBean;//���β�����󣬸��û���ǰ�Ķ�.
	int64_t m_lUnitHappyBeanChg;//����ʱ�Ĳ���
	int16_t m_iUnitCount;//����ʱ�Ĳ���
	int8_t m_iStrategy;
	int64_t m_lHappyBeanChg;
	char m_szTransTag[max_game_tag_length];		
	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; 
};


class CNotifyRefreshHappyBean:public CMessageBody
{
public:
	CNotifyRefreshHappyBean();
	~CNotifyRefreshHappyBean();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	UIN m_iUin;			
	int64_t m_lCurrentHappyBean;//���β�����󣬸��û���ǰ�Ķ�.
	int64_t m_lHappyBeanChg;
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; 
};


class CNotifyLogicHelloBeat:public CMessageBody
{
public:
	CNotifyLogicHelloBeat();
	~CNotifyLogicHelloBeat();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	UIN m_iUin;			
	int8_t m_nGameCount;
	int16_t m_aryGameId[max_game_count_in_server];
	
	int16_t m_nMoneyLockType;

	int16_t m_iMoneyLockedServerType;			 
	int32_t m_iMoneyLockedServerID;
	int16_t m_nMoneyLockedGameID;
	int32_t m_iMoneyLockedRoomID;			 
	int32_t m_iMoneyLockedTableID;			 
	int32_t m_nLockMoney;

	int16_t m_nHappyBeanLockType;
	int16_t m_iHappyBeanLockedServerType;			 
	int32_t m_iHappyBeanLockedServerID;	
	int16_t m_nHappyBeanLockedGameID;
	int32_t m_iHappyBeanLockedRoomID;			 
	int32_t m_iHappyBeanLockedTableID;	
};

//֪ͨȺ�û������ݱ仯
class CNotifyQunPlayerData:public CMessageBody
{
public:
	CNotifyQunPlayerData();
	~CNotifyQunPlayerData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	UIN m_iUin;
	stWebQunData m_stWebQunData;
	int32_t m_nServiceTag;
	int16_t m_nDataCount;
	stQunPlayerData m_astData[QPD_MAX_DATA_NUMBER];
};



typedef enum
{
	PAD_Type_Begin =0 ,        
	PAD_TYPE_LOGIN ,
	PAD_Type_End,   
	PAD_MAX_DATA_NUMBER = 30   
}PlayerActionDataType;



typedef struct _stOnlineData
{
	int32_t m_iLoginCount;
	int16_t m_nHallServerId;
	int32_t m_iHallPlayerId;
}stOnlineData;

typedef struct _stPlayerActionData
{
	int8_t m_nDataType;
	union _U
	{
		stOnlineData m_stOnlineData; //PAD_TYPE_LOGIN

	}m_DataValue;	

}stPlayerActionData;


//֪ͨ�û������ݱ仯��action server
class CNotifyPlayerActionData : public CMessageBody
{
public:
	CNotifyPlayerActionData();
	~CNotifyPlayerActionData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	UIN m_iUin;
	TAccount m_stAccount;
	int16_t m_nDataCount;
	stPlayerActionData m_astData[PAD_MAX_DATA_NUMBER];
};


}}
#endif
