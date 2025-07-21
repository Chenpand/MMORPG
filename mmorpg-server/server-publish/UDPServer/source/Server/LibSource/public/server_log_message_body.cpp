#include "gameplatform_public.h"
#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/server_log_message_body.h"

namespace KingNet { namespace Server {  


	CCSNotifyWriteLog::CCSNotifyWriteLog()
	{
		initialize();
	}
	CCSNotifyWriteLog::~CCSNotifyWriteLog()
	{

	}
	int32_t CCSNotifyWriteLog::initialize()
	{
		m_iUid = -1;
		m_shTableType = -1;
		memset(m_szContentStr,0,sizeof(m_szContentStr));
		return success;
	}
	int32_t CCSNotifyWriteLog::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUid);
		iOutLength += encode_length;


		encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shTableType);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp,m_szContentStr,sizeof(m_szContentStr));
		iOutLength += encode_length;
		return success;
	}
	int32_t CCSNotifyWriteLog::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUid);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shTableType);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp,m_szContentStr,sizeof(m_szContentStr));
		remain_length -= decode_length;

		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CCSNotifyWriteLog::dump()
	{

	}
	CCSNotifyWriteLog& CCSNotifyWriteLog::operator =(const CCSNotifyWriteLog& original)
	{

		if (this == &original)
		{
			return *this;
		}
		m_iUid = original.m_iUid;
		m_shTableType = original.m_shTableType;
		STRNCPY(m_szContentStr, original.m_szContentStr, sizeof(m_szContentStr));
		return *this;
	}
}}

