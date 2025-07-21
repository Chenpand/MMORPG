#ifndef __CS_ANTIBOT_MESSAGE_BODY_H__
#define __CS_ANTIBOT_MESSAGE_BODY_H__

#include "server_message.h"

namespace KingNet { namespace Server {

enum
{
    max_size_stream_length = 10240 , 
} ;

enum
{
    antibot_subtype_triger = 0x0001 ,
    antibot_subtype_data_check = 0x0002 ,
    antibot_subtype_data_result = 0x0003 ,
    antibot_subtype_active_report = 0x0004 ,
} ;

enum
{
    antibot_type_process    = 0x1 << 1 ,
    antibot_type_api        = 0x1 << 2 ,
    antibot_type_debug      = 0x1 << 3 ,
    antibot_type_rsa        = 0x1 << 4 ,
    antibot_type_snapshot   = 0x1 << 5 ,
    antibot_type_module     = 0x1 << 6 ,
    
} ;

class CCSAntibotSubHead:public CMessageBody
{
public:
	CCSAntibotSubHead();
	~CCSAntibotSubHead();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
    int32_t size() { return 2;} ;
	int16_t m_shMsgID;


};

//SS_MSG_NOTIFY_ANTIBOT
class CCSNotifyAntibot:public CMessageBody
{
public:
	CCSNotifyAntibot();
	~CCSNotifyAntibot();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:

	int32_t m_iUin;
	int32_t m_iMask;


};
//SS_MSG_CHECK_ANTIBOT
class CCSRequestAntibotCheck :public CMessageBody
{
public:
	CCSRequestAntibotCheck();
	~CCSRequestAntibotCheck();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:

	int32_t m_iUin;
    int32_t m_iSeq ;
    int32_t  m_iType ;
    int16_t m_shStreamLength ;
    char    m_szStreamContent[max_size_stream_length] ;

};
//SS_MSG_CHECK_ANTIBOT
class CCSResponseAntibotCheck :public CMessageBody
{
public:
	CCSResponseAntibotCheck();
	~CCSResponseAntibotCheck();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:
	int32_t m_iUin;
    int32_t m_iSeq ;
    int32_t m_iResult ;
    int32_t  m_iType ;
    int16_t m_shStreamLength ;
    char    m_szStreamContent[max_size_stream_length] ;

};

}}//namespace KingNet { namespace Server {



#endif //__SERVER_ANTIBOT_MESSAGE_BODY_H__

