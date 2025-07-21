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
//7zipѹ��
#include "lzma/Types.h"
#include "lzma/Lzma86Enc.h"
#include "lzma/Lzma86Dec.h"

namespace KingNet { namespace Server {


    enum
    {
        max_setted_timer_count		= 16,		/**< ÿ���û����������õĶ�ʱ����Ŀ*/
        max_url_length				= 64,		//URL��ַ�ĳ���
        max_date_string_length		= 34,
        max_describe_string_length	= 128,
	    max_ipaddr_len = 30,					//ip�ĵ�ַ�ĳ���
        player_data_other_count		= 32, 
        max_player_id_length		= 32,		/**<  �û��ַ������ʺŵĳ���*/
        max_player_name_length		= 32,		/**< ����û����ֳ���*/
		max_identity_card_length	= 19,		/*���֤��Ϣ*/
		max_signature_length		= 256,		//signature��󳤶�
		max_room_session_count		= 8,		//һ���ͻ��ˣ����ͬʱ��8������
    };


    enum 
    {
		max_crypt_string_length			= 128,  //���ܴ���󳤶� 
        max_game_tag_length				= 32,
        max_game_para_length			= 32,
        max_account_string_length		= 32,   //�û�account��󳤶�
        max_nickname_length				= 64,	//�ǳ���󳤶�
        max_present_message_length		= 302,  //������󳤶ȣ�100������(ע�������Ҫ̫������һ���������Ƿ���͸�������еģ���֤�����͸������) 
        max_system_message_length		= 4096,	//ϵͳ��Ϣ����󳤶�
		max_reason_message_length		= 4096, //ԭ����󳤶�
        max_operate_description_length	= 64,   //����������󳤶�
        max_transparent_data_size		= 4096, //͸��������󳤶�
        
		max_sub_message_size			= 4096,	//SubMessage����󳤶�,�������л��Ժ���ڶ����͸�������е�
		max_qun_crypt_length			= 2048,	//51Ⱥ�������Ϣ
		max_profile_crypt_length		= 2048,	//profile���ܴ���󳤶�
		max_web_qun_count				= 32,	//��վȺ�ĸ���
		max_private_chat_length			= 3002,	//���һ����1000������
		max_loudspeaker_length			= 302,	//�������100������
		
		max_time_string_length = 32,			//����ʱ�䴮����	
    };

	//��ǩ����֤�����Ϣ
	enum
	{
		auth_service_key_length			= 16,	//����֤������Լ������Կ����
		crypt_key_length				= 16,	//��ͻ��˵�ͨѶʹ�õļ�����Կ����

		max_signature_valid_period		= 43200,//(12*60*60) 12Сʱ
		min_signature_valid_period		= 3600,	//60*60	1Сʱ

		max_encrypt_key_length = 16,			/**< ������Կ�ĳ���*/

		max_user_base_info_length = 2048,		//�û�������Ϣ��󳤶�
		max_chat_content		  =	2048,		//����������󳤶�
		max_error_msg_length	  =	512			//������Ϣ��󳤶�
	};

typedef char TAccount[max_account_string_length];			//���ͣ�account
typedef char TPresent[max_present_message_length];			//���ͣ�����
typedef char TReason[max_reason_message_length];			//���ͣ�ʧ��ԭ��
typedef char TNick[max_nickname_length];					//���ͣ��ǳ�
typedef char TCrypt[max_crypt_string_length];				//���ͣ����ܴ�
typedef char TIDCard[max_identity_card_length];				//����: ���֤����
typedef char TSystemMessage[max_system_message_length];		//���ͣ�ϵͳ��Ϣ
typedef char TChat[max_private_chat_length];				//���ͣ�˽����Ϣ
typedef char TSpeaker[max_loudspeaker_length];				//���ͣ�������Ϣ

typedef char TServiceBitmap[16] ;		//�����ʶλ



typedef char TUserBaseInfo[max_user_base_info_length];  //���ͣ��û�������Ϣ
typedef char TChatContent[max_chat_content];			//���ͣ���������

//CMessageHead�ĳ���
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

//һ���Զ���ָ���ÿյ�ģ�塣��CMessage�����������У����m_pstBody != NULL������Ҫɾ��body
//��ĳЩʱ��message body�Ƕ�ջ�Ϸ������ʱ����������Ҫ��message����ǰ��m_pstBody��Ϊ��.
//�����ö��������˳���ϸ����乹����෴��˳����е�C++��׼�����һ����

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
/*					    ��������֮��ͨ�õ�Э��                          */
/************************************************************************/

//���״̬
enum enmPlayerState
{
	player_offline	= 0,
	player_online	= 1,
	player_hide 	= 2,
	player_alive	= 3,	//hall server��score server֮��ʹ��
};

//֪ͨplayer����/����
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
	int8_t   m_cState;		//ö�ټ�enmPlayerState
	int32_t  m_iAddtional;	//���ֶθ�server�Զ���..
	int64_t  m_lMacAddr;	//�ͻ��˵�MAC��ַ
	int8_t	 m_cClientType; //�ͻ������ͣ���ʱ��������ȥ
};

//Server��init��֪ͨ.
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


//��State Server ����ת��͸�����ݵ�ָ�����͵�����Ŀ�ķ�����
class CRequestBroadcastMessage : public CMessageBody
{
public:
	CRequestBroadcastMessage();
	~CRequestBroadcastMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int8_t  m_nServerType;
	int16_t m_nDataSize;		//͸�����ݳ���
	char m_szTransparentData[max_transparent_data_size];	//͸������
};

class CResponseBroadcastMessage : public CMessageBody
{
public:
	CResponseBroadcastMessage();
	~CResponseBroadcastMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t     m_iResult;             //����ֵ��successΪ�ɹ�������Ϊʧ�� 
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
	int16_t m_nDataSize;		//͸�����ݳ���
	char m_szTransparentData[max_transparent_data_size];	//͸������
};

//����������
class CNotifyKickPlayer : public CMessageBody
{
public:
	CNotifyKickPlayer();
	~CNotifyKickPlayer();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t  m_iVictimUid;						//�����û�
	int32_t  m_iActorUid;						//�����û�
	int16_t  m_nReasonID;						//ȡֵgameplatfrom_message.h��result_id_kick_player_multilogin��
	TReason  m_szReasonMessage;
};

//����
typedef CNotifyKickPlayer CForbiddenTalk;

//�����˵�ID��
int32_t prepare_game_tag_string(char* pOut, int16_t iOutLength, int8_t server_type, int32_t server_id, int32_t timestamp, int32_t unique_sequence);



//ǰ�˷�����֮�乫����͸��������������Ҫ������notify�ͻ�����ʾ���ʵ���Ϣ����market server��hall server֮���֪ͨ�ͻ���˭��ta������Ʒ�����Ե�

//��ʾclient��������������Ʒ
struct stCharmItem
{
    int32_t  m_iSrcUIN;             //������UIN
	TAccount m_szSrcAccount;		//������account
	TNick	 m_szSrcNick;			//�������ǳ�
	int8_t   m_cSrcGender;			//�����ߵ��Ա�
    int32_t  m_iDestUIN;            //������UIN
    int32_t  m_iDeltaCharmValue;    //����ֵ�仯��
    int32_t  m_iItemID;             //����ID
	char     m_szItemName[max_item_name_length];	//��������
    int16_t  m_nDeltaItemCount;     //���߱仯��
    TPresent m_szPresentMessage;    //����
};

struct stCommonItemInfo
{
	int32_t m_iItemID;             //��ƷID
	char    m_szItemName[max_item_name_length];	//��������
	int16_t m_nDeltaItemCount;     //��Ʒ�仯��
};

//��ʾclient����������ͨ��Ʒ
struct stCommonItem
{
    int32_t  m_iSrcUIN;             //������UIN
	TAccount m_szSrcAccount;		//������account
	TNick	 m_szSrcNick;			//�������ǳ�
	int8_t   m_cSrcGender;			//�����ߵ��Ա�
    int32_t  m_iDestUIN;            //������UIN
	int8_t	 m_cItemTypeCount;		//���������
	stCommonItemInfo m_astCommonItemInfo[max_item_count_in_goods];	//��Ʒ�ľ�����Ϣ(һ����Ʒ��������8����Ʒ)
    TPresent m_szPresentMessage;    //����
	int32_t  m_iHappyBeanCount;		//���͵Ļ��ֶ�
};

//��ʾclient���˶��Լ�ʹ����ը��
struct stBombBlast
{
    int32_t  m_iSrcUIN;             //ʹ����UIN
	TAccount m_szSrcAccount;		//ʹ����account
	TNick	 m_szSrcNick;			//ʹ�����ǳ�
	int8_t   m_cSrcGender;			//ʹ���ߵ��Ա�
    int32_t  m_iDestUIN;            //�ܺ���UIN
    int32_t  m_iDeltaCharmValue;    //����ֵ�仯��
    int32_t  m_iItemID;             //����ID
	char     m_szItemName[max_item_name_length];	//��������
    int16_t  m_nDeltaItemCount;     //���߱仯��
};

class CNotifySystemMessage : public CMessageBody
{
public:
	CNotifySystemMessage();
	~CNotifySystemMessage();

	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	virtual void dump();

	int16_t m_nMessageSize;				//ϵͳ��Ϣ����
	TSystemMessage m_szSystemMessage;	//ϵͳ��Ϣ����buff
};

enum enmSubMessageID
{
    notify_present_charm_item   = 0x1000,   //֪ͨ������������Ʒ
    notify_present_common_item  = 0x1001,   //֪ͨ��������ͨ��Ʒ
    notify_use_bomb             = 0x1002,   //֪ͨ����ʹ��ը��
	notify_use_hall_speaker		= 0x1003,	//֪ͨ����ʹ�ô�������
	notify_system_message		= 0x1004,	//֪ͨ��ϵͳ��Ϣ
	notify_use_game_speaker		= 0x1005,	//֪ͨ����ʹ����Ϸ����
	notify_present_chest		= 0x1006,	//֪ͨϵͳ�����˱���
	notify_vip_message			= 0x1007,	//֪ͨVIP��Ϣ
};


//��Э��
class CSubMessage
{
public:
    CSubMessage();
    ~CSubMessage();

	int32_t  encode(char *pszOut, int32_t& iOutLength);
	int32_t  decode(const char *pszIn, const int32_t iInLength);
	void dump();

public:

    int16_t m_nMessageID;   //ö��ֵ��enmSubMessageID
    
    union
    {
		stCharmItem  m_stCharmItem;
        stCommonItem m_stCommonItem;
        stBombBlast  m_stBombBlast;
		stCommonItemInfo m_stChestInfo;
    };

	int16_t m_nBuffSize;	//�ͻ���ѹ�����ʾ��Ϣ��XML�ļ�����
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

//͸�����ݱ���룬����ֵΪ����ͽ���Ĵ�����
int32_t encode_sub_message(char** pszOut, CSubMessage& message);
int32_t decode_sub_message(char** pszIn, CSubMessage& message);

//��*pOut����iByteCount���ֽ�
void skip_byte_of(char** pOut, int32_t iByteCount);



//////////////////////////////////////////////////////////////////////////

enum enmProfileInfoMask
{
	profile_mask_video_certification = (int32_t)0x00000001,	//ͨ����վ��Ƶ��֤
	profile_mask_is_51vip = (int32_t)0x00000002,//�Ƿ���51vip
};

struct stCryptUserBaseWebProfile
{
	int32_t m_iFlag;	//����MASK��ö��enmProfileInfoMask
	int32_t m_iVIPScore;    
	int32_t m_iVIPLevel;
};

int32_t encode_profile_info(char** pszOut, stCryptUserBaseWebProfile& info);
int32_t decode_profile_info(char** pszIn, stCryptUserBaseWebProfile& info);

//����ֵΪ0��ɹ�������������ʧ��(ע�������iOutBuffLength�����buff��size������ʱ����buff���ݵĳ���)
int32_t encrypt_profile_info(char* pszOut, int16_t& nOutBuffLength, stCryptUserBaseWebProfile& info);
int32_t decrypt_profile_info(char* pszIn, int16_t nInBuffLength, stCryptUserBaseWebProfile& info);



struct stCryptUserWebScore
{
	int32_t m_i51Score;    
	int32_t m_i51Level;
};

int32_t encode_profile_webscore(char** pszOut, stCryptUserWebScore& info);
int32_t decode_profile_webscore(char** pszIn, stCryptUserWebScore& info);

//����ֵΪ0��ɹ�������������ʧ��(ע�������iOutBuffLength�����buff��size������ʱ����buff���ݵĳ���)
int32_t encrypt_profile_webscore(char* pszOut, int16_t& nOutBuffLength, stCryptUserWebScore& info);
int32_t decrypt_profile_webscore(char* pszIn, int16_t nInBuffLength, stCryptUserWebScore& info);




//��ҵ�ǩ����Ϣ
struct stPlayerSignature
{
	int16_t m_shPLen;						//ǩ������ ����PLen
	int32_t m_iUid;                         //�û�UID
	char m_szKey[max_encrypt_key_length];   //SessionKey
	int32_t m_iTimeStamp;					//ǩ����ȡʱ��ʱ���
	int32_t m_iOption;						//��չ�ֶ�
};


//��ҵ���ǩ����Ϣ��2009-06-16��
class CPlayerSignatureEx200906
{
public:
	CPlayerSignatureEx200906();
	~CPlayerSignatureEx200906();

	int32_t  encode(char *pszOut, int32_t& iOutLength);
	int32_t  decode(const char *pszIn, const int32_t iInLength);

public:
	int16_t m_nSize;	//ǩ������
	char m_szKey[max_encrypt_key_length];
	int32_t m_iUin;
	TAccount m_szUid;			//�ڲ��ʺ�
	TAccount m_szExtraUid;		//�ⲿ�ʺ�
	int32_t  m_nGoodId;			//GoodId
	TServiceBitmap m_szServiceBitmap;
	int32_t m_iTimeStamp;
	int16_t m_nSignatureFlag;	//���ּ����Զ����ϵ�
};


// ���ƽ̨ǩ��A
 struct stPlayerPlatformSignatureA 
 {
 	int32_t m_iFlag;				//ʹ��enmProfileInfoMask��Ϊmask����ȡֵ
 	int32_t m_i51VIPScore;			//51VIP����
 	int16_t m_n51VIPLevel;			//51VIP�ȼ�
 };
 
 //���ƽ̨ǩ��B
 struct stPlayerPlatformsignatureB 
 {
 	int32_t m_i51Score;				//51����
 	int16_t m_n51Level;				//51�ȼ�
 };


struct stQunDetailInfo
{
	char    m_szQunUID[max_qun_uid_length];	//Ⱥ���˺�
	int16_t m_nQunRole;						//��CSMessage_i.h�е�ö��enm51QunDefine
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
	int32_t m_iValidTime;		//����ʱ��
	int16_t  m_nQunCount;
	stQunDetailInfo m_astQunInfo[max_qun_count];
};

////����ֵΪ0��ɹ�������������ʧ��(ע�������iOutBuffLength�����buff��size������ʱ����buff���ݵĳ���)
int32_t encrypt_qun_info(char* pszOut, int16_t& nOutBuffLength, stCryptUser51Qun& info);
int32_t decrypt_qun_info(char* pszIn, int16_t nInBuffLength, stCryptUser51Qun& info);

//�����㷨��������
int16_t get_crypt_algorithm(int16_t flag);

int16_t get_compress_algorithm(int16_t flag);

//�õ�ǩ����Ϣ�ļ����㷨
int16_t get_signature_algorithm();

int32_t compress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength, 
						char* pszOut, int32_t& iOutLength);

int32_t uncompress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength, 
						  char* pszOut, int32_t& iOutLength);

int32_t crypt_buffer(int16_t nCryptAlgorithm, const char* pszKey, const char* pszIn, const int32_t iInLength,
					 char* pszOut, int32_t& iOutLength);

int32_t decrypt_buffer(int16_t nUnCryptAlgorithm, const char* pszKey, const char* pszIn, const int32_t iInLength, 
					   char* pszOut, int32_t& iOutLength);

//�������ǩ��
int32_t decrypt_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
								 char *pszOut, int32_t& nOutLength);

//����ǩ��
int32_t decode_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
								stPlayerSignature &stSignature);
//����ǩ��
int32_t encode_player_signature(char *pszOut, int32_t& iOutLength,stPlayerSignature stSignature);
////����200906�°汾ǩ��
int32_t decode_player_signature_ex_200906(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
										  CPlayerSignatureEx200906 &stSignature);

//���벿�ֵ�Message Head�Լ�Message Body
int32_t decrypt_cs_message(int16_t shFlag, const char* szKey, const char* pszIn, const int32_t iInLength, char* pszOut, int32_t& iOutLength);

bool is_game_vip(int32_t nVipType, int32_t iVipExpireTime);

}} //namespace KingNet { namespace Server {

#endif
