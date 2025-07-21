#ifndef __SERVER_ITEMDB_MESSAGE_BODY_H__
#define __SERVER_ITEMDB_MESSAGE_BODY_H__
#include "server_message.h"
#include "server_item_define.h"
#include "server_state_message_body.h"
namespace KingNet { namespace Server { 


//道具种类的count，和打包道具一致起来
#define MAX_GOODS_UPDATE_COUNT	15
#define MAX_ITEM_UPDATE_COUNT 	(MAX_GOODS_UPDATE_COUNT * max_item_count_in_goods)   
#define MAX_ITEM_COUNT 512
#define MAX_EQUIPMENT_COUNT 16 //用户能装备的(魔法)表情的最大的个数 add by luozhen,2009-06-11

#define MAX_QUERY_COUNT 8


//道具更新模式
enum enmItemDataUpdateMode //以下枚举值的变化需要和logicserver_itablegame.h 中的enmItemUpdateMode保持一致
{
    ITEM_DATA_UPDATE_MODE_DEFAULT   = 0,    //空操作，不进行数据的更新，只是记录账单
    ITEM_DATA_UPDATE_MODE_DELTA     = 1,    //增量更新
    ITEM_DATA_UPDATE_MODE_ALL       = 2,    //全量更新
};

struct stItemInfo
{
    int32_t m_nItemID;      //道具ID
    int16_t m_nItemType;    //道具类型，枚举值见enmItemType
    int32_t m_iCount;       //道具数量
    int16_t m_nGameID;      //道具使用的场景（游戏ID）
    int32_t m_nExpiredTime;  //道具的失效时间
	int8_t  m_cUpdateMode;      //更新模式，见enmItemDataUpdateMode
};


struct stItemInfo2
{
	int32_t m_nItemID;      //道具ID
	int32_t m_iCount;       //道具数量
	int32_t m_nExpiredTime;  //道具的失效时间
};

struct stGoodsInfo
{
	int32_t m_nGoodsID;	//商品ID
	int32_t m_nCount;	//商品数量
	int32_t m_nPrice;	//商品价格	
};

struct stSuperTypeInfo
{
	int32_t m_nItemSuperType;
	int32_t m_nItemCount;       //道具数量
	stItemInfo2 m_stItemInfos[MAX_ITEM_COUNT]; //具体的道具信息
};
// 装备更新方式:add by luozhen,2009-06-11
enum enmEquipmentUpdateMode
{
	Equipment_Update_Mode_must = 0, //必须,不论该位置上是否有该道具.
	Equipment_Update_Mode_if = 1, // 如果该位置上无有效的道具,即用当前的更新的道具，否则跳过更新.
	Equipment_Update_Mode_Expiretime = 2, //超时清理.
};
//装备更新单元
struct stEquipmentUpdate
{
	int32_t m_nEquipmentPosi; //装备位,即装备数组的下标
	int32_t m_nItemId;       //装备的道具ID
	int8_t m_nEquipmentUpdateMod; //装备更新的方式,Equipment_Update_Mode_must 是缺省
};

int32_t encode_item_info(char** pszOut, stItemInfo& item_info);
int32_t decode_item_info(char** pszIn, stItemInfo& item_info);

int32_t encode_item_info2(char** pszOut, stItemInfo2& item_info);
int32_t decode_item_info2(char** pszIn, stItemInfo2& item_info);


int32_t encode_goods_info(char** pszOut, stGoodsInfo& good_info);
int32_t decode_goods_info(char** pszIn,  stGoodsInfo&  good_info);

int32_t encode_EquipmentUpdate(char** pszOut, stEquipmentUpdate& equipment);
int32_t decode_EquipmentUpdate(char** pszOut, stEquipmentUpdate& equipment);
 
//道具更新请求hall --> item db
class CResquestUpdateItemInfo : public CMessageBody
{
public:
    CResquestUpdateItemInfo();
    ~CResquestUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int32_t  m_nRealUIN;        //实际要进行更新物品信息的玩家UIN
    TAccount m_szRealAccount;	//实际要进行更新物品信息的玩家account
    int32_t	 m_nActorUIN;       //引发此次更新操作的玩家UIN，如果是自己买东西给自己，则m_nRealUIN和m_nAcotrUIN都是自己
    TAccount m_szActorAccount;	//引发此次更新操作的玩家account
	int32_t  m_nServiceTag;     //业务标识,由业务类型(高两个字节,具体值域参见 server_public_define.h里的enmServiceTagType的定义)+子业务类型(低两个字节,具体值域参见server_public_define.h的enmServiceTagXXX的定义)组成.
    int32_t  m_nSourceTag;		//引起业务的源,由源的类型(高两个字节,具体值域定义参见 server_public_define.h里enmUpdateItemInfoSourceType定义)+源的子类型(低两个字节组成,如果'源类型=update_item_source_type_game',则此值GameID,其它则为0).
    int32_t  m_nIP;             //ip地址信息

    int16_t  m_nItemCount;	//道具数量
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT];	//具体的道具信息
    
    int16_t m_nGoodsCount;	//商品数量
    stGoodsInfo m_stGoodsInfo[MAX_GOODS_UPDATE_COUNT];	//商品的详细信息..

    int16_t m_nTransparentDataSize;
    char m_szTransparentData[max_transparent_data_size]; //对item DB透明的数据

    char m_szOperateDescription[max_operate_description_length];
    char m_szTransTag[max_game_tag_length];		

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据

};

//道具更新应答 item db --> hall server
class CResponseUpdateItemInfo : public CMessageBody
{
public:
    CResponseUpdateItemInfo();
    ~CResponseUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int16_t  m_nResultID;       //返回结果
    int32_t  m_nRealUIN;        //实际要进行更新物品信息的玩家UIN
    TAccount m_szRealAccount;	//实际要进行更新物品信息的玩家account
    int32_t  m_nActorUIN;       //引发此次更新操作的玩家UIN，如果是自己买东西给自己，则m_nRealUIN和m_nAcotrUIN都是自己
    TAccount m_szActorAccount;	//引发此次更新操作的玩家account

    int16_t m_nItemCount;	//道具数量
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT];	//具体的道具信息

    int16_t m_nTransparentDataSize;
    char m_szTransparentData[max_transparent_data_size];	//对item DB透明的数据

    char m_szReasonMsg[max_reason_message_length];
    char m_szTransTag[max_game_tag_length];		
};


//notify消息，通知(刷新)玩家物品信息
class CNotifyUpdateItemInfo : public CMessageBody
{
public:
    CNotifyUpdateItemInfo();
    ~CNotifyUpdateItemInfo();

    virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
    virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
    virtual void dump();

public:
    int32_t  m_nDstUIN;         //目标玩家UIN
    TAccount m_szDstAccount;	//目标玩家account
    int16_t m_nItemCount;       //道具数量
    stItemInfo m_stItemInfos[MAX_ITEM_UPDATE_COUNT]; //具体的道具信息
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,

};


//service server 请求中转道具消息(借用state server message body的定义....
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
	int16_t m_nResultID;        //返回结果
	int16_t m_nItemCount;       //道具数量
	stItemInfo2 m_stItemInfos[MAX_ITEM_COUNT]; //具体的道具信息

	int16_t m_nCharmItemCount;       //魅力(道具)数量
	stItemInfo2 m_stCharmItemInfos[MAX_ITEM_COUNT]; //具体的魅力物品信息

	//add by luozhen,2009-05-14
	int32_t m_nDataSize;
	char m_szTransparentData[max_transparent_data_size];

};

//(GM)删除指定的道具.
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


//(GM)清理过期道具
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


//拉取玩家指定的道具及装备信息  add by luozhen,2009-06-11
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
	 int32_t m_nSuperType[MAX_QUERY_COUNT];//值域: enmMarketItemSuperType(除了:item_super_type_goods)的所有值.
	 int8_t m_bNeedEquipmentInfo; // 1=是否装备信息
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
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//魔法表情的装备信息
};

//更新玩家的装备信息...add by luozhen,2009-06-11
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
	char m_szTransparentData[max_transparent_data_size]; //对item DB透明的数据

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据
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
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//(魔法表情)装备信息(所有的)

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_transparent_data_size]; //对item DB透明的数据
};

//通知用户装备信息更新 add by luozhen,2009-06-11
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
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//(魔法表情)装备信息(所有的)

	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据
};
}} //namespace KingNet { namespace Server { 
#endif
