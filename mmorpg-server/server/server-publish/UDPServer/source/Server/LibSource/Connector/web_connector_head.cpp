//#include "stdafx.h"
#include "Connector/web_connector_head.h"


namespace KingNet { namespace Server {





	CWebNetHead::CWebNetHead()
	{
		m_iPackageSize = 0;
		m_cControlType = head_type_start_request;
		m_unConnectionIndex = 0;

		memset(&m_stControlInfo, 0, sizeof(m_stControlInfo));
	}

	CWebNetHead::~CWebNetHead()
	{
		//do nothing
	}

	int32_t CWebNetHead::encode( char* pszCode, int32_t& iCodeSize )
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

		encode_length = CCodeEngine::encode_int16(&ptmp,m_unConnectionIndex);
		iCodeSize += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iUniqueID);
		iCodeSize += encode_length;

		switch( m_cControlType )
		{
		case head_type_start_request :
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			iCodeSize += encode_length;
			encode_length = CCodeEngine::encode_int16(&ptmp,m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			iCodeSize += encode_length;
			break;
		case head_type_transform_request :
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_iTime);
			iCodeSize += encode_length;
			break;
		case head_type_force_close_connection:
			//encode_length = CCodeEngine::encode_int8(&ptmp,m_stControlInfo.m_cDisconnectType);
			encode_length = CCodeEngine::encode_int8(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType);
			iCodeSize += encode_length;

			encode_length = CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
			iCodeSize += encode_length;

			encode_length = CCodeEngine::encode_int16(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_nSourcePort);
			iCodeSize += encode_length;

			break;
		case head_type_disconnect_notify :
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_stClientDisconnect.m_iUin);
			iCodeSize += encode_length;
			encode_length = CCodeEngine::encode_int8(&ptmp,m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
			iCodeSize += encode_length;
			break;
		}
		return success;
	}

	int32_t CWebNetHead::decode( const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen/*=NULL*/ )
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

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_unConnectionIndex);
		remian_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUniqueID);
		remian_length -= decode_length;

		switch( m_cControlType )
		{
		case head_type_start_request :
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			remian_length -= decode_length;
			decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			remian_length -= decode_length;
			break;
		case head_type_transform_request :
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_iTime);
			remian_length -= decode_length;
			break;
		case head_type_force_close_connection:
			//decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stControlInfo.m_cDisconnectType);
			decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType);
			remian_length -= decode_length;

			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
			remian_length -= decode_length;

			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stControlInfo.m_stDisconnectInfo.m_nSourcePort);
			remian_length -= decode_length;

			break;
		case head_type_disconnect_notify :
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_stClientDisconnect.m_iUin);
			remian_length -= decode_length;
			decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
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

	void CWebNetHead::dump()
	{

	}

	int32_t CWebNetHead::size()
	{
		int32_t iLength = web_connector_head_base_size ;
		switch( m_cControlType )
		{
		case head_type_start_request :
			iLength += sizeof(int32_t);
			iLength += sizeof(int16_t);
			break;
		case head_type_transform_request:
			iLength += sizeof(int32_t);
			break;
		case head_type_force_close_connection :
			iLength += sizeof(int8_t);
			iLength += sizeof(int32_t);
			iLength += sizeof(int16_t);
			break;
		case head_type_disconnect_notify :
			iLength += sizeof(int32_t);
			iLength += sizeof(int8_t);
			break;
		case head_type_group_transform_request :
			break;
		default :
			return 0;
		}
		return iLength;
	}

}}//namespace KingNet { namespace Server {
