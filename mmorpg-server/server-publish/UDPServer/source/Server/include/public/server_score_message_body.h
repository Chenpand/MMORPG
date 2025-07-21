#ifndef _SCORE_SERVER_MESSAGE_BODY_H_
#define _SCORE_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

	class CNotifyPlayer51Score : public CMessageBody
	{
	public:
		CNotifyPlayer51Score();
		~CNotifyPlayer51Score();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iMyUIN;
		int16_t m_nDeltaScore;
	};
	
	enum
	{
	    notify_message_type_game = 1 ,
	    notify_message_type_hall = 2 ,
	} ;

	class CNotifyMessageBox : public CMessageBody
	{
	public:
		CNotifyMessageBox();
		~CNotifyMessageBox();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iMyUIN;
		int16_t m_nType ;
		int16_t m_nBuffSize;
		char	m_szTransparentBuff[max_transparent_data_size];
	};
	
    class CNotifyLogin : public CMessageBody
	{
	public:
		CNotifyLogin();
		~CNotifyLogin();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iUin ;
		TAccount m_szAccount ;
		int32_t m_iCount ;
	};

	//暂时借用这块宝地！
	class CNotifyMakeFriendCountInfo : public CMessageBody
	{
	public:
		CNotifyMakeFriendCountInfo();
		~CNotifyMakeFriendCountInfo();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iMyUIN;
		int32_t m_iCount;
		int32_t m_iLastTime;
	};

}}

#endif //_SCORE_SERVER_MESSAGE_BODY_H_

