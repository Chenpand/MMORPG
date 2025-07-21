/**
@file   cs_pay_message.h
@brief  定义了pay.51.com php 和market server通讯的包头和包体结构

@author sjij@mail.51.com
@date Begin 2008.9
@date Dnd 2008.9
*/

#ifndef _CS_PAY_MESSAGE_H_
#define _CS_PAY_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"

using namespace KingNet::Server;

namespace KingNet { namespace Server {

const int32_t MAX_ORDER_ID_LEN = 64;
const int32_t MAX_TOKEN_LEN = 128;
const int32_t MAX_VIP_TYPE_LEN = 10;
const int32_t MAX_VIP_ADDITIONAL_LEN = 1024;

	/**	
	@brief  pay.51.com发送的充值请求协议
	*/
	class  CCSRequestPayToGame:public CMessageBody
	{
	public:
		CCSRequestPayToGame();
		~CCSRequestPayToGame();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestPayToGame& operator = (const CCSRequestPayToGame& original);

	public:
		
		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iChannel;
		int32_t m_iAmount;
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;
		
	private:
	};

	
	/**	
	@brief  pay.51.com发送的充值响应协议
	*/
	class  CCSResponsePayToGame:public CMessageBody
	{
	public:
		CCSResponsePayToGame();
		~CCSResponsePayToGame();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iChannel;
		int32_t m_iAmount;
		int32_t m_iPayamount;
		int32_t m_iMoney;   //更新后的当前游戏币的数量

		int32_t m_iRequestTimestamp;
		
		char m_szGameSequenceID[MAX_ORDER_ID_LEN];
		int32_t m_iResponseTimestamp;

	};	

	/**	
	@brief  pay.51.com发送的兑换请求协议
	*/
	class  CCSRequestPayExchange:public CMessageBody
	{
	public:
		CCSRequestPayExchange();
		~CCSRequestPayExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestPayExchange& operator = (const CCSRequestPayExchange& original);

	public:

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iExcid;  //兑换比率数据记录的id
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;

	private:
	};


	/**	
	@brief  pay.51.com发送的兑换响应协议
	*/
	class  CCSResponsePayExchange:public CMessageBody
	{
	public:
		CCSResponsePayExchange();
		~CCSResponsePayExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iExcid;     //兑换比率数据记录的id
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;

		char m_szGameSequenceID[MAX_ORDER_ID_LEN];
		int32_t m_iResponseTimestamp;
	};


	/**	
	@brief  查询游戏币请求协议
	*/
	class  CCSRequestSearchGameMoney:public CMessageBody
	{
	public:
		CCSRequestSearchGameMoney();
		~CCSRequestSearchGameMoney();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestSearchGameMoney& operator = (const CCSRequestSearchGameMoney& original);

	public:

		int32_t  m_iSrcUin;
	
	private:
	};


	/**	
	@brief  pay.51.com发送的查询游戏币响应协议
	*/
	class  CCSResponseSearchGameMoney:public CMessageBody
	{
	public:
		CCSResponseSearchGameMoney();
		~CCSResponseSearchGameMoney();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;
		int32_t  m_iSrcUin;		
		int32_t  m_iCoin;
	};

	/**
	@brief  pay.51.com发送的查询用户是否为VIP请求协议
	*/
	class CCSRequestSearchVipInfo:public CMessageBody
	{
	public:
		CCSRequestSearchVipInfo();
		~CCSRequestSearchVipInfo();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t m_iSrcUin;
		//TAccount m_szSrcAccount;
	};

	/**
	 @brief 查询用户是否为VIP回复
	 */
	class CCSResponseSearchVipInfo:public CMessageBody
	{
	public:
		CCSResponseSearchVipInfo();
		~CCSResponseSearchVipInfo();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t m_iSrcUin;
		//TAccount m_szSrcAccount;
		int16_t m_shResultID;
		char m_szVipType[MAX_VIP_TYPE_LEN];  //vip类型
		int32_t m_iEndTime;                  //vip结束时间

	};

	/**
	 @brief pay.51.com给用户加普通蓝钻请求
	 */
	class CCSRequestPayToVip:public CMessageBody
	{
	public:
		CCSRequestPayToVip();
		~CCSRequestPayToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];   //流水号
		int32_t m_iSrcUin;                    //发起UIN
		TAccount m_szSrcAccount;              //发起account
		int32_t m_iRequestTime;               //发起请求时的时间戳
		int32_t m_iOperUin;                   //受益UIN
		TAccount m_szOperAccount;             //受益account
		int32_t m_iAmount;                    //充值扣费金额
		int32_t m_iChannelID;                 //渠道ID
		int32_t m_iComboID;                   //套餐ID，主要是充值中心的
		int32_t m_iGoodsID;                   //活动ID，包含是否赠送给用户相关物品或服务
		int32_t m_iAddTime;                   //给用户加VIP的时长，增量值
		int32_t m_iVipType;                   //用户的vip类型
		int32_t m_iAddVipInt;                 //附加字段，供扩展用途
		char m_szAddVipStr[MAX_VIP_ADDITIONAL_LEN];         //附加字段，供扩展用途
	};
	
	/**
	 @brief 给用户加蓝钻回复
	 */
	class CCSResponsePayToVip:public CMessageBody
	{
	public:
		CCSResponsePayToVip();
		~CCSResponsePayToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];     //php发送过来的流水号
		int32_t m_iSrcUin;                      //操作的uin
		TAccount m_szSrcAccount;                //操作的account
		int32_t m_iOperUin;						//受益的uin
		TAccount m_szOperAccount;				//受益的account
		int16_t m_shResultID;					//返回结果
		char m_szTransTag[MAX_ORDER_ID_LEN];	//server生成的一个流水号
		int16_t m_shActResultID;				//赠送物品结果
		int32_t m_iVipEndTime;					//VIP失效时间

	};

	/**
	 @brief  手机包月蓝钻
	 */
	class CCSRequestPayMobileToVip:public CMessageBody
	{
	public:
		CCSRequestPayMobileToVip();
		~CCSRequestPayMobileToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];   //流水号
		int32_t m_iSrcUin;                    //发起UIN
		TAccount m_szSrcAccount;              //发起account
		int32_t m_iRequestTime;               //发起请求时的时间戳
		int32_t m_iChannelID;                 //渠道ID
		int32_t m_iComboID;                   //套餐ID，主要是充值中心的
		int32_t m_iGoodsID;                   //活动ID，包含是否赠送给用户相关物品或服务
		char m_szMobile[MAX_ORDER_ID_LEN];    //绑定手机号码
		int32_t m_iAddVipInt;                 //附加字段，供扩展用途
		char m_szAddVipStr[MAX_VIP_ADDITIONAL_LEN];         //附加字段，供扩展用途

	};

	/************************************************************************/
	/* @brief  手机包月蓝钻回复                                             */
	/************************************************************************/
	class CCSResponsePayMobileToVip:public CMessageBody
	{
	public:
		CCSResponsePayMobileToVip();
		~CCSResponsePayMobileToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];             //php生成的流水号
		int32_t m_iSrcUin;								//发起UIN
		TAccount m_szSrcAccount;						//发起account
		int16_t m_shResultID;							//返回结果
		char m_szTransTag[MAX_ORDER_ID_LEN];			//server生成的全局唯一流水号
		int16_t m_shActResultID;					    //活动返回的ID
		//int32_t m_iGoodsID;							//活动ID
	};

	/************************************************************************/
	/* @brief 手机包月退订VIP请求                                           */
	/************************************************************************/
	class CCSRequestPayUnsubscriptVip:public CMessageBody
	{
	public:
		CCSRequestPayUnsubscriptVip();
		~CCSRequestPayUnsubscriptVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];		//php前端生成的流水号
		int32_t m_iSrcUin;						//发起用户UIN
		TAccount m_szSrcAccount;				//发起用户Account
		int32_t m_iRequestTime;					//发起请求时间
		char m_szMobile[MAX_ORDER_ID_LEN];		//手机号

	};

	/************************************************************************/
	/* @brief 手机包月退订VIP回复                                           */
	/************************************************************************/
	class CCSResponsePayUnsubscriptVip:public CMessageBody
	{
	public:
		CCSResponsePayUnsubscriptVip();
		~CCSResponsePayUnsubscriptVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];		//流水号
		int32_t m_iSrcUin;						//发起用户UIN
		TAccount m_szSrcAccount;				//发起用户Account
		int16_t m_shResultID;					//返回结果
		char m_szTransTag[MAX_ORDER_ID_LEN];	//server生成流水号
	};


}}//namespace KingNet { namespace Server

#endif

