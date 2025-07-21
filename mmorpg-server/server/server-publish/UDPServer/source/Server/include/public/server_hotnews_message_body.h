#ifndef _HOTNEWS_SERVER_MESSAGE_BODY_H_
#define _HOTNEWS_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {
    static const int32_t MAX_HOTNEWS_LENGTH = 8192 ;
    typedef char THotNews[MAX_HOTNEWS_LENGTH];

	class CNotifyPlayerHotNews : public CMessageBody
	{
	public:
		CNotifyPlayerHotNews();
		~CNotifyPlayerHotNews();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iMyUIN;
        TAccount m_szAccount ;
        THotNews m_szHotNews ;
	};

}}

#endif //_HOTNEWS_SERVER_MESSAGE_BODY_H_

