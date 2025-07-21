#ifndef __SERVER_MESSAGE_H__
#define __SERVER_MESSAGE_H__

#include <stdio.h>
#include "gameplatform_types.h"
#include "gameplatform_message.h"
#include "Common/common_base.h"
#include "Common/common_object.h"

#include "Connector/connector_head.h"

#include "public/server_item_define.h"
#include "public/cs_head.h"
//7zip压缩
#include "lzma/Types.h"
#include "lzma/Lzma86Enc.h"
#include "lzma/Lzma86Dec.h"

namespace KingNet { namespace Server {


    enum
    {
        max_setted_timer_count		= 16,		/**< 每个用户最多可以设置的定时器数目*/
        max_url_length				= 64,		//URL地址的长度
        max_date_string_length		= 34,
        max_describe_string_length	= 128,
	    max_ipaddr_len = 30,					//ip的地址的长度
        player_data_other_count		= 32, 
        max_player_id_length		= 32,		/**<  用户字符串型帐号的长度*/
        max_player_name_length		= 32,		/**< 最大用户名字长度*/
		max_identity_card_length	= 19,		/*身份证信息*/
		max_signature_length		= 256,		//signature最大长度
		max_room_session_count		= 8,		//一个客户端，最多同时开8个房间
    };


    enum 
    {
		max_crypt_string_length			= 128,  //加密串最大长度 
        max_game_tag_length				= 32,
        max_game_para_length			= 32,
        max_account_string_length		= 32,   //用户account最大长度
        max_nickname_length				= 64,	//昵称最大长度
        max_present_message_length		= 302,  //赠言最大长度，100个汉字(注：这个不要太长，有一部分赠言是放在透明数据中的，保证不溢出透明数据) 
        max_system_message_length		= 4096,	//系统消息的最大长度
		max_reason_message_length		= 4096, //原因最大长度
        max_operate_description_length	= 64,   //操作描述最大长度
        max_transparent_data_size		= 4096, //透明数据最大长度
        
		max_sub_message_size			= 4096,	//SubMessage的最大长度,它是序列化以后放在额外的透明数据中的
		max_qun_crypt_length			= 2048,	//51群组加密信息
		max_profile_crypt_length		= 2048,	//profile加密串最大长度
		max_web_qun_count				= 32,	//网站群的个数
		max_private_chat_length			= 3002,	//最多一次聊1000个汉字
		max_loudspeaker_length			= 302,	//喇叭最大100个汉字
		
		max_time_string_length = 32,			//最大的时间串长度	
    };

	//与签名认证相关信息
	enum
	{
		auth_service_key_length			= 16,	//与认证服务器约定的密钥长度
		crypt_key_length				= 16,	//与客户端的通讯使用的加密密钥长度

		max_signature_valid_period		= 43200,//(12*60*60) 12小时
		min_signature_valid_period		= 3600,	//60*60	1小时

		max_encrypt_key_length = 16,			/**< 加密密钥的长度*/

		max_user_base_info_length = 2048,		//用户基本信息最大长度
		max_chat_content		  =	2048,		//聊天内容最大长度
		max_error_msg_length	  =	512			//错误信息最大长度
	};

typedef char TAccount[max_account_string_length];			//类型：account
typedef char TPresent[max_present_message_length];			//类型：赠言
typedef char TReason[max_reason_message_length];			//类型：失败原因
typedef char TNick[max_nickname_length];					//类型：昵称
typedef char TCrypt[max_crypt_string_length];				//类型：加密串
typedef char TIDCard[max_identity_card_length];				//类型: 身份证号码
typedef char TSystemMessage[max_system_message_length];		//类型：系统消息
typedef char TChat[max_private_chat_length];				//类型：私聊信息
typedef char TSpeaker[max_loudspeaker_length];				//类型：喇叭信息

typedef char TServiceBitmap[16] ;		//服务标识位



typedef char TUserBaseInfo[max_user_base_info_length];  //类型：用户基本信息
typedef char TChatContent[max_chat_content];			//类型：聊天内容

//CMessageHead的长度
#define MESSAGE_HEAD_SIZE	(3*sizeof(int32_t) + 2*sizeof(int16_t) + 3*sizeof(int8_t))

class CMessageHead
{
public:
	CMessageHead();
	~CMessageHead();

	int32_t  encode(char* pszOut, int32_t& iOutLength);
	int32_t  decode(const char* pszIn, const int32_t iInLength);
	void dump();
	void dump(FILE* fp);
	void dump(const char* file);

	int16_t m_iMessageID;
	int8_t  m_nMessageType;
	int8_t  m_bSourceEntityType;
	int8_t  m_bDestEntityType;
	int32_t m_iSourceID;
	int32_t m_iDestID;
	int16_t m_shVersionType;
	int32_t m_iMessageSequence;
} ;
	
class CMessageBody
{
public:
	CMessageBody();
	virtual ~CMessageBody();
	virtual int32_t  encode(char* pszOut, int32_t& iOutLength) = 0;
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength) = 0;
	virtual void dump() = 0;
};

//一个自动把指针置空的模板。在CMessage的析构函数中，如果m_pstBody != NULL，则需要删除body
//而某些时候，message body是堆栈上分配的临时变量，所以要在message析构前把m_pstBody置为空.
//可利用对象的析构顺序严格按照其构造的相反的顺序进行的C++标准解决这一问题

template<typename T>
class CSmartNull
{
public:
	typedef T* value_point;

public:
	CSmartNull(T*& lv, T* rv):m_pT(lv)
	{
		m_pT = rv;
	}

	~CSmartNull()
	{
		m_pT = NULL;
	}

private:
	value_point& m_pT;
};

typedef CSmartNull<CMessageBody> CSmartNullMessageBody;



/************************************************************************/
/*					    各服务器之间通用的协议                          */
/************************************************************************/

//玩家状态
enum enmPlayerState
{
	player_offline	= 0,
	player_online	= 1,
	player_hide 	= 2,
	player_alive	= 3,	//hall server和score server之间使用
};

//通知player的上/下线
class CNotifyPlayerState: public CMessageBody
{
public:
	CNotifyPlayerState();
	~CNotifyPlayerState();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_iUin;
	TAccount m_szAccount;
	int8_t   m_cState;		//枚举见enmPlayerState
	int32_t  m_iAddtional;	//此字段各server自定义..
	int64_t  m_lMacAddr;	//客户端的MAC地址
	int8_t	 m_cClientType; //客户端类型，暂时不编解码进去
};

//Server的init的通知.
class CNotifyServerInit : public CMessageBody
{
public:
	CNotifyServerInit();
	~CNotifyServerInit();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iLocalIP;
	int16_t m_nLocalPort;
};


//向State Server 请求转发透明数据到指定类型的所有目的服务器
class CRequestBroadcastMessage : public CMessageBody
{
public:
	CRequestBroadcastMessage();
	~CRequestBroadcastMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int8_t  m_nServerType;
	int16_t m_nDataSize;		//透明数据长度
	char m_szTransparentData[max_transparent_data_size];	//透明数据
};

class CResponseBroadcastMessage : public CMessageBody
{
public:
	CResponseBroadcastMessage();
	~CResponseBroadcastMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t     m_iResult;             //返回值，success为成功，否则为失败 
	int8_t		m_nServerType;
	int16_t		m_nDataSize;
	TReason		m_szReasonMessage;
};

class CNotifyBroadcastMessage : public CMessageBody
{
public:
	CNotifyBroadcastMessage();
	~CNotifyBroadcastMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int8_t  m_nServerType;
	int16_t m_nDataSize;		//透明数据长度
	char m_szTransparentData[max_transparent_data_size];	//透明数据
};

//服务器踢人
class CNotifyKickPlayer : public CMessageBody
{
public:
	CNotifyKickPlayer();
	~CNotifyKickPlayer();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_iVictimUid;						//被踢用户
	int32_t  m_iActorUid;						//踢人用户
	int16_t  m_nReasonID;						//取值gameplatfrom_message.h中result_id_kick_player_multilogin等
	TReason  m_szReasonMessage;
};

//禁言
typedef CNotifyKickPlayer CForbiddenTalk;

//生成账单ID串
int32_t prepare_game_tag_string(char* pOut, int16_t iOutLength, int8_t server_type, int32_t server_id, int32_t timestamp, int32_t unique_sequence);



//前端服务器之间公共的透传数据声明，主要是用于notify客户端提示性质的消息，如market server与hall server之间的通知客户端谁送ta魅力物品和赠言等

//提示client有人赠送魅力物品
struct stCharmItem
{
    int32_t  m_iSrcUIN;             //赠送者UIN
	TAccount m_szSrcAccount;		//赠送者account
	TNick	 m_szSrcNick;			//赠送者昵称
	int8_t   m_cSrcGender;			//赠送者的性别
    int32_t  m_iDestUIN;            //受赠者UIN
    int32_t  m_iDeltaCharmValue;    //魅力值变化量
    int32_t  m_iItemID;             //道具ID
	char     m_szItemName[max_item_name_length];	//道具名称
    int16_t  m_nDeltaItemCount;     //道具变化量
    TPresent m_szPresentMessage;    //赠言
};

struct stCommonItemInfo
{
	int32_t m_iItemID;             //物品ID
	char    m_szItemName[max_item_name_length];	//道具名称
	int16_t m_nDeltaItemCount;     //物品变化量
};

//提示client有人赠送普通物品
struct stCommonItem
{
    int32_t  m_iSrcUIN;             //赠送者UIN
	TAccount m_szSrcAccount;		//赠送者account
	TNick	 m_szSrcNick;			//赠送者昵称
	int8_t   m_cSrcGender;			//赠送者的性别
    int32_t  m_iDestUIN;            //受赠者UIN
	int8_t	 m_cItemTypeCount;		//多少类道具
	stCommonItemInfo m_astCommonItemInfo[max_item_count_in_goods];	//物品的具体信息(一个商品最多可能有8类物品)
    TPresent m_szPresentMessage;    //赠言
	int32_t  m_iHappyBeanCount;		//赠送的欢乐豆
};

//提示client有人对自己使用了炸弹
struct stBombBlast
{
    int32_t  m_iSrcUIN;             //使用者UIN
	TAccount m_szSrcAccount;		//使用者account
	TNick	 m_szSrcNick;			//使用者昵称
	int8_t   m_cSrcGender;			//使用者的性别
    int32_t  m_iDestUIN;            //受害者UIN
    int32_t  m_iDeltaCharmValue;    //魅力值变化量
    int32_t  m_iItemID;             //道具ID
	char     m_szItemName[max_item_name_length];	//道具名称
    int16_t  m_nDeltaItemCount;     //道具变化量
};

class CNotifySystemMessage : public CMessageBody
{
public:
	CNotifySystemMessage();
	~CNotifySystemMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nMessageSize;				//系统消息长度
	TSystemMessage m_szSystemMessage;	//系统消息内容buff
};

enum enmSubMessageID
{
    notify_present_charm_item   = 0x1000,   //通知有赠送魅力物品
    notify_present_common_item  = 0x1001,   //通知有赠送普通物品
    notify_use_bomb             = 0x1002,   //通知有人使用炸弹
	notify_use_hall_speaker		= 0x1003,	//通知有人使用大厅喇叭
	notify_system_message		= 0x1004,	//通知有系统消息
	notify_use_game_speaker		= 0x1005,	//通知有人使用游戏喇叭
	notify_present_chest		= 0x1006,	//通知系统赠送了宝箱
	notify_vip_message			= 0x1007,	//通知VIP信息
};


//子协议
class CSubMessage
{
public:
    CSubMessage();
    ~CSubMessage();

	int32_t  encode(char *pszOut, int32_t& iOutLength);
	int32_t  decode(const char *pszIn, const int32_t iInLength);
	void dump();

public:

    int16_t m_nMessageID;   //枚举值见enmSubMessageID
    
    union
    {
		stCharmItem  m_stCharmItem;
        stCommonItem m_stCommonItem;
        stBombBlast  m_stBombBlast;
		stCommonItemInfo m_stChestInfo;
    };

	int16_t m_nBuffSize;	//客户端压入的提示信息框XML文件内容
	char	m_szTransparentBuff[max_transparent_data_size];
};


int32_t encode_charm_item_message(char** pszOut, stCharmItem& message);
int32_t decode_charm_item_message(char** pszIn, stCharmItem& message);

int32_t encode_common_item_info_message(char** pszOut, stCommonItemInfo& message);
int32_t decode_common_item_info_message(char** pszIn, stCommonItemInfo& message);

int32_t encode_common_item_message(char** pszOut, stCommonItem& message);
int32_t decode_common_item_message(char** pszIn, stCommonItem& message);


int32_t encode_bomb_blast_message(char** pszOut, stBombBlast& message);
int32_t decode_bomb_blast_message(char** pszIn, stBombBlast& message);

//透明数据编解码，返回值为编码和解码的串长度
int32_t encode_sub_message(char** pszOut, CSubMessage& message);
int32_t decode_sub_message(char** pszIn, CSubMessage& message);

//从*pOut跳过iByteCount个字节
void skip_byte_of(char** pOut, int32_t iByteCount);



//////////////////////////////////////////////////////////////////////////

enum enmProfileInfoMask
{
	profile_mask_video_certification = (int32_t)0x00000001,	//通过网站视频认证
	profile_mask_is_51vip = (int32_t)0x00000002,//是否是51vip
};

struct stCryptUserBaseWebProfile
{
	int32_t m_iFlag;	//它的MASK见枚举enmProfileInfoMask
	int32_t m_iVIPScore;    
	int32_t m_iVIPLevel;
};

int32_t encode_profile_info(char** pszOut, stCryptUserBaseWebProfile& info);
int32_t decode_profile_info(char** pszIn, stCryptUserBaseWebProfile& info);

//返回值为0则成功，返回其它则失败(注：传入的iOutBuffLength是这个buff的size，返回时候是buff内容的长度)
int32_t encrypt_profile_info(char* pszOut, int16_t& nOutBuffLength, stCryptUserBaseWebProfile& info);
int32_t decrypt_profile_info(char* pszIn, int16_t nInBuffLength, stCryptUserBaseWebProfile& info);



struct stCryptUserWebScore
{
	int32_t m_i51Score;    
	int32_t m_i51Level;
};

int32_t encode_profile_webscore(char** pszOut, stCryptUserWebScore& info);
int32_t decode_profile_webscore(char** pszIn, stCryptUserWebScore& info);

//返回值为0则成功，返回其它则失败(注：传入的iOutBuffLength是这个buff的size，返回时候是buff内容的长度)
int32_t encrypt_profile_webscore(char* pszOut, int16_t& nOutBuffLength, stCryptUserWebScore& info);
int32_t decrypt_profile_webscore(char* pszIn, int16_t nInBuffLength, stCryptUserWebScore& info);




//玩家的签名信息
struct stPlayerSignature
{
	int16_t m_shPLen;						//签名长度 包含PLen
	int32_t m_iUid;                         //用户UID
	char m_szKey[max_encrypt_key_length];   //SessionKey
	int32_t m_iTimeStamp;					//签名获取时的时间戳
	int32_t m_iOption;						//扩展字段
};


//玩家的新签名信息（2009-06-16）
class CPlayerSignatureEx200906
{
public:
	CPlayerSignatureEx200906();
	~CPlayerSignatureEx200906();

	int32_t  encode(char *pszOut, int32_t& iOutLength);
	int32_t  decode(const char *pszIn, const int32_t iInLength);

public:
	int16_t m_nSize;	//签名长度
	char m_szKey[max_encrypt_key_length];
	int32_t m_iUin;
	TAccount m_szUid;			//内部帐号
	TAccount m_szExtraUid;		//外部帐号
	int32_t  m_nGoodId;			//GoodId
	TServiceBitmap m_szServiceBitmap;
	int32_t m_iTimeStamp;
	int16_t m_nSignatureFlag;	//保持兼容性而加上的
};


// 玩家平台签名A
 struct stPlayerPlatformSignatureA 
 {
 	int32_t m_iFlag;				//使用enmProfileInfoMask作为mask进行取值
 	int32_t m_i51VIPScore;			//51VIP积分
 	int16_t m_n51VIPLevel;			//51VIP等级
 };
 
 //玩家平台签名B
 struct stPlayerPlatformsignatureB 
 {
 	int32_t m_i51Score;				//51积分
 	int16_t m_n51Level;				//51等级
 };


struct stQunDetailInfo
{
	char    m_szQunUID[max_qun_uid_length];	//群组账号
	int16_t m_nQunRole;						//见CSMessage_i.h中的枚举enm51QunDefine
};

int32_t encode_qun_info(char** pszOut, stQunDetailInfo& qun_info);
int32_t decode_qun_info(char** pszIn, stQunDetailInfo& qun_info);

typedef struct _PlayerQunData
{
	int16_t m_nQunCount;
	stQunDetailInfo m_astQunDetailInfo[max_web_qun_count];
}stPlayerQunData;

int32_t encode_player_qun_data(char* pszOut, int16_t& nOutBuffLength, stPlayerQunData& player_qun_data);
int32_t decode_player_qun_data(char* pszIn, int16_t nInBuffLength, stPlayerQunData& player_qun_data);

struct stCryptUser51Qun
{
	TAccount m_szAccount;
	int32_t m_iValidTime;		//到期时间
	int16_t  m_nQunCount;
	stQunDetailInfo m_astQunInfo[max_qun_count];
};

////返回值为0则成功，返回其它则失败(注：传入的iOutBuffLength是这个buff的size，返回时候是buff内容的长度)
int32_t encrypt_qun_info(char* pszOut, int16_t& nOutBuffLength, stCryptUser51Qun& info);
int32_t decrypt_qun_info(char* pszIn, int16_t nInBuffLength, stCryptUser51Qun& info);

//加密算法可以配置
int16_t get_crypt_algorithm(int16_t flag);

int16_t get_compress_algorithm(int16_t flag);

//得到签名信息的加密算法
int16_t get_signature_algorithm();

int32_t compress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength, 
						char* pszOut, int32_t& iOutLength);

int32_t uncompress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength, 
						  char* pszOut, int32_t& iOutLength);

int32_t crypt_buffer(int16_t nCryptAlgorithm, const char* pszKey, const char* pszIn, const int32_t iInLength,
					 char* pszOut, int32_t& iOutLength);

int32_t decrypt_buffer(int16_t nUnCryptAlgorithm, const char* pszKey, const char* pszIn, const int32_t iInLength, 
					   char* pszOut, int32_t& iOutLength);

//解密玩家签名
int32_t decrypt_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
								 char *pszOut, int32_t& nOutLength);

//解码签名
int32_t decode_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
								stPlayerSignature &stSignature);
//编码签名
int32_t encode_player_signature(char *pszOut, int32_t& iOutLength,stPlayerSignature stSignature);
////解码200906新版本签名
int32_t decode_player_signature_ex_200906(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
										  CPlayerSignatureEx200906 &stSignature);

//解码部分的Message Head以及Message Body
int32_t decrypt_cs_message(int16_t shFlag, const char* szKey, const char* pszIn, const int32_t iInLength, char* pszOut, int32_t& iOutLength);

bool is_game_vip(int32_t nVipType, int32_t iVipExpireTime);

}} //namespace KingNet { namespace Server {

#endif
