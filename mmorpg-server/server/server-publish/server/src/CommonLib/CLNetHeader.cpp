#include "CLNetHeader.h"
#include <AvalonCodeEngine.h>

CNetHead::CNetHead()
{
	m_iPackageSize = 0;
	m_cCtrlId = enmControlId_invalid;
	m_RoleId = 0;
	m_unAccId = 0;

	memset(&m_stControlInfo, 0, sizeof(m_stControlInfo));
}

CNetHead::~CNetHead()
{

}

int32_t CNetHead::encode(char* pszCode, int32_t& iCodeSize)
{
	if (NULL == pszCode || iCodeSize <= 0)
	{
		return fail;
	}

	int32_t encode_length = 0;
	char* ptmp = pszCode;

	encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_iPackageSize);
	encode_length += Avalon::CCodeEngine::encode_int8(&ptmp, m_cCtrlId);
	encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_unAccId);
	encode_length += Avalon::CCodeEngine::encode_int64(&ptmp, m_RoleId);

	switch (m_cCtrlId)
	{
		case enmControlId_notify_start:
			encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			encode_length += Avalon::CCodeEngine::encode_int16(&ptmp, m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			break;
		case enmControlId_request_transmit:
			encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_iPing);
			break;
		case enmControlId_request_disconnect_client:
			encode_length += Avalon::CCodeEngine::encode_int8(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType);
			encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
			encode_length += Avalon::CCodeEngine::encode_int16(&ptmp, m_stControlInfo.m_stDisconnectInfo.m_nSourcePort);
			break;
		case enmControlId_notify_disconnect:
			//encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stClientDisconnect.m_iAccId);
			encode_length += Avalon::CCodeEngine::encode_int8(&ptmp, m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
			break;
		case enmControlId_notify_ping:
			//encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stClientDisconnect.m_iAccId);
			encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_iPing);
			break;
		default:
			return fail;
			break;
	}

	iCodeSize = encode_length;
	return success;
}

int32_t CNetHead::decode(const char *pszCode, const int32_t iCodeSize, int16_t *pHeadLen /*= NULL*/)
{
	if (NULL == pszCode || iCodeSize < 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszCode);
	//int32_t decode_length = 0;
	int32_t remian_length = iCodeSize;

	remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPackageSize);

	if (m_iPackageSize < iCodeSize) //should be equal 
	{
		return fail;
	}

	remian_length -= Avalon::CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cCtrlId);
	remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, &m_unAccId);
	remian_length -= Avalon::CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_RoleId);

	switch (m_cCtrlId)
	{
		case enmControlId_notify_start:
			remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
			remian_length -= Avalon::CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			break;

		case enmControlId_request_transmit:
			remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_iPing);
			break;

		case enmControlId_request_disconnect_client:
			remian_length -= Avalon::CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType);
			remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
			remian_length -= Avalon::CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stControlInfo.m_stDisconnectInfo.m_nSourcePort);
			break;

		case enmControlId_notify_disconnect:
			//remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_stClientDisconnect.m_iAccId);
			remian_length -= Avalon::CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
			break;
		case enmControlId_notify_ping:
			//encode_length += Avalon::CCodeEngine::encode_int32(&ptmp, m_stControlInfo.m_stClientDisconnect.m_iAccId);
			remian_length -= Avalon::CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stControlInfo.m_iPing);
			break;
		default:
			return fail;
			break;
	}

	if (remian_length < 0)
	{
		return fail;
	}

	if (pHeadLen != NULL)
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
	int32_t total = sizeof(m_iPackageSize)+sizeof(m_cCtrlId)+sizeof(m_unAccId)+sizeof(m_RoleId);

	switch (m_cCtrlId)
	{
	case enmControlId_notify_start:
		total += sizeof(m_stControlInfo.m_stSourceIPInfo.m_iSourceIP);
		total += sizeof(m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
		break;

	case enmControlId_request_transmit:
		total += sizeof(m_stControlInfo.m_iPing);
		break;

	case enmControlId_request_disconnect_client:
		total += sizeof(m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType);
		total += sizeof(m_stControlInfo.m_stDisconnectInfo.m_iSourceIP);
		total += sizeof(m_stControlInfo.m_stDisconnectInfo.m_nSourcePort);

		break;

	case enmControlId_notify_disconnect:
		//total += sizeof(m_stControlInfo.m_stClientDisconnect.m_iAccId);
		total += sizeof(m_stControlInfo.m_stClientDisconnect.m_cDisconnectReason);
		break;

	case enmControlId_notify_ping:
		total += sizeof(m_stControlInfo.m_iPing);
		break;

	default:
		return 0;
		break;
	}

	return total;
}

