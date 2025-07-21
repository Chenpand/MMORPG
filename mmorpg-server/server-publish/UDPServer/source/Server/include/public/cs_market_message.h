/**
@file   cs_market_message.h
@brief  定义了fastcgi和market server通讯的包头和包体结构

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/

#ifndef _CS_MARKET_MESSAGE_H_
#define _CS_MARKET_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"


#ifndef MAX_EQUIPMENT_COUNT
#define MAX_EQUIPMENT_COUNT 16
#endif


using namespace KingNet::Server;

namespace KingNet { namespace Server {

//const int32_t MAX_CS_FILE_CONTENT_LENGTH = 65536;

const int32_t MAX_COMMODITY_SIZE = 6;
const int32_t MAX_FAILED_MESSAGE_LENGTH = 256;
const int32_t MAX_TRANSPARENT_LENGTH = 256 ;
const int32_t MAX_WORD_LENGTH = 512 ;
const int16_t MAX_USER_WORD_LENGTH = 600;
const int16_t MAX_ONLINE_RANK = 300 ;


struct stCSCommodityData
{
	int32_t m_iID;   //ID
	int16_t m_shCount;
};

struct stOnlineRank
{
	int32_t m_iUin ;
	TAccount m_szAccount;
	int32_t m_iRank ;
} ;

enum enmPayMode //支付方式
{
	pay_mode_yxb	= 0,			//游戏币支付
	pay_mode_51b	= 1,			//51币支付
	pay_mode_misc	= 2,			//其他支付手段
	pay_mode_bean   = 3,            //欢乐豆支付
};

enum enmMarketServiceTag
{
	service_tag_game_client		= 0,	//游戏client
	service_tag_game_webmarket	= 1,    //游戏内web商城
	service_tag_web_center		= 2,	//51网站
	service_tag_misc			= 3,	//从其他入口购买
};

	/**	
	@brief  购买/赠送物品请求协议
	*/
	class  CCSRequestBuyCommodity:public CMessageBody
	{
	public:
		CCSRequestBuyCommodity();
		~CCSRequestBuyCommodity();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		//CCSRequestBuyCommodity& operator = (const CCSRequestBuyCommodity& original);

	public:
		
		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;
        TNick    m_szSrcNickname ;
        int8_t   m_cSrcGender;
		int8_t   m_cVipLevel;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		int16_t m_shPaymentMode;
		int32_t m_iClientIP;
		int32_t m_iCommodityPrice ;
		int16_t m_shCommodityCount;
		stCSCommodityData m_CommodityData[MAX_COMMODITY_SIZE];

		char m_szWord[MAX_WORD_LENGTH];
		
	    int16_t m_nTransparentDataSize;
	    char m_szTransparentData[MAX_TRANSPARENT_LENGTH];
	private:
	};

	
	/**	
	@brief  购买/赠送物品响应协议 
	*/
	class  CCSResponseBuyCommodity:public CMessageBody
	{
	public:
		CCSResponseBuyCommodity();
		~CCSResponseBuyCommodity();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:
		int16_t m_shResultID;

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;
		
        int32_t m_iPrice ;
        int16_t m_shCommodityCount ;
        int64_t m_iBean ;
		int16_t m_nBeanFlag;
        int16_t m_shItemCount;
		stCSCommodityData m_ItemInfo[MAX_COMMODITY_SIZE];

	    int16_t m_nTransparentDataSize;
	    char m_szTransparentData[MAX_TRANSPARENT_LENGTH];
	    		
	private:
	};	

    /**	
	@brief  魅力物品回兑 
	*/
	class  CCSRequestItemExchange:public CMessageBody
	{
	public:
		CCSRequestItemExchange();
		~CCSRequestItemExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestItemExchange& operator = (const CCSRequestItemExchange& original);

	public:
		
		int32_t  m_iUin;
		TAccount m_szAccount;

		int16_t m_shItemCount;
		int32_t m_iItemID ;
		int32_t m_iClientIP ;

	private:
	};

	class  CCSResponseItemExchange:public CMessageBody
	{
	public:
		CCSResponseItemExchange();
		~CCSResponseItemExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:
		int16_t m_shResultID;

		int32_t  m_iUin;
		TAccount m_szAccount;

		int32_t m_iGameMoney ;
		
	private:
	};	
	
class CCSRequestRenewService:public CMessageBody
{
public:
	CCSRequestRenewService();
	~CCSRequestRenewService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
	int32_t m_iClientIP ;
	int32_t m_iRenewID ;
    int8_t   m_cVipLevel;
};
class CCSResponseRenewService:public CMessageBody
{
public:
	CCSResponseRenewService();
	~CCSResponseRenewService();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	
	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_nResultID;
	int32_t m_iServiceID ;
	int32_t m_iRenewID ;
	int32_t m_iPrice ;
	int64_t m_iBean ;
	
};	


class CCSRequestLuckyDraw:public CMessageBody
{
public:
	CCSRequestLuckyDraw();
	~CCSRequestLuckyDraw();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
	int32_t m_iChestID ;
	int32_t m_iClientIP ;
};
class CCSResponseLuckyDraw:public CMessageBody
{
public:
	CCSResponseLuckyDraw();
	~CCSResponseLuckyDraw();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	
	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_shResultID;
	int32_t m_iChestID ;
	int16_t m_shChestCount ;
	int32_t m_iMoney ;
	int32_t m_iBean ;
	int32_t m_iItemID ;
	int16_t m_shItemCount ;
	
};	


class CCSRequestGetActionInfo:public CMessageBody
{
public:
	CCSRequestGetActionInfo();
	~CCSRequestGetActionInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
	int32_t m_iClientIP ;
};
class CCSResponseGetActionInfo:public CMessageBody
{
public:
	CCSResponseGetActionInfo();
	~CCSResponseGetActionInfo();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	
	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_shResultID;
	int32_t m_iOnlineSeconds ; 
	int16_t m_shMaxHit ;       
	int16_t m_shCurHit ;
	int32_t m_iMoney ;
	int32_t m_iBean ;
	int32_t m_iScore ;
	int32_t m_iItemID ;
	int16_t m_shItemCount ;
	
};	

class CCSRequestGetActionOnline:public CMessageBody
{
public:
	CCSRequestGetActionOnline();
	~CCSRequestGetActionOnline();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
};

class CCSResponseGetActionOnline:public CMessageBody
{
public:
	CCSResponseGetActionOnline();
	~CCSResponseGetActionOnline();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_shResultID;
	int32_t m_today_value ;
	int32_t m_total_value ;
	int16_t m_hummer_count ;
	int32_t m_ranking ;
};

class CCSRequestGetActionOnlineRank:public CMessageBody
{
public:
	CCSRequestGetActionOnlineRank();
	~CCSRequestGetActionOnlineRank();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:

};

class CCSResponseGetActionOnlineRank:public CMessageBody
{
public:
	CCSResponseGetActionOnlineRank();
	~CCSResponseGetActionOnlineRank();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	int16_t m_shResultID;
	int16_t m_iCountRank ;
	stOnlineRank m_szRank[MAX_ONLINE_RANK] ;
};


class CCSRequestBalance:public CMessageBody
{
public:
	CCSRequestBalance();
	~CCSRequestBalance();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
	int32_t m_iFlag ;
	int32_t m_iClientIP ;
};

class CCSResponseBalance:public CMessageBody
{
public:
	CCSResponseBalance();
	~CCSResponseBalance();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	
	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_shResultID;
	int32_t m_iMoney ;
	int32_t m_iBean ;
	int32_t m_iItemID ;
	int16_t m_shItemCount ;
	int32_t m_iCardID ;
	int32_t m_iChestID ;
	
};	

typedef CCSRequestLuckyDraw CCSRequestQuery ;
typedef CCSResponseLuckyDraw CCSResponseQuery ;

class CCSRequestActionHit:public CMessageBody
{
public:
	CCSRequestActionHit();
	~CCSRequestActionHit();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;
public:
	int32_t	 m_iUin;
	TAccount m_szAccount;
	int32_t m_iHit;
	int32_t m_iClientIP ;
};

class CCSResponseActionHit:public CMessageBody
{
public:
	CCSResponseActionHit();
	~CCSResponseActionHit();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
	virtual void dump() ;

	
	int32_t m_iUin;
	TAccount m_szAccount;
	int16_t m_shResultID;
	int32_t m_iMoney ;
	int32_t m_iBean ;
	int32_t m_iScore ;
	int32_t m_iItemID ;
	int16_t m_shItemCount ;
	
};


class CCSRequestUpdateEquipment:public CMessageBody
{
public:
	CCSRequestUpdateEquipment();
	~CCSRequestUpdateEquipment();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:	
	int32_t m_iUin;
	int16_t m_nEquipmentUpdateCount ;
	struct 
	{
	    int32_t m_iItemID ;
	    int8_t m_cPosition ;
	} m_stEquipmentUpdate[MAX_EQUIPMENT_COUNT];

};
class CCSResponseUpdateEquipment:public CMessageBody
{
public:
	CCSResponseUpdateEquipment();
	~CCSResponseUpdateEquipment();
	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	//char m_szTransTag[max_game_tag_length];		
	int32_t m_iUin;
	int16_t m_nResultID;
	int32_t m_aryEquipmentInfo[MAX_EQUIPMENT_COUNT];//(魔法表情)装备信息(所有的)
};



	/**	
	@brief  发货请求协议
	*/
	class  CCSRequestConsign:public CMessageBody
	{
	public:
		CCSRequestConsign();
		~CCSRequestConsign();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:
		
		int32_t  m_iUin;
		TAccount m_szAccount;
        int32_t m_iConsignType ;    //发货类型
		int32_t m_iConsignID ;      //发货ID
		int32_t m_iConsignCount ;
		int32_t m_iClientIP;
		
	};

	
	/**	
	@brief  发货响应协议 
	*/
	class  CCSResponseConsign:public CMessageBody
	{
	public:
		CCSResponseConsign();
		~CCSResponseConsign();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t  m_iUin;
		TAccount m_szAccount;
		int16_t m_shResultID;		
		int32_t m_iConsignID ;      //发货ID
		int32_t m_iConsignCount ;
	};	

}}//namespace KingNet { namespace Server

#endif

