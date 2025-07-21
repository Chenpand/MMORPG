//#include "stdafx.h"
#include "Connector/udp_connector_head.h"


namespace KingNet { namespace Server {


	CUdpNetHead::CUdpNetHead()
	{
		m_iPackageSize = 0;
		m_cControlType = udp_head_type_client_request;
		m_unSocketIndex = 0;

		memset(&m_stControlInfo, 0, sizeof(m_stControlInfo));
	}

	CUdpNetHead::~CUdpNetHead()
	{
		//do nothing
	}

	int32_t CUdpNetHead::encode( char* pszCode, int32_t& iCodeSize )
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

		encode_length = CCodeEngine::encode_int16(&ptmp,m_unSocketIndex);
		iCodeSize += encode_length;

		switch( m_cControlType )
		{
		case udp_head_type_client_request :
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			iCodeSize += encode_length;

			encode_length = CCodeEngine::encode_int16(&ptmp,m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			iCodeSize += encode_length;

			break;

		case udp_head_type_service_notify :
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_stDestIPInfo.m_iDestIP);
			iCodeSize += encode_length;

			encode_length = CCodeEngine::encode_int16(&ptmp,m_stControlInfo.m_stDestIPInfo.m_nDestPort);
			iCodeSize += encode_length;

			break;
		}

		return success;
	}

	int32_t CUdpNetHead::decode( const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen/*=NULL*/ )
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

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_unSocketIndex);
		remian_length -= decode_length;
		switch( m_cControlType )
		{
		case udp_head_type_client_request :
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			remian_length -= decode_length;
			
			decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			remian_length -= decode_length;

			break;

		case udp_head_type_service_notify :
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_stDestIPInfo.m_iDestIP);
			remian_length -= decode_length;

			decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stControlInfo.m_stDestIPInfo.m_nDestPort);
			remian_length -= decode_length;

			break;
		}

		if(remian_length < 0)
		{
			return fail;
		}

		if(pHeadLen != NULL)
		{
			*pHeadLen = iCodeSize - remian_length;
		}	

		return success;
	}

	void CUdpNetHead::dump()
	{

	}

	int32_t CUdpNetHead::size()
	{
		int32_t iLength = sizeof(int32_t) + sizeof(int8_t) + sizeof(uint16_t);

		switch( m_cControlType )
		{
		case udp_head_type_client_request :
			iLength += sizeof(int32_t);
			iLength += sizeof(int16_t);

			break;

		case udp_head_type_service_notify:
			iLength += sizeof(int32_t);
			iLength += sizeof(int16_t);

			break;

		default :
			return 0;
		}

		return iLength;
	}

}}//namespace KingNet { namespace Server {
