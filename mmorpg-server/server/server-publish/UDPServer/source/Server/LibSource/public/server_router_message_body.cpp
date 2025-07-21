
#include "public/server_router_message_body.h"
#include "Common/common_codeengine.h"

namespace KingNet { namespace Server {

	


	CNotifyRouterHeartBeat::CNotifyRouterHeartBeat()
	{
		m_iServerAddr = 0;
		m_nServerPort = 0;
	}

	CNotifyRouterHeartBeat::~CNotifyRouterHeartBeat()
	{
		//do nothing
	}

	int32_t CNotifyRouterHeartBeat::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iServerAddr);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nServerPort);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CNotifyRouterHeartBeat::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerAddr);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nServerPort);
		remained_length -= decoded_length;

		return success;
	}

	void CNotifyRouterHeartBeat::dump()
	{

	}
}}//namespace KingNet { namespace Server {


