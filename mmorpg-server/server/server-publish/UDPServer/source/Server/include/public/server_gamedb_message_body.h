#ifndef __SERVER_GAMEDB_MESSAGE_BODY_H__
#define __SERVER_GAMEDB_MESSAGE_BODY_H__
#include "server_message.h"


namespace KingNet { namespace Server { 


	enum enmVipType		//道具类型
	{
		vip_type_invalid		= 0,
		vip_type_normal  		= 1,//普通vip
		vip_type_mobile			= 2,//手机(包月)vip
		vip_type_honorary		= 4,//荣誉vip
	};
	
	enum enmLockType
	{
		lock_type_unlock            = 0,		//解锁
		lock_type_exclusive_lock    = 1,	    //增加互斥锁
		lock_type_force_unlock      = 2,        //强制解锁(仅限于GM发起强制解锁).
	};
	
	enum enmLockStrategy
	{
			lock_type_strategy_try = 1, //尽可能锁.
			lock_type_strategy_must =2,//必须锁全值...
	};

	enum enmLockMoney
	{
		lock_money_all = -1,	//锁全部的金币
	};

	enum enmIconType
	{
		icon_type_default           = 0,		//默认头像, 未设定头像
		icon_type_male_default      = 1,		//默认男性头像
		icon_type_female_default    = 2,		//默认女性头像

		icon_type_zone_photo        = 126,		//51空间形象照
		icon_type_game_photo        = 127,		//游戏形象照
	};

	enum 
	{
		max_game_other_data_count = 8,
		max_game_data_count = 256,
		max_game_ext_int_count =128,
		max_game_ext_data_size = 1024, //最大的扩展内存大小.
		max_web_order_id_size =30,//网站充值[回兑]的流水号大小
		max_web_qun_data_size = 1024,//网站群的信息块大小
		max_game_count_in_server = 10,//单server上最多可放10个游戏.
		
	};

	

	typedef enum
	{
		PCD_OP_Begin				= 0,
		PCD_OP_RejectAnyChat		= PCD_OP_Begin,     //拒绝接收任何私聊 
		PCD_OP_RejectAnyGameInvite	= 1,				//不接收任何游戏邀请 
		PCD_OP_AddFriendLimit		= 2,				//加玩伴的限制
		PCD_OP_VedioInvitationLimit	= 3,				//视频邀请的限制
		PCD_OP_End = 31
	}PCD_OthersPosi;
	
	

	//	
	typedef enum
	{
		PCD_Type_Begin =0 ,        
		PCD_Charming        = PCD_Type_Begin,    //魅力值
		PCD_Achievement     = 1,    //成就值
		PCD_PunishMethod    = 2,    //惩罚
		PCD_OtherData = 3,         //保留字段... //该字段作废
		PCD_LoginCount =4,         //平台登录次数,
		PCD_LastLoginTime =5,      //上一次的登录时间
		PCD_LastLoginIP =6,        //上一次的登录IP
		PCD_WebQunData =7,         //群的信息
		PCD_VipData = 8,		   //VIP信息
		//add by luozhen,20090828
		PCD_IdCard =9,             //身份证信息
		PCD_Sex = 10,              //性别
		PCD_Birthday = 11,         //出生年月
		
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
		update_mode_bit_non_and = 6, //先非再与
		update_mode_bit_non_or = 7, //先非再或
	};

	enum{
		TimeType_Relative =1 ,
		TimeType_Absolute ,
		TimeType_Subscribe,  //定购包月(手机,荣誉)
		TimeType_UnSubscribe, //取消包月(手机,荣誉)
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
	
	//惩罚方法....
	enum enmPunishMethod
	{
		punish_method_none = 0,        //无
		punish_method_forbit_chat = 1, //禁止发言

		punish_method_freeze_account = 5,//冻结帐号
	};

	struct stWebQunData
	{
		int16_t m_nDataSize ;
		char m_szWebQunInfo[max_web_qun_data_size];
	};

	struct stVipData
	{
		int32_t m_iGameVipExpireTime;	   //游戏VIP的过期时间(如果没有开通过，时间为0)
		int32_t m_iGameVIPScore;	   //游戏VIP积分
		int32_t m_iGameVIPLevel;	   //游戏VIP等级
		int32_t m_iNextUpdateVIPLevelTime; //下次升级的时间
		char m_szVIPTips[max_system_message_length]; //个人资料面板上的提示信息
		int32_t m_iGameVIPType ;           //游戏VIP类型,具体值定义参见  enmVIPType
	};

	struct stBirthday
	{
		int16_t m_iYear;
		int16_t m_iMonth;
		int32_t m_iDay;
	};
	//玩家基本信息==>DB Table UserInfo
	struct stPlayerCommonInfo 
	{
		int32_t m_iCoin;
		int32_t m_iLoginCount;
		int32_t m_iOfflineCount;
		int32_t m_iFirstLoginDatetime;		//第一次登陆平台的时间. added by aprilliu, 2009-1-1

		int32_t m_iCharming;
		int32_t m_iAchievement;
		int64_t m_lHappyBean;	//added by aprilliu, 2009-1-1

		//
		int32_t m_iBlackLevel;
		char m_szValidDate[max_date_string_length];
		int32_t m_iPunishMethod;		
		char m_szLastLoginDate[max_date_string_length];
		int32_t m_iLastLoginIP;		
		char m_szDescString[max_describe_string_length];		//进入黑名单的描述
		char m_szLastConsumeDate[max_date_string_length];
		

		int32_t m_iMiscFlag; //具体值域的定义,见后.
		//每天赠送欢乐豆enmHappyBeanPresentMode_daily_present
		int32_t m_iHappyBean_Daily_PresentedCount;//当天已赠送的次数
		int32_t m_iHappyBean_Daily_LastPresentedTime; //最近一次赠送的时间


		stWebQunData m_stWebQunData;
		//int16_t m_nOtherCount;
		int32_t m_aiOthers[player_data_other_count];			//预留信息, 下标访问

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


	//玩家游戏设置===> DB Table:UserGameInfo后部份数据
	struct stPlayerGameConfig
	{
		int32_t m_iMinPoint;					//最小积分
		int32_t m_iMaxPointGap;					//最大积分差距

		uint8_t m_byMaxOfflineRate;				//最大掉线率
		uint8_t m_byIPBit;					//
		uint32_t m_uiBitMap;					//


		uint8_t m_byOtherSize;					//个数
		int32_t m_aiOthers[max_game_other_data_count];

	};

	typedef struct 
	{
		int32_t m_aryExtInt[max_game_ext_int_count];
		int16_t m_nExtDataSize;
		char m_szExtData[max_game_ext_data_size];
	} TDBExtGameInfo;


	//玩家游戏数据===> DB Table:UserGameInfo
	typedef struct stDBGameData
	{
		int16_t m_nGameID;				//游戏类型编号

		int8_t m_byLevel;				//积分等级(无需此字段,请在5.1后拿掉...)

		int32_t m_iPoint;				//积分

		int32_t m_iWinRound;			//赢
		int32_t m_iLoseRound;			//输
		int32_t m_iDrawRound;			//平局
		int32_t m_iEscapeRound;			//逃跑局数

		int32_t m_iOrgID;				//所属帮派
		int16_t m_nPosition;			//帮派位置

		uint32_t m_uiTotalSecs;				//游戏时间
		char m_szLastDate[max_date_string_length]; //最后玩的时间.
		stPlayerGameConfig m_stGameConfig;		//游戏配置
		TDBExtGameInfo m_stExtGameInfo; //游戏扩展数据...
	}TDBGameData;



	enum enmPlayerCommonInfoMiscFlag
	{
		enmPlayerCommonInfoMiscFlag_FirstEnterPlatformPresentedBean=  (int32_t)0x00000001,  //第一次进入游戏平台赠送欢乐豆
		enmPlayerCommonInfoMiscFlag_FirstEnterHappyGamePresentedBean= (int32_t)0x00000002, //每一次进入欢乐游戏赠送欢乐豆
	};

	enum
	{
		max_service_type_count = 32,//512,

		max_ss_shared_money_entry_count = 16, //与 logic server中的max_shared_entry_count定义一致
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
		int32_t m_iTimeChg;//单位：S
		int32_t m_iExpireTime;
	};

	struct stServiceDataChg
	{
		int16_t m_nCount;
		stServiceChg m_stService[max_request_register_service_number];
	};

	//从*pOut跳过iByteCount个字节
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

	int32_t m_iUin;			//Game DB要根据iUin组织Game Data的Cache
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
	int8_t m_cIsFirstEnterGame;//是否是第一次登录游戏该游戏,1=是,0=否,add by lz 2008.10.22
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
	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
	int8_t m_iLockStrategy;//策略:此值仅当m_nLockType为锁的时候才为有效...
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

	int32_t m_iCurrentMoney;			//当前游戏币
	int16_t m_iLockedServerType;			//当前占有游戏币锁的服务器type
	int32_t m_iLockedServerID;			//当前占有游戏币锁的服务器id
	int32_t m_iLockedRoomID;			//当前占有游戏币锁的房间id
	int32_t m_iLockedTableID;			//当前占有游戏币锁的游戏桌id
	int32_t m_nLockMoney;				//当前占有游戏币锁的锁定金额 add by lz,2008.07.09
	
	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
	int8_t m_iLockStrategy;//策略:此值仅当m_nLockType为锁的时候才为有效...
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

	int32_t m_iGameSeconds;					//游戏耗时
	//int32_t m_iDeltaCharming;
	//int32_t m_iDeltaAchievement;
	int32_t m_iDeltaServiceFare;
	int64_t m_lDeltaServiceFareHappyBean;

	int32_t m_iACT;							//异步完成标记，由游戏对象维护，对平台透明
	char m_szGameTag[max_game_tag_length];	//游戏局标签， 唯一表示一局游戏
	char m_szGamePara[max_game_para_length];	//游戏参数

	int32_t m_iRoomBillFlag;		//账单类型: 普通房间账单，VIP房间账单等
	int32_t m_iRoomActiveID;		//房间内活动ID。默认是0，无活动



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

	//更新后的(全量)数据
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	int32_t m_iMoneyChg;
	int64_t m_lHappyBean;    //变更后的值
	int64_t m_lHappyBeanChg; //实际变更值
	//请求数据变化量
	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;
	int64_t m_iDeltaHappyBean;//请求变更值
	
	int32_t m_iACT;			//异步完成标记，由游戏对象维护，对平台透明
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

	//更新后的(全量)数据
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
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
protected:
private:
};

//仅报给action server使用
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

	//更新后的(全量)数据
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	int32_t m_iMoneyChg;
	int64_t m_lHappyBean;    //变更后的值
	int64_t m_lHappyBeanChg; //实际变更值

	//请求数据变化量
	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;
	int64_t m_iDeltaHappyBean;//请求变更值

	//附加数据
	int32_t m_iDeltaGameSeconds;	//本局游戏耗时
	int32_t m_iTotalGameSeconds;    //本游戏总共耗时

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
class CRequestUpdateGameConfig : public CMessageBody//更新'gameinfo_xx'
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
	money_change_strategy_must_be_all			= 1,			//必须足额扣除
	money_change_strategy_as_possible_as_unit	= 2,			//若余额不足，则尽可能扣除最大单元数
	money_change_strategy_try_your_best_zero	= 3,			//若余额不足，则扣除到零
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
	int32_t m_iUnitMoneyChg; //每个变动的单元
	int16_t m_iUnitCount;	 //变动的单元数
	int8_t m_iStrategy;	//变动策略,仅当 m_iUnitMoneyChg<0时才有效; 可取值: enmChgMoneyStrategy; =1:必须要全额扣除,=2:如果用户余额不足,尽最大可能扣除'变动单元'的整数倍,3:如果用户余额不足,则扣到0.
	int32_t m_nServiceTag; //参见:server_public_define.h里定义的 'GET_SERVICE_TAG'
	int8_t m_cFlag;						//标识。当数据库中没有数据项时，是否要创建一个数据项. 1 -- 是， 0 -- 不创建
	char m_szDescription[max_string_message_length];
	char m_szTransTag[max_game_tag_length];			//事务标识....
	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//对GameDB透明的数据
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
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

	int32_t m_iCurrentMoney;//本次操作完后，该用户当前的钱.

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//对GameDB透明的数据

	int32_t m_iUnitMoneyChg;//请求时的参数
	int16_t m_iUnitCount;//请求时的参数
	int8_t m_iStrategy;

	int32_t m_iMoneyChg;
	char m_szResultStr[max_string_message_length]; //m_nResultID != result_id_success 有效
	char m_szTransTag[max_game_tag_length];		

	//int16_t m_nNofifyTransparentDataSize;
	//char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
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

	char m_szGameTag[max_game_tag_length];	//游戏局标签， 唯一表示一局游戏

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


//SS_MSG_LOGICSERVER_INIT  notify . cs head中的source id设置为 ServerID,
//typedef CBaseMessageBody CLogicServerInit;
//该协议没有协议体

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
	//只会使用stPlayerCommonInfo中的m_iCoin， m_iLoginCount，m_iOfflineCount等成员
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

	//新增
	stServiceData m_stServiceData;

	//add by lz, 2009-05-14
	TIDCard m_szIDCard;							//身份证号...
	char m_szUserName[max_player_name_length];//身份证上的真实姓名

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
	int8_t  m_anDataOtherPosi[PCD_MAX_DATA_NUMBER]; //它仅当m_anDataType[i] = PCD_OtherData时,m_nDataOtherPosi[i]的值才有意义.
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
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
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
	 char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
protected:
private:

};


///////////////游戏的扩展信息 /////////////////////////
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

//更新身份证信息...
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
	char m_szUserName[max_player_name_length];//用户身份证的真实姓名...
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

//充值,A给B充值
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
	int32_t m_iChannelId;// 渠道ID，＝4,是51币充值,否则就是人民币充值.
	int32_t m_iChannelAmout;//渠道数量
	int32_t m_iPayAmount;//加游戏币数量
	int32_t m_iReqTimestamp;//请求时时间戳

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
	int32_t m_iUin;//支付人
	TAccount m_szAccount;
	int32_t m_iOperUin;//受益人
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size]; //支付频道的流水
	int32_t m_iChannelId;// 渠道ID，＝4,是51币充值,否则就是人民币充值
	int32_t m_iChannelAmout;//渠道数量
	int32_t m_iPayAmount;//(加)游戏币数量
	int32_t m_iReqTimestamp;//请求时时间戳
	char m_szTransTag[max_game_tag_length];	//游戏频道流水
	int32_t m_iResTimestamp;//请求时时间戳
	int16_t m_iResultID;//结果
	int32_t m_iMoney;   //更新后的当前游戏币的数量
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

//回兑
class CRequestPlayerExchange:public CMessageBody
{
public:
	CRequestPlayerExchange();
	~CRequestPlayerExchange();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t m_iUin;//支付人
	TAccount m_szAccount;
	int32_t m_iOperUin;//受益人
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size];
	int32_t m_iExcid;//兑换比率数据记录的id
	int32_t m_iPayAmount;//(减)游戏币数量
	int32_t m_iReqTimestamp;//请求时时间戳

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

	int32_t m_iUin;//支付人
	TAccount m_szAccount;
	int32_t m_iOperUin;//受益人
	TAccount m_szOperAccount;
	char m_szOrderID[max_web_order_id_size];
	int32_t m_iExcid;//兑换比率数据记录的id
	int32_t m_iPayAmount;//(减)游戏币数量
	int32_t m_iReqTimestamp;//请求时时间戳
	char m_szTransTag[max_game_tag_length];	//游戏频道流水
	int32_t m_iResTimestamp;//请求时时间戳
	int16_t m_iResultID;//结果,0=成功,1=过去成功,2=失败
	int32_t m_iMoney;   //更新后的当前游戏币的数量
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
	int32_t m_iCumulativeOnLine;	//累计在线时长
	int32_t m_iCumulativeOffLine;	//累计下线时长
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
	TIDCard m_szIDCard;				//身份证号
	int32_t m_iCumulativeOnLine;	//累计在线时长
	int32_t m_iCumulativeOffLine;	//累计下线时长
	time_t  m_tmLastOffline;		//上一次下线时间
};

enum
{
	//充值...
	enum_StatLastHourChargeRMB=0,
	enum_StatLastDayChargeRMB=100,

	enum_StatLastHourCharge51=1,
	enum_StatLastDayCharge51=101,
	//回兑...
	enum_StatLastHourExchange51=2,
	enum_StatLastDayExchange51=102,


	//全局支出:不包含充值与回兑的所有支出....
	enum_StatLastDayOutgo2=103,

	//支出分支:游戏
	enum_StatLastHourGameOutgo=104,
	enum_StatLastDayGameOutgo=105,

	//支出分支:购卖平台道具
	enum_StatLast5MinBuyPlatformItem=106,
	enum_StatLastHourBuyPlatformItem=107 ,
	enum_StatLastDayBuyPlatformItem=108,

	//支出分支:购卖魅力道具
	enum_StatLast5MinBuyCharmItem=109,
	enum_StatLastHourBuyCharmItem=110,
	enum_StatLastDayBuyCharmItem=111,

	//全局收入:
	enum_StatLastDayIncome2=114,//不包括充值的收入....
	//收入分支:game
	enum_StatLastHourGameIncome=115,
	enum_StatLastDayGameIncome=116,

	//收入分支:兑换魅力物品的收入.....
	enum_StatLast5MinCharmItemExchange=112,
	enum_StatLastHourCharmItemExchange=113, 
	enum_StatLastDayCharmItemExchange =6,

	//欢乐豆部份
	enum_StatLast5MinHappyBeanPresentBuy = 200,
	enum_StatLastDayHappyBeanPresentSystem =201, //用户1天内通过系统赠送收入超出50,000欢乐豆
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

//监控
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
	int32_t m_iChangeTime;//最近的一次变更时间
	int32_t m_iChangeMoney;//最近的一次改变的值,正负值
	int32_t m_iServiceType;//业务标识
	char m_szTransTag[max_game_tag_length];//事务号
	int32_t m_iSelfShared;//系统抽水,正值,如果=0,表示无.
	
	int16_t m_stStatDataCount ;
	StatData m_aryStatData[enum_StatMaxSize];
};



//查询当前DB的金币变动类型的开关...
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

//设置当前DB的金币变动类型的开关
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
	int8_t m_iUpdateMode;//值域:(枚举)UpdateMode
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
	int32_t m_iLockType; //仅当m_nResultID=success时,此值才有意义,表示当前平台金币锁的类型.....
};

//SS_MSG_REFRESH_PLATFORM_MONEY_LOCK
typedef CResponseQueryPlatformMoneyLockInfo CNotifyPlatformMoneyLockInfo;

 

//注册服务

typedef struct {
	int32_t m_iServiceID;
    int8_t m_iTimeType;   //时间类型:绝对值,相对值
    int32_t m_iServiceTime; //时间:TimeType_Relative,TimeType_Absolute
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
	struct stServiceData  m_stServiceData;//返回该用户本次变化的服务信息.
};

//查询用户详细资料
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

	//金币锁
	int16_t m_nLockType;
	int16_t m_iGameID;
	int16_t m_iLockedServerType;		//当前占有游戏币锁的服务器type
	int32_t m_iLockedServerID;			//当前占有游戏币锁的服务器id
	int32_t m_iLockedRoomID;			//当前占有游戏币锁的房间id
	int32_t m_iLockedTableID;			//当前占有游戏币锁的游戏桌id
	int32_t m_nLockMoney;				//当前占有游戏币锁的锁定金额 add by lz,2008.07.09


	//欢乐豆锁
	int16_t m_nHappyBeanLockType;
	int16_t m_iHappyBeanGameID;
	int16_t m_iHappyBeanLockedServerType;		//当前占有游戏币锁的服务器type
	int32_t m_iHappyBeanLockedServerID;			//当前占有游戏币锁的服务器id
	int32_t m_iHappyBeanLockedRoomID;			//当前占有游戏币锁的房间id
	int32_t m_iHappyBeanLockedTableID;			//当前占有游戏币锁的游戏桌id
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
	char	m_cIsExist; // 0=不在,1=存在
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

	int32_t m_nRenewPrice; //续费价格,>=0
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
	int32_t m_nCoin;//当前还剩的金币数
	//struct stServiceDataChg  m_stServiceDataChg;
	stServiceChg m_stServiceChg;
	char m_szTransTag[max_game_tag_length];	
	int16_t m_nTransparentDataSize;
	char m_TransparentData[max_transparent_data_size];
};

//SS_MSG_PRESENT_HAPPY_BEAN   request 赠送欢乐豆
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
	int16_t m_nGameID;   //为账单而增加的
	int16_t m_nPresentMode;
	int32_t m_iServiceTag;
	int64_t m_lDeltaHappyBean;
	int32_t m_iMaxPresentCount;
	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
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
	int32_t m_iPlayerCommonInfoMiscFlag;   //stPlayerCommonInfo中的misc flag
	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
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


//锁玩家的欢乐豆
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
	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
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
	int16_t m_iLockedServerType;			//当前占有游戏币锁的服务器type
	int32_t m_iLockedServerID;			//当前占有游戏币锁的服务器id
	int32_t m_iLockedRoomID;			//当前占有游戏币锁的房间id
	int32_t m_iLockedTableID;			//当前占有游戏币锁的游戏桌id
 	int32_t m_iACT;			//异步完成标记，对平台透明的数据，由游戏对象维护
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

//更新玩家的欢乐豆
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
	int64_t m_lUnitHappyBeanChg; //每个变动的单元
	int16_t m_iUnitCount;	 //变动的单元数
	int8_t m_iStrategy;	//变动策略,仅当 m_iUnitMoneyChg<0时才有效; 可取值: enmChgMoneyStrategy; =1:必须要全额扣除,=2:如果用户余额不足,尽最大可能扣除'变动单元'的整数倍,3:如果用户余额不足,则扣到0.
	int32_t m_nServiceTag; //参见:server_public_define.h里定义的 'GET_SERVICE_TAG'
	int8_t m_cFlag;						//标识。当数据库中没有数据项时，是否要创建一个数据项. 1 -- 是， 0 -- 不创建
	char m_szTransTag[max_game_tag_length];			//事务标识....
	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size];			//对GameDB透明的数据
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
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
	int64_t m_lCurrentHappyBean;//本次操作完后，该用户当前的豆.
	int64_t m_lUnitHappyBeanChg;//请求时的参数
	int16_t m_iUnitCount;//请求时的参数
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
	int64_t m_lCurrentHappyBean;//本次操作完后，该用户当前的豆.
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

//通知群用户的数据变化
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


//通知用户的数据变化给action server
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
