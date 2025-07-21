/**
@file   cs_gm_message.cpp
@brief  php和web server通讯消息的定义

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/cs_gm_message.h"
#include "public/server_message.h"


namespace KingNet { namespace Server { 

	//惩罚请求协议
	CCSRequestPunish::CCSRequestPunish()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_szReason[0] = '\0';

		m_byPunishType = 0;

		m_iRequestTimestamp = 0;
		m_iPunishEndTimestamp = 0;
	}

	CCSRequestPunish::~CCSRequestPunish()
	{

	}

	CCSRequestPunish& CCSRequestPunish::operator =(const CCSRequestPunish& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iSrcUin = original.m_iSrcUin;
		m_iDstUin = original.m_iDstUin;
		STRNCPY(m_szSrcAccount, original.m_szSrcAccount, sizeof(m_szSrcAccount));
		STRNCPY(m_szDstAccount, original.m_szDstAccount, sizeof(m_szDstAccount));
			
		m_byPunishType = original.m_byPunishType;
		STRNCPY(m_szReason, original.m_szReason, sizeof(m_szReason));
		m_iRequestTimestamp = original.m_iRequestTimestamp;
		m_iPunishEndTimestamp = original.m_iPunishEndTimestamp;
		return *this;
	}

	int32_t CCSRequestPunish::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byPunishType);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_string(&ptmp, m_szReason, (const int16_t)sizeof(m_szReason));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPunishEndTimestamp);
		iOutLength += coded_length;
		return success;
	}

	int32_t CCSRequestPunish::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byPunishType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szReason, (const int16_t)sizeof(m_szReason));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPunishEndTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestPunish::dump()
	{
		return;
	}
	

	//惩罚响应协议
	CCSResponsePunish::CCSResponsePunish()
	{
		m_shResultID = 0;
		m_szFailReason[0] = '\0';
	}

	CCSResponsePunish::~CCSResponsePunish()
	{

	}

	int32_t CCSResponsePunish::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFailReason, (const int16_t)sizeof(m_szFailReason));
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSResponsePunish::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFailReason, (const int16_t)sizeof(m_szFailReason));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponsePunish::dump()
	{
		return;
	}


	//发送系统消息请求协议
	CCSRequestSendSystemMessage::CCSRequestSendSystemMessage()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_szMessageContent[0] = '\0';
		m_iRequestTimestamp = 0;
	}

	CCSRequestSendSystemMessage::~CCSRequestSendSystemMessage()
	{

	}

	CCSRequestSendSystemMessage& CCSRequestSendSystemMessage::operator =(const CCSRequestSendSystemMessage& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iSrcUin = original.m_iSrcUin;
		STRNCPY(m_szSrcAccount, original.m_szSrcAccount, sizeof(m_szSrcAccount));
		
		STRNCPY(m_szMessageContent, original.m_szMessageContent, sizeof(m_szMessageContent));
		m_iRequestTimestamp = original.m_iRequestTimestamp;
		return *this;
	}

	int32_t CCSRequestSendSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMessageContent, (const int16_t)sizeof(m_szMessageContent));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSRequestSendSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMessageContent, (const int16_t)sizeof(m_szMessageContent));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestSendSystemMessage::dump()
	{
		return;
	}


	//惩罚响应协议
	CCSResponseSendSystemMessage::CCSResponseSendSystemMessage()
	{
		m_shResultID = 0;
		m_szFailReason[0] = '\0';
	}

	CCSResponseSendSystemMessage::~CCSResponseSendSystemMessage()
	{

	}

	int32_t CCSResponseSendSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFailReason, (const int16_t)sizeof(m_szFailReason));
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSResponseSendSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFailReason, (const int16_t)sizeof(m_szFailReason));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseSendSystemMessage::dump()
	{
		return;
	}

	//SS_MSG_RELAY_SYSTEM_MESSAGE
	CRequestRelaySystemMessage::CRequestRelaySystemMessage()
	{
		m_iRoomID = invalid_object_id;
		m_nGameID = -1;
        m_iMessageID = -1;
		m_cCount = 0;
	}

	CRequestRelaySystemMessage::~CRequestRelaySystemMessage()
	{	
		//nothing
	}

	int32_t CRequestRelaySystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

        if (m_cCount > (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0])))
        {
            m_cCount = (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0]));
        }

        if (m_cCount < 0)
        {
            m_cCount = 0;
        }

        coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cCount);
        iOutLength += coded_length;

        for (int32_t i = 0; i < m_cCount; ++i)
        {
            coded_length = encode_system_message(&ptmp, m_stSystemMessage[i]);
            iOutLength += coded_length;
        }


		return success;
	}

	int32_t CRequestRelaySystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cCount);
        remained_length -= decoded_length;

        if (m_cCount > (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0])) )
        {
            return fail;
        }

        if (m_cCount < 0)
        {
            return fail;
        }

        for (int32_t i = 0; i < m_cCount; ++i)
        {
            decoded_length = decode_system_message(&ptmp, m_stSystemMessage[i]);
            remained_length -= decoded_length;
        }

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestRelaySystemMessage::dump()
	{

	}

	//
	CResponseRelaySystemMessage::CResponseRelaySystemMessage()
	{
		m_nResultID = result_id_success;
		m_iRoomID = invalid_object_id;
		m_nGameID = -1;
        m_iMessageID = -1;
		m_szResultString[0] = '\0';
	}

	CResponseRelaySystemMessage::~CResponseRelaySystemMessage()
	{
		//nothing
	}

	int32_t CResponseRelaySystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseRelaySystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}	

	void CResponseRelaySystemMessage::dump()
	{

	}

	//SS_MSG_SUBMIT_SYSTEM_MESSAGE
	CRequestSubmitSystemMessage::CRequestSubmitSystemMessage()
	{
		m_iRoomID = invalid_object_id;
		m_cPushTrigger = enmPushTrigger_null;
		m_cMessageIndex = -1;
		m_nGameID = -1;
        m_iMessageID = -1;
		m_cCount = 0;
	}

	CRequestSubmitSystemMessage::~CRequestSubmitSystemMessage()
	{
		//
	}

	int32_t CRequestSubmitSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;
	
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cPushTrigger);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cMessageIndex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

		if (m_cCount > (int8_t)(sizeof(m_astSystemMessages)/sizeof(m_astSystemMessages[0])))
		{
			m_cCount = (int8_t)(sizeof(m_astSystemMessages)/sizeof(m_astSystemMessages[0]));
		}

		if (m_cCount < 0)
		{
			m_cCount = 0;
		}

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cCount);
		iOutLength += coded_length;

		for (int32_t i = 0; i < m_cCount; ++i)
		{
			coded_length = encode_system_message(&ptmp, m_astSystemMessages[i]);
			iOutLength += coded_length;
		}



		return success;
	}

	int32_t CRequestSubmitSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cPushTrigger);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cMessageIndex);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cCount);
		remained_length -= decoded_length;

		if (m_cCount > (int8_t)(sizeof(m_astSystemMessages)/sizeof(m_astSystemMessages[0])) )
		{
			return fail;
		}

		if (m_cCount < 0)
		{
			return fail;
		}

		for (int32_t i = 0; i < m_cCount; ++i)
		{
			decoded_length = decode_system_message(&ptmp, m_astSystemMessages[i]);
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestSubmitSystemMessage::dump()
	{

	}

	//
	int32_t encode_system_message( char**pOut, stSystemMessage& system_message )
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int16_t unit_size = 0; //不包含自身的2Byte
		char* ptmp = *pOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;
		
		*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)system_message.m_iLowVersion);
		unit_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)system_message.m_iHighVersion);
		unit_size += coded_length;

		if (system_message.m_nSize > (int16_t)sizeof(system_message.m_szMessage))
		{
			system_message.m_nSize = (int16_t)sizeof(system_message.m_szMessage);
		}

		coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)system_message.m_nSize);
		unit_size += coded_length;
		
		coded_length = CCodeEngine::encode_memory(pOut, system_message.m_szMessage, system_message.m_nSize);
		unit_size += coded_length;

		//
		*pOut -= (unit_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pOut, unit_size);

		iOutLength = coded_length + unit_size;

		*pOut = ptmp + iOutLength;

		return iOutLength;

	}

	int32_t decode_system_message( char**pOut, stSystemMessage& system_message )
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int16_t data_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&system_message.m_iLowVersion);
		iOutLength += decode_length;
		data_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&system_message.m_iHighVersion);
		iOutLength += decode_length;
		data_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&system_message.m_nSize);
		iOutLength += decode_length;
		data_size -= decode_length;

		if (system_message.m_nSize > (int16_t)sizeof(system_message.m_szMessage))
		{
			system_message.m_nSize = (int16_t)sizeof(system_message.m_szMessage); //这种处理方式不妥!
		}
		
		decode_length = CCodeEngine::decode_memory(pOut, system_message.m_szMessage, system_message.m_nSize);
		iOutLength += decode_length;
		data_size -= decode_length; 

		if (data_size < 0)
		{
			return 0;
		}
		else
		{
			skip_byte_of(pOut, data_size);
			iOutLength += data_size;
		}

		return iOutLength;
	}


	//
	CResponseSubmitSystemMessage::CResponseSubmitSystemMessage()
	{
		m_nResultID = result_id_success;
		m_iRoomID = invalid_object_id;
		m_cPushTrigger = enmPushTrigger_null;
		m_cMessageIndex = -1;
		m_nGameID = -1;
        m_iMessageID = -1;
		m_szResultString[0] = '\0';
	}

	CResponseSubmitSystemMessage::~CResponseSubmitSystemMessage()
	{
		//
	}

	int32_t CResponseSubmitSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cPushTrigger);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cMessageIndex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseSubmitSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cPushTrigger);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cMessageIndex);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}


		return success;
	}

	void CResponseSubmitSystemMessage::dump()
	{
		//
	}


	//SS_MSG_CANCEL_SYSTEM_MESSAGE
	CRequestCancelSystemMessage::CRequestCancelSystemMessage()
	{
		m_iRoomID = invalid_object_id;
		m_cPushTrigger = enmPushTrigger_null;
		m_cMessageIndex = -1;
		m_nGameID = -1;
	}

	CRequestCancelSystemMessage::~CRequestCancelSystemMessage()
	{

	}

	int32_t CRequestCancelSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cPushTrigger);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cMessageIndex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestCancelSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cPushTrigger);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cMessageIndex);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestCancelSystemMessage::dump()
	{

	}

	//

	CResponseCancelSystemMessage::CResponseCancelSystemMessage()
	{
		m_nResultID = result_id_success;
		m_iRoomID = invalid_object_id;
		m_cPushTrigger = enmPushTrigger_null;
		m_cMessageIndex = -1;
		m_nGameID = -1;
		m_szResultString[0] = '\0';
	}

	CResponseCancelSystemMessage::~CResponseCancelSystemMessage()
	{
		//
	}

	int32_t CResponseCancelSystemMessage::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cPushTrigger);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cMessageIndex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseCancelSystemMessage::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cPushTrigger);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cMessageIndex);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseCancelSystemMessage::dump()
	{

	}

    //SS_MSG_CANCEL_MESSAGE
    CRequestCancelMessage::CRequestCancelMessage()
    {
        m_iMessageID = -1;
        m_iRoomID = invalid_object_id;
        m_nGameID = -1;
    }

    CRequestCancelMessage::~CRequestCancelMessage()
    {

    }

    int32_t CRequestCancelMessage::encode( char *pszOut, int32_t& iOutLength )
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
        iOutLength += coded_length;

        return success;
    }

    int32_t CRequestCancelMessage::decode( const char *pszIn, const int32_t iInLength )
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
        remained_length -= decoded_length;

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

    void CRequestCancelMessage::dump()
    {

    }

    //

    CResponseCancelMessage::CResponseCancelMessage()
    {
        m_nResultID = result_id_success;
        m_iMessageID = -1;
        m_iRoomID = invalid_object_id;
        m_nGameID = -1;
        m_szResultString[0] = '\0';
    }

    CResponseCancelMessage::~CResponseCancelMessage()
    {
        //
    }

    int32_t CResponseCancelMessage::encode( char *pszOut, int32_t& iOutLength )
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
        iOutLength += coded_length;

        if (result_id_success != m_nResultID)
        {
            coded_length = CCodeEngine::encode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
            iOutLength += coded_length;
        }

        return success;
    }

    int32_t CResponseCancelMessage::decode( const char *pszIn, const int32_t iInLength )
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
        remained_length -= decoded_length;

        if (result_id_success != m_nResultID)
        {
            decoded_length = CCodeEngine::decode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
            remained_length -= decoded_length;
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

    void CResponseCancelMessage::dump()
    {

    }

    CRequestSendSystemMessageToPlayer::CRequestSendSystemMessageToPlayer() : 
        m_iUin(-1),
        m_iMessageID(-1),
        m_cCount(0)
    {}

    CRequestSendSystemMessageToPlayer::~CRequestSendSystemMessageToPlayer()
    {
    }

    
    int32_t CRequestSendSystemMessageToPlayer::encode(char *pszOut, int32_t& iOutLength)
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

        if (m_cCount > (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0])))
        {
            m_cCount = (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0]));
        }

        if (m_cCount < 0)
        {
            m_cCount = 0;
        }

        coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cCount);
        iOutLength += coded_length;

        for (int32_t i = 0; i < m_cCount; ++i)
        {
            coded_length = encode_system_message(&ptmp, m_stSystemMessage[i]);
            iOutLength += coded_length;
        }


        return success;

    }

    int32_t CRequestSendSystemMessageToPlayer::decode(const char *pszIn, const int32_t iInLength)
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cCount);
        remained_length -= decoded_length;

        if (m_cCount > (int8_t)(sizeof(m_stSystemMessage)/sizeof(m_stSystemMessage[0])) )
        {
            return fail;
        }

        if (m_cCount < 0)
        {
            return fail;
        }

        for (int32_t i = 0; i < m_cCount; ++i)
        {
            decoded_length = decode_system_message(&ptmp, m_stSystemMessage[i]);
            remained_length -= decoded_length;
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;

    }

    void CRequestSendSystemMessageToPlayer::dump() 
    {

    }

    CResponseSendSystemMessageToPlayer::CResponseSendSystemMessageToPlayer() :
        m_nResultID(-1),
        m_iUin(-1),
        m_iMessageID(-1)
    {
        m_szResultString[0] = '\0';
    }

    CResponseSendSystemMessageToPlayer::~CResponseSendSystemMessageToPlayer()
    {

    }

    int32_t CResponseSendSystemMessageToPlayer::encode(char *pszOut, int32_t& iOutLength)
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageID);
        iOutLength += coded_length;

        if (result_id_success != m_nResultID)
        {
            coded_length = CCodeEngine::encode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
            iOutLength += coded_length;
        }

        return success;
    }

    int32_t CResponseSendSystemMessageToPlayer::decode( const char *pszIn, const int32_t iInLength )
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMessageID);
        remained_length -= decoded_length;

        if (result_id_success != m_nResultID)
        {
            decoded_length = CCodeEngine::decode_string(&ptmp, m_szResultString, (const int16_t)sizeof(m_szResultString));
            remained_length -= decoded_length;
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }	

    void CResponseSendSystemMessageToPlayer::dump()
    {

    }

}}//namespace KingNet { namespace Server {


