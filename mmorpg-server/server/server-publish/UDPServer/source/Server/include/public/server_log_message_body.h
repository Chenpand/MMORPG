#ifndef _SERVER_LOG_MESSAGE_BODY_H_
#define _SERVER_LOG_MESSAGE_BODY_H_

#include "Common/common_base.h"  // for (fail/sucess)µ»∂®“Â
#include "public/server_message.h"

namespace KingNet { namespace Server {
	
	class CCSNotifyWriteLog : public CMessageBody
	{
		public:
			CCSNotifyWriteLog();
			~CCSNotifyWriteLog();
			virtual int32_t encode(char* pszOut, int32_t& iOutLength);
			virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
			virtual void dump() ;

			CCSNotifyWriteLog& operator = (const CCSNotifyWriteLog& original);
			int32_t initialize();
		public:
			int32_t m_iUid;
			int16_t m_shTableType;
			char	m_szContentStr[max_string_message_length];
	};
	

}}//namespace KingNet { namespace Server 

#endif


