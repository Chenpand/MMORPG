#ifndef __SERVER_ITEMDB_MESSAGE_BODY_H__
#define __SERVER_ITEMDB_MESSAGE_BODY_H__
#include "server_message.h"
#include "server_item_define.h"
#include "server_state_message_body.h"
namespace KingNet { namespace Server { 


//���������count���ʹ������һ������
#define MAX_GOODS_UPDATE_COUNT	15
#define MAX_ITEM_UPDATE_COUNT 	(MAX_GOODS_UPDATE_COUNT * max_item_count_in_goods)   
#define MAX_ITEM_COUNT 512
#define MAX_EQUIPMENT_COUNT 16 //�û���װ����(ħ��)��������ĸ��� add by luozhen,2009-06-11

#define MAX_QUERY_COUNT 8


//���߸���ģʽ
enum enmItemDataUpdateMode //����ö��ֵ�ı仯��Ҫ��logicserver_itablegame.h �е�enmItemUpdateMode����һ��
{
    ITEM_DATA_UPDATE_MODE_DEFAULT   = 0,    //�ղ��������������ݵĸ��£�ֻ�Ǽ�¼�˵�
    ITEM_DATA_UPDATE_MODE_DELTA     = 1,    //��������
    ITEM_DATA_UPDATE_MODE_ALL       = 2,    //ȫ������
};

struct stItemInfo
{
    int32_t m_nItemID;      //����ID
    int16_t m_nItemType;    //�������ͣ�ö��ֵ��enmItemType
    int32_t m_iCount;       //��������
    int16_t m_nGameID;      //����ʹ�õĳ�������ϷID��
    int32_t m_nExpiredTime;  //���ߵ�ʧЧʱ��
	int8_t  m_cUpdateMode;      //����ģʽ����enmItemDataUpdateMode
};


struct stItemInfo2
{
	int32_t m_nItemID;      //����ID
	int32_t m_iCount;       //��������
	int32_t m_nExpiredTime;  //���ߵ�ʧЧʱ��
};

struct stGoodsInfo
{
	int32_t m_nGoodsID;	//��ƷID
	int32_t m_nCount;	//��Ʒ����
	int32_t m_nPrice;	//��Ʒ�۸�	
};

struct stSuperTypeInfo
{
	int32_t m_nItemSuperType;
	int32_t m_nItemCount;       //��������
	stItemInfo2 m_stItemInfos[MAX_ITEM_COUNT]; //����ĵ�����Ϣ
};
// װ�����·�ʽ:add by luozhen,2009-06-11
enum enmEquipmentUpdateMode
{
	Equipment_Update_Mode_must = 0, //����,���۸�λ�����Ƿ��иõ���.
	Equipment_Update_Mode_if = 1, // �����λ��������Ч�ĵ���,���õ�ǰ�ĸ��µĵ��ߣ�������������.
	Equipment_Update_Mode_Expiretime = 2, //��ʱ����.
};
//װ�����µ�Ԫ
struct stEquipmentUpdate
{
	int32_t m_nEquipmentPosi; //װ��λ,��װ��������±�
	int32_t m_nItemId;       //װ���ĵ���ID
	int8_t m_nEquipmentUpdateMod; //װ�����µķ�ʽ,Equipment_Update_Mode_must ��ȱʡ
};

int32_t encode_item_info(char** pszOut, stItemInfo& item_info);
int32_t decode_item_info(char** pszIn, stItemInfo& item_info);

int32_t encode_item_info2(char** pszOut, stItemInfo2& item_info);
int32_t decode_item_info2(char** pszIn, stItemInfo2& item_info);


int32_t encode_goods_info(char** pszOut, stGoodsInfo& good_info);
int32_t decode_goods_info(char** pszIn,  stGoodsInfo&  good_info);

int32_t encode_EquipmentUpdate(char** pszOut, stEquipmentUpdate& equipment);
int32_t decode_EquipmentUpdate(char** pszOut, stEquipmentUpdate& equipment);
 
//���߸�������hall --> item db
class CResquestUpdateItemInfo : public CMessageBody
{
public:
    CResquestUpdateItemInfo();
    ~CResquestUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int32_t  m_nRealUIN;        //ʵ��Ҫ���и�����Ʒ��Ϣ�����UIN
    TAccount m_szRealAccount;	//ʵ��Ҫ���и�����Ʒ��Ϣ�����account
    int32_t	 m_nActorUIN;       //�����˴θ��²��������UIN��������Լ��������Լ�����m_nRealUIN��m_nAcotrUIN�����Լ�
    TAccount m_szActorAccount;	//�����˴θ��²��������account
	int32_t  m_nServiceTag;     //ҵ���ʶ,��ҵ������(�������ֽ�,����ֵ��μ� server_public_define.h���enmServiceTagType�Ķ���)+��ҵ������(�������ֽ�,����ֵ��μ�server_public_define.h��enmServiceTagXXX�Ķ���)���.
    int32_t  m_nSourceTag;		//����ҵ���Դ,��Դ������(�������ֽ�,����ֵ����μ� server_public_define.h��enmUpdateItemInfoSourceType����)+Դ��������(�������ֽ����,���'Դ����=update_item_source_type_game',���ֵGameID,������Ϊ0).
    int32_t  m_nIP;             //ip��ַ��Ϣ

    int16_t  m_nItemCount;	//��������
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT];	//����ĵ�����Ϣ
    
    int16_t m_nGoodsCount;	//��Ʒ����
    stGoodsInfo m_stGoodsInfo[MAX_GOODS_UPDATE_COUNT];	//��Ʒ����ϸ��Ϣ..

    int16_t m_nTransparentDataSize;
    char m_szTransparentData[max_transparent_data_size]; //��item DB͸��������

    char m_szOperateDescription[max_operate_description_length];
    char m_szTransTag[max_game_tag_length];		

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������

};

//���߸���Ӧ�� item db --> hall server
class CResponseUpdateItemInfo : public CMessageBody
{
public:
    CResponseUpdateItemInfo();
    ~CResponseUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int16_t  m_nResultID;       //���ؽ��
    int32_t  m_nRealUIN;        //ʵ��Ҫ���и�����Ʒ��Ϣ�����UIN
    TAccount m_szRealAccount;	//ʵ��Ҫ���и�����Ʒ��Ϣ�����account
    int32_t  m_nActorUIN;       //�����˴θ��²��������UIN��������Լ��������Լ�����m_nRealUIN��m_nAcotrUIN�����Լ�
    TAccount m_szActorAccount;	//�����˴θ��²��������account

    int16_t m_nItemCount;	//��������
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT];	//����ĵ�����Ϣ

    int16_t m_nTransparentDataSize;
    char m_szTransparentData[max_transparent_data_size];	//��item DB͸��������

    char m_szReasonMsg[max_reason_message_length];
    char m_szTransTag[max_game_tag_length];		
};


//notify��Ϣ��֪ͨ(ˢ��)�����Ʒ��Ϣ
class CNotifyUpdateItemInfo : public CMessageBody
{
public:
    CNotifyUpdateItemInfo();
    ~CNotifyUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int32_t  m_nDstUIN;         //Ŀ�����UIN
    TAccount m_szDstAccount;	//Ŀ�����account
    int16_t m_nItemCount;       //��������
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT]; //����ĵ�����Ϣ
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,

};


//service server ������ת������Ϣ(����state server message body�Ķ���....
class CNotifyItemDBTransferMessage : public CMessageBody
{
public:
	CNotifyItemDBTransferMessage();
	~CNotifyItemDBTransferMessage();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int32_t m_nSourceUin;
	int32_t m_nDestUin;
	int16_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];
protected:
private:
};


//SS_MSG_ITEMDB_GET_USER_ITEM_INFO
class CReqeustGetPlayerItemData:public CMessageBody
{
public:
	CReqeustGetPlayerItemData();
	~CReqeustGetPlayerItemData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	CReqeustGetPlayerItemData& operator=(const CReqeustGetPlayerItemData& obj) ;
	virtual void dump();
public:
	int32_t m_nUIN;

	//add by luozhen,2009-05-14
	int32_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];
};

class CResponseGetPlayerItemData:public CMessageBody
{
public:
	CResponseGetPlayerItemData();
	~CResponseGetPlayerItemData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

public:
	int32_t m_nUIN;
	int16_t m_nResultID;        //���ؽ��
	int16_t m_nItemCount;       //��������
	stItemInfo2 m_stItemInfos[MAX_ITEM_COUNT]; //����ĵ�����Ϣ

	int16_t m_nCharmItemCount;       //����(����)����
	stItemInfo2 m_stCharmItemInfos[MAX_ITEM_COUNT]; //�����������Ʒ��Ϣ

	//add by luozhen,2009-05-14
	int32_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];

};

//(GM)ɾ��ָ���ĵ���.
class CRequestDeleteItem:public CMessageBody
{
public:
	CRequestDeleteItem();
	~CRequestDeleteItem();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int16_t m_nItemCount;
	int32_t m_aryItemID[MAX_ITEM_COUNT];
	char trans_tag[max_game_tag_length];
};

class CResponseDeleteItem:public CMessageBody
{
public:
	CResponseDeleteItem();
	~CResponseDeleteItem();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int16_t m_nResultID;
	char trans_tag[max_game_tag_length];
};


//(GM)������ڵ���
class CRequestCleanExpireItem:public CMessageBody
{
public:
	CRequestCleanExpireItem();
	~CRequestCleanExpireItem();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	char trans_tag[max_game_tag_length];

};

class CResponseCleanExpireItem:public CMessageBody
{
public:
	CResponseCleanExpireItem();
	~CResponseCleanExpireItem();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int16_t m_nResultID;
	char trans_tag[max_game_tag_length];
};


//��ȡ���ָ���ĵ��߼�װ����Ϣ  add by luozhen,2009-06-11
class CRequestQueryPlayerItemData:public CMessageBody
{
public:
	CRequestQueryPlayerItemData();
	~CRequestQueryPlayerItemData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	 int32_t m_nUIN;
	 int32_t m_nSuperTypeCount ;
	 int32_t m_nSuperType[MAX_QUERY_COUNT];//ֵ��: enmMarketItemSuperType(����:item_super_type_goods)������ֵ.
	 int8_t m_bNeedEquipmentInfo; // 1=�Ƿ�װ����Ϣ
};

class CResponseQueryPlayerItemData:public CMessageBody
{
public:
	CResponseQueryPlayerItemData();
	~CResponseQueryPlayerItemData();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_nUIN;
	int16_t m_nResultID;

	int32_t m_nSuperTypeCount ;
	stSuperTypeInfo m_stSuperTypeInfo[MAX_QUERY_COUNT];

	int8_t m_cHaveEquipmentInfo;
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//ħ�������װ����Ϣ
};

//������ҵ�װ����Ϣ...add by luozhen,2009-06-11
class CRequestUpdatePlayerEquipmentInfo:public CMessageBody
{
public:
	CRequestUpdatePlayerEquipmentInfo();
	~CRequestUpdatePlayerEquipmentInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	//char m_szTransTag[max_game_tag_length];	
	int32_t m_nUIN;
	int16_t m_nEquipmentUpdateCount ;
	stEquipmentUpdate m_stEquipmentUpdate[MAX_EQUIPMENT_COUNT];

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size]; //��item DB͸��������

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������
};

class CResponseUpdatePlayerEquipmentInfo:public CMessageBody
{
public:
	CResponseUpdatePlayerEquipmentInfo();
	~CResponseUpdatePlayerEquipmentInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	//char m_szTransTag[max_game_tag_length];		
	int32_t m_nUIN;
	int16_t m_nResultID;
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//(ħ������)װ����Ϣ(���е�)

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size]; //��item DB͸��������
};

//֪ͨ�û�װ����Ϣ���� add by luozhen,2009-06-11
class CNotifyPlayerEquipmentInfo:public CMessageBody
{
public:
	CNotifyPlayerEquipmentInfo();
	~CNotifyPlayerEquipmentInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_nUIN;
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//(ħ������)װ����Ϣ(���е�)

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������
};
}} //namespace KingNet { namespace Server { 
#endif
