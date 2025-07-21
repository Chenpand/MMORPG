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
	int32_t m_iGameVipExpireTime;	//��ϷVIP�Ĺ���ʱ��(���û�п�ͨ����ʱ��Ϊ0)
	int32_t m_iGameVIPScore;	//��ϷVIP����
	int32_t m_iGameVIPLevel;	//��ϷVIP�ȼ�
	char m_szVIPTips[max_system_message_length]; //������������ϵ���ʾ��Ϣ
	int32_t m_nNextUpdateTime;      //�´������ľ���ʱ��
	int32_t m_nVipType;             // Vip����
	int32_t m_nLastBecomeVIPTime ;  //���һ�γ�ΪVIP��ʱ��.
	int16_t m_nNofifyTransparentDataSize;
	char m_szNofifyTransparentData[max_sub_message_size]; //notify͸������,
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
	//int32_t m_nServiceId;       //(VIP)����ID
 	//int32_t m_nDeltaExpireTime; //�仯��ʧЧʱ��,=0,��ʾ���ֲ���
	int32_t m_nDeltaScore; //�仯�Ļ��֣�=0,��ʾ�����ֲ���

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_sub_message_size]; //notify͸������,
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
	//int32_t m_nServiceId;       //(VIP)����ID
	int16_t m_nResultID;
	int32_t m_nVipType;             // Vip����
	int32_t m_iGameVipExpireTime;//��ϷVIP�Ĺ���ʱ��(���û�п�ͨ����ʱ��Ϊ0)
	int32_t m_iGameVIPScore;	//��ϷVIP����
	int32_t m_iGameVIPLevel;	//��ϷVIP�ȼ�
	int32_t m_nNextUpdateTime;      //�´������ľ���ʱ��

	int16_t m_nTransparentDataSize;
	char m_szTransparentData[max_sub_message_size]; //notify͸������,
};


}}
#endif
