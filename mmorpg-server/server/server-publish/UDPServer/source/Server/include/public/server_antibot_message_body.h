#ifndef __SERVER_ANTIBOT_MESSAGE_BODY_H__
#define __SERVER_ANTIBOT_MESSAGE_BODY_H__

#include "server_message.h"

namespace KingNet { namespace Server {

enum
{
    max_size_antibot_length = 4096 , 
} ;

enum
{
    punish_subtype_freeze = 0x0001 ,
    punish_subtype_kick = 0x0002 ,
    punish_subtype_stop = 0x0003 ,

} ;
enum
{
    punish_mode_delta = 0x0001 , 
    punish_mode_all  = 0x0002 ,
} ;


//SS_MSG_NOTIFY_ANTIBOT
class CNotifyAntibotData:public CMessageBody
{
public:
	CNotifyAntibotData();
	~CNotifyAntibotData();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:

	int32_t m_iUin;
	TAccount m_szAccount ;
    int32_t m_iStreamLength ;
    char    m_szStreamContent[max_size_antibot_length] ;


};

class CNotifyPunish:public CMessageBody
{
public:
	CNotifyPunish();
	~CNotifyPunish();

	virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
	virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
	virtual void dump();
public:

	int32_t m_iUin;
	TAccount m_szAccount ;
    int16_t m_shSubType ;
    int16_t m_shMode ;
    int32_t m_iValue ;
};

}}//namespace KingNet { namespace Server {



#endif //__SERVER_ANTIBOT_MESSAGE_BODY_H__

