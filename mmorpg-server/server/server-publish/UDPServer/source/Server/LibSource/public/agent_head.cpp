#include "public/agent_head.h"

namespace KingNet { namespace Server {

	CAgentHead::CAgentHead()
	{
		m_iPackageSize = 0;
		m_cControlType = agent_type_connect_alive;
		memset(m_szKeyName, 0, sizeof(m_szKeyName));
	}

	CAgentHead::~CAgentHead()
	{
		//do nothing
	}

	int32_t CAgentHead::encode( char* pszCode, int32_t& iCodeSize )
	{
		if(NULL == pszCode)
		{
			return fail;
		}
		iCodeSize = 0;
		char* ptmp = pszCode;
		int32_t encode_length = 0;
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iPackageSize);
		iCodeSize += encode_length;

		encode_length = CCodeEngine::encode_int8(&ptmp,m_cControlType);
		iCodeSize += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp,m_szKeyName,sizeof(m_szKeyName));
		iCodeSize += encode_length;
		m_shKeyLen = encode_length;

		return success;
	}

	int32_t CAgentHead::decode( const char *pszCode, const int32_t iCodeSize)
	{
		if(NULL == pszCode || iCodeSize < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszCode);
		int32_t decode_length = 0;
		int32_t remian_length = iCodeSize;
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iPackageSize);
		remian_length -= decode_length;

		if(m_iPackageSize < iCodeSize)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cControlType);
		remian_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp,m_szKeyName,sizeof(m_szKeyName));
		remian_length -= decode_length;
		m_shKeyLen = decode_length;

		if(remian_length < 0)
		{
			return fail;
		}
		return success;
	}

	void CAgentHead::dump()
	{

	}

	int32_t CAgentHead::size()
	{
		int32_t iLength = sizeof(int32_t) + sizeof(int8_t);
		iLength += m_shKeyLen;
		return iLength;
	}

}}//namespace KingNet { namespace Server {
