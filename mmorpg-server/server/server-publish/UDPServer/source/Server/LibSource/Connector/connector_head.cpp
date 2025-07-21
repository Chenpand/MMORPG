//#include "stdafx.h"
#include "Connector/connector_head.h"


namespace KingNet { namespace Server {





	CNetHead::CNetHead()
	{
		m_iPackageSize = 0;
		m_cControlType = head_type_start_request;
		m_unConnectionIndex = 0;

		memset(&m_stControlInfo, 0, sizeof(m_stControlInfo));
	}

	CNetHead::~CNetHead()
	{
		//do nothing
	}

	int32_t CNetHead::encode( char* pszCode, int32_t& iCodeSize )
	{
		/*
		int32_t m_iPackageSize;//整个包的长度
		int8_t m_cControlType;
		uint16_t m_unConnectionIndex;

		union __U
		{
		struct stSourceIPInfo
		{
		int32_t m_iSourceIP;
		int16_t m_nSourcePort;
		}m_stSourceIPInfo;				//head_type_start_request

		int32_t m_iTime;				//head_type_transform_request
		int8_t m_cDisconnectType;		//head_type_force_close_connection

		struct stClientRequestDisconnect
		{
		int32_t m_iUid;
		int8_t m_cDisconnectReason;
		}m_stClientDisconnect;			//head_type_disconnect_notify

		}m_stControlInfo;

		*/

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
			encode_length = CCodeEngine::encode_int32(&ptmp,m_stControlInfo.m_stClientDisconnect.m_iUid);
			iCodeSize += encode_length;
			encode_length = CCodeEngine::encode_int8(&ptmp,m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
			iCodeSize += encode_length;
			break;

		case head_type_disconnect_by_mac:
			{
				iCodeSize += CCodeEngine::encode_int64(&ptmp,m_stControlInfo.m_lMac);
				break;
			}

		default:
			break;

		}

		return success;
	}

	int32_t CNetHead::decode( const char *pszCode, const int32_t iCodeSize,int16_t *pHeadLen/*=NULL*/ )
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
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stControlInfo.m_stClientDisconnect.m_iUid);
			remian_length -= decode_length;
			decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
			remian_length -= decode_length;
			break;

		case head_type_disconnect_by_mac:
			{
				remian_length -= CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_stControlInfo.m_lMac);
				break;
			}

		default:
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

	void CNetHead::dump()
	{

	}

	int32_t CNetHead::size()
	{
		int32_t iLength = connector_head_base_size ;
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
		
		case head_type_disconnect_by_mac:
			iLength += sizeof(int64_t);
			break;

		default :
			return 0;
		}
		return iLength;
	}

}}//namespace Game51 { namespace Server {
