#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/server_score_message_body.h"

namespace KingNet { namespace Server {

	CNotifyPlayer51Score::CNotifyPlayer51Score()
	{
		m_iMyUIN = 0;
		m_nDeltaScore = 0;
	}

	CNotifyPlayer51Score::~CNotifyPlayer51Score()
	{
		//do nothing
	}

	int32_t CNotifyPlayer51Score::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nDeltaScore);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyPlayer51Score::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDeltaScore);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyPlayer51Score::dump()
	{
		//do nothing
	}


	CNotifyMessageBox::CNotifyMessageBox()
	{
		m_iMyUIN = 0;
		m_nType = 0 ;
		m_nBuffSize = 0;
	}

	CNotifyMessageBox::~CNotifyMessageBox()
	{
		//do nothing
	}

	void CNotifyMessageBox::dump()
	{
		//do nothing
	}

	int32_t CNotifyMessageBox::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
		iOutLength += encode_length;
        
        encode_length = CCodeEngine::encode_int16(&ptmp, m_nType);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int16(&ptmp, m_nBuffSize);
		iOutLength += encode_length;

		if (m_nBuffSize > 0)
		{
			encode_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentBuff, m_nBuffSize);
			iOutLength += encode_length;
		}

		return success;
	}

	int32_t CNotifyMessageBox::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
		remain_length -= decode_length;
        
        decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nType);
		remain_length -= decode_length;
		
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nBuffSize);
		remain_length -= decode_length;

		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentBuff, m_nBuffSize);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}
	
	
	CNotifyLogin::CNotifyLogin()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0' ;
		m_iCount = 0 ;
	}

	CNotifyLogin::~CNotifyLogin()
	{
		//do nothing
	}

	int32_t CNotifyLogin::encode(char* pszOut, int32_t& iOutLength)
	{
        if(NULL == pszOut)
        {
            return fail;
        }


        char* ptmp = pszOut;

        iOutLength = 0;

   	    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iUin);
   	    iOutLength += CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
   	    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iCount);

	return success;
	}

	int32_t CNotifyLogin::decode(const char* pszIn, const int32_t iInLength)
	{
        static const int16_t min_size = 8 ;
        if(NULL == pszIn || iInLength < min_size )
        {
            return fail;
        }

  
        char* ptmp = const_cast<char*>(pszIn);

        CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
        CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
        CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iCount);
   	
	    return success;	
	}

	void CNotifyLogin::dump()
	{
		//do nothing
	}


	CNotifyMakeFriendCountInfo::CNotifyMakeFriendCountInfo()
	{
		m_iMyUIN	= 0;
		m_iCount	= 0;
		m_iLastTime	= 0;
	}

	CNotifyMakeFriendCountInfo::~CNotifyMakeFriendCountInfo()
	{
		//do nothing
	}

	void CNotifyMakeFriendCountInfo::dump()
	{
		//do nothing
	}

	int32_t CNotifyMakeFriendCountInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iCount);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iLastTime);
		iOutLength += encode_length;

		return success;
	}

	int32_t CNotifyMakeFriendCountInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCount);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLastTime);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

}}
