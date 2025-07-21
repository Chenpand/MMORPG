#ifndef __SERVER_VIPDB_MESSAGE_BODY_H__
#define __SERVER_VIPDB_MESSAGE_BODY_H__
#include "server_message.h"
namespace KingNet { namespace Server { 

//SS_MSG_GETGAMEDATA
class CRefreshVIPData : public CMessageBody
{
public:
	CRefreshVIPData();
	~CRefreshVIPData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();

	int32_t m_iUin;	
	char m_szAccount[max_account_string_length ];
	int32_t m_iGameVipExpireTime;	//游戏VIP的过期时间(如果没有开通过，时间为0)
	int32_t m_iGameVIPScore;	//游戏VIP积分
	int32_t m_iGameVIPLevel;	//游戏VIP等级
	char m_szVIPTips[max_system_message_length]; //个人资料面板上的提示信息
	int32_t m_nNextUpdateTime;      //下次升级的绝对时间
	int32_t m_nVipType;             // Vip类型
	int32_t m_nLastBecomeVIPTime ;  //最近一次成为VIP的时间.
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify透传数据,
protected:
private:
};
 
class CRequestUpdatePlayerVIPData: public CMessageBody
{
public:
	CRequestUpdatePlayerVIPData();
	~CRequestUpdatePlayerVIPData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int32_t m_iUin;	
	char m_szAccount[max_account_string_length ];

	char m_szTransTag[max_game_tag_length];	
	//int32_t m_nServiceId;       //(VIP)服务ID
 	//int32_t m_nDeltaExpireTime; //变化的失效时间,=0,表示保持不变
	int32_t m_nDeltaScore; //变化的积分，=0,表示保抚持不变

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_sub_message_size]; //notify透传数据,
};

class CResponseUpdatePlayerVIPData: public CMessageBody
{
public:
	CResponseUpdatePlayerVIPData();
	~CResponseUpdatePlayerVIPData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
	int32_t m_iUin;	
	char m_szAccount[max_account_string_length ];
	char m_szTransTag[max_game_tag_length];	
	//int32_t m_nServiceId;       //(VIP)服务ID
	int16_t m_nResultID;
	int32_t m_nVipType;             // Vip类型
	int32_t m_iGameVipExpireTime;//游戏VIP的过期时间(如果没有开通过，时间为0)
	int32_t m_iGameVIPScore;	//游戏VIP积分
	int32_t m_iGameVIPLevel;	//游戏VIP等级
	int32_t m_nNextUpdateTime;      //下次升级的绝对时间

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_sub_message_size]; //notify透传数据,
};


}}
#endif
