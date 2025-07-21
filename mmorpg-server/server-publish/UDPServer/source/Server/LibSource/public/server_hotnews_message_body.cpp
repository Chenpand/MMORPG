#include "Common/common_codeengine.h"
#include "public/server_hotnews_message_body.h"

namespace KingNet { namespace Server {

	CNotifyPlayerHotNews::CNotifyPlayerHotNews()
	{
		m_iMyUIN = 0;
        memset(m_szAccount,0,sizeof(m_szAccount)) ;
        memset(m_szHotNews,0,sizeof(m_szHotNews)) ;
	}

	CNotifyPlayerHotNews::~CNotifyPlayerHotNews()
	{
		//do nothing
	}

	int32_t CNotifyPlayerHotNews::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szHotNews,sizeof(m_szHotNews));
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyPlayerHotNews::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szHotNews,sizeof(m_szHotNews));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyPlayerHotNews::dump()
	{
		//do nothing
	}

}}
