#include "public/server_message.h"
#include "public/server_public_define.h"
#include "public/server_gamedb_message_body.h"
#include "Common/common_codeengine.h"
#include "Common/Crypt.h"


namespace KingNet { namespace Server {

//profile加解密信息
char szProfileKey[] = { 'P','r','o','f','i','l','e','C','r','@','5','1','.','C','O','M' };

//Session key
char szSSKey[auth_service_key_length] = {'B', 'e', 's', 't', 'G', 'a', 'm', 'e', '@', 'K', 'i', 'n', 'g', 'N', 'e', 't'};

CMessageHead::CMessageHead()
{
	m_iMessageID		= invalid_message_id;
	m_nMessageType		= invalid_message_type; 
	m_bSourceEntityType = entity_type_invalid;
	m_bDestEntityType	= entity_type_invalid;
	m_iSourceID			= invalid_object_id;
	m_iDestID			= invalid_object_id;
	m_shVersionType		= client_type_not_client;
	m_iMessageSequence	= -1;
}

CMessageHead::~CMessageHead()
{
	//do nothing
}

int32_t CMessageHead::encode(char* pszOut, int32_t& iOutLength)
{
	if (NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iMessageID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_nMessageType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_bSourceEntityType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_bDestEntityType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSourceID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shVersionType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMessageSequence);
	iOutLength += coded_length;


	return success;

}

int32_t CMessageHead::decode(const char* pszIn, const int32_t iInLength)
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_iMessageID));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_nMessageType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_bSourceEntityType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_bDestEntityType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iSourceID));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iDestID));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_shVersionType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iMessageSequence));
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CMessageHead::dump()
{

}

void CMessageHead::dump(FILE* fp)
{
	if (NULL == fp)
	{
		return;
	}

	fprintf(fp, "MessageID:%04d, MessageType:%02d, Sequence:%08d\n", m_iMessageID, m_nMessageType, m_iMessageSequence);

}

void CMessageHead::dump(const char *file)
{
	if (NULL == file)
	{
		return;
	}

	FILE* fp = fopen(file, "a+");
	if (NULL == fp)
	{
		return;
	}

	dump(fp);

	fclose(fp);

}

//////////////////////////////////////////////////////////////////////////
CMessageBody::CMessageBody()
{
	//do nothing
}

CMessageBody::~CMessageBody()
{
	//do nothing
}



//////////////////////////////////////////////////////////////////////////

	CNotifyServerInit::CNotifyServerInit()
	{
		m_iLocalIP = 0;
		m_nLocalPort = 0;
	}

	CNotifyServerInit::~CNotifyServerInit()
	{
		//do nothing
	}

	int32_t CNotifyServerInit::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLocalIP);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_nLocalPort);
		iOutLength += coded_length;


		return success;
	}

	int32_t CNotifyServerInit::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLocalIP);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLocalPort);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;

	}

	void CNotifyServerInit::dump()
	{
		//do nothing
	}
	
//////////////////////////////////////////////////////////////////////////
			
	//SS_HALL_NOTIFY_PLAYER_STATE
	CNotifyPlayerState::CNotifyPlayerState()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_cState = player_offline;
		m_iAddtional = 0;
		m_lMacAddr = 0;
		m_cClientType = 0;
	}

	CNotifyPlayerState::~CNotifyPlayerState()
	{
		//do nothing
	}

	int32_t CNotifyPlayerState::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cState);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAddtional);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int64(&ptmp, m_lMacAddr);
		iOutLength += coded_length;

		//coded_length = CCodeEngine::encode_int8(&ptmp, m_cClientType);
		//iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyPlayerState::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cState);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAddtional);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lMacAddr);
		remained_length -= decoded_length;

		//decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cClientType);
		//remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}
		
		return success;
	}

	void CNotifyPlayerState::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

	CRequestBroadcastMessage::CRequestBroadcastMessage()
	{
		m_nServerType	= -1;
		m_nDataSize		= 0;
		m_szTransparentData[0] = '\0';
	}

	CRequestBroadcastMessage::~CRequestBroadcastMessage()
	{
		//do nothing
	}

	int32_t CRequestBroadcastMessage::encode(char *pszOut, int32_t& iOutLength)
	{
		if ((NULL == pszOut) || (m_nDataSize > max_transparent_data_size))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_nServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataSize);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestBroadcastMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t *)&m_nServerType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
		remained_length -= decoded_length;

		if (m_nDataSize > max_transparent_data_size)
		{
			return fail;
		}

		decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestBroadcastMessage::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

	CResponseBroadcastMessage::CResponseBroadcastMessage()
	{
		m_iResult	= success;
		m_nServerType	= 0;
		m_nDataSize = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseBroadcastMessage::~CResponseBroadcastMessage()
	{
		//do nothing
	}

	int32_t CResponseBroadcastMessage::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_iResult);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_nServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataSize);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int32_t)(sizeof(m_szReasonMessage)));
		iOutLength += coded_length;

		return success;
	}

	int32_t CResponseBroadcastMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_iResult);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t *)&m_nServerType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int32_t)(sizeof(m_szReasonMessage)));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseBroadcastMessage::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

	CNotifyBroadcastMessage::CNotifyBroadcastMessage()
	{
		m_nServerType	= -1;
		m_nDataSize		= 0;
		m_szTransparentData[0] = '\0';
	}

	CNotifyBroadcastMessage::~CNotifyBroadcastMessage()
	{
		//do nothing
	}

	int32_t CNotifyBroadcastMessage::encode(char *pszOut, int32_t& iOutLength)
	{
		if ((NULL == pszOut) || (m_nDataSize > max_transparent_data_size))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_nServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataSize);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyBroadcastMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t *)&m_nServerType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
		remained_length -= decoded_length;

		if (m_nDataSize > max_transparent_data_size)
		{
			return fail;
		}

		decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyBroadcastMessage::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

	CNotifyKickPlayer::CNotifyKickPlayer()
	{
		m_iVictimUid = 0;
		m_iActorUid = 0;
		m_nReasonID  = 0;
		m_szReasonMessage[0] = '\0';
	}

	CNotifyKickPlayer::~CNotifyKickPlayer()
	{
		//do nothing
	}

	int32_t CNotifyKickPlayer::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iVictimUid);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iActorUid);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nReasonID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyKickPlayer::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iVictimUid);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iActorUid);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nReasonID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyKickPlayer::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

    int32_t prepare_game_tag_string(char* pOut, int16_t iOutLength, int8_t server_type, int32_t server_id, int32_t timestamp, int32_t unique_sequence)
    {
        if ((NULL == pOut) || (iOutLength < max_game_tag_length))
        {
            return fail;
        }

        int16_t length = iOutLength - 1;

        snprintf(pOut, length, "TP%02dID%03dTM%08XSQ%04X", server_type, server_id, timestamp, (unique_sequence & 0xffff));
        pOut[length] = '\0';

        return success;
    }



//////////////////////////////////////////////////////////////////////////

    int32_t encode_charm_item_message(char** pszOut, stCharmItem& message)
    {
        if (NULL == *pszOut || NULL == pszOut)
        {
            return 0;
        }

        int16_t message_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength      = 0;
        int32_t encode_length   = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间


        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iSrcUIN);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		message_size += encode_length;

        encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        message_size += encode_length;

		encode_length = CCodeEngine::encode_int8(pszOut, message.m_cSrcGender);
		message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iDestUIN);
        message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iDeltaCharmValue);
        message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iItemID);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szItemName, sizeof(message.m_szItemName));
		message_size += encode_length;

        encode_length = CCodeEngine::encode_int16(pszOut, message.m_nDeltaItemCount);
        message_size += encode_length;

        encode_length = CCodeEngine::encode_string(pszOut, message.m_szPresentMessage, sizeof(message.m_szPresentMessage));
        message_size += encode_length;


        *pszOut -= (message_size + sizeof(int16_t));
        encode_length = CCodeEngine::encode_int16(pszOut, message_size);

        iOutLength = (message_size + encode_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_charm_item_message(char** pszIn, stCharmItem& message)
    {
        if (NULL == *pszIn || NULL == pszIn)
        {
            return 0;
        }

        int16_t message_size    = 0;
        int32_t iInLength       = 0;
        int32_t decode_length   = 0;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message_size);
        iInLength += decode_length;


        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iSrcUIN);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&message.m_cSrcGender);
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iDestUIN);
        iInLength += decode_length;
        message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iDeltaCharmValue);
        iInLength += decode_length;
        message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iItemID);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szItemName, sizeof(message.m_szItemName));
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message.m_nDeltaItemCount);
        iInLength += decode_length;
        message_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszIn, message.m_szPresentMessage, sizeof(message.m_szPresentMessage));
        iInLength += decode_length;
        message_size -= decode_length;


        if (message_size < 0)
        {
            return 0;
        }
        else //对于不能解码的字节流，跳过
        {
            (*pszIn)  += message_size;
            iInLength += message_size;
        }

        return iInLength;
    }

	int32_t encode_common_item_info_message(char** pszOut, stCommonItemInfo& message)
	{
		if (NULL == *pszOut || NULL == pszOut)
		{
			return 0;
		}

		int16_t message_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength      = 0;
		int32_t encode_length   = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		encode_length = CCodeEngine::encode_int32(pszOut, message.m_iItemID);
		message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szItemName, sizeof(message.m_szItemName));
		message_size += encode_length;

		encode_length = CCodeEngine::encode_int16(pszOut, message.m_nDeltaItemCount);
		message_size += encode_length;


		*pszOut -= (message_size + sizeof(int16_t));
		encode_length = CCodeEngine::encode_int16(pszOut, message_size);

		iOutLength = (message_size + encode_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_common_item_info_message(char** pszIn, stCommonItemInfo& message)
	{
		if (NULL == *pszIn || NULL == pszIn)
		{
			return 0;
		}

		int16_t message_size    = 0;
		int32_t iInLength       = 0;
		int32_t decode_length   = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message_size);
		iInLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iItemID);
		iInLength += decode_length;
		message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szItemName, sizeof(message.m_szItemName));
		iInLength += decode_length;
		message_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message.m_nDeltaItemCount);
		iInLength += decode_length;
		message_size -= decode_length;

		if (message_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn)  += message_size;
			iInLength += message_size;
		}

		return iInLength;
	}


    int32_t encode_common_item_message(char** pszOut, stCommonItem& message)
    {
        if (NULL == *pszOut || NULL == pszOut)
        {
            return 0;
        }

		if ((message.m_cItemTypeCount <= 0) || ((uint8_t)message.m_cItemTypeCount > max_item_count_in_goods))
		{
			return 0;
		}

        int16_t message_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength      = 0;
        int32_t encode_length   = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间


        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iSrcUIN);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		message_size += encode_length;

        encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        message_size += encode_length;

		encode_length = CCodeEngine::encode_int8(pszOut, message.m_cSrcGender);
		message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iDestUIN);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_int8(pszOut, message.m_cItemTypeCount);
		message_size += encode_length;

		for(int8_t i=0; i<message.m_cItemTypeCount; ++i)
		{
			encode_length = encode_common_item_info_message(pszOut, message.m_astCommonItemInfo[i]);
			message_size += encode_length;
		}

        encode_length = CCodeEngine::encode_string(pszOut, message.m_szPresentMessage, sizeof(message.m_szPresentMessage));
        message_size += encode_length;

		encode_length = CCodeEngine::encode_int32(pszOut, message.m_iHappyBeanCount);
		message_size += encode_length;


        *pszOut -= (message_size + sizeof(int16_t));
        encode_length = CCodeEngine::encode_int16(pszOut, message_size);

        iOutLength = (message_size + encode_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_common_item_message(char** pszIn, stCommonItem& message)
    {
        if (NULL == *pszIn || NULL == pszIn)
        {
            return 0;
        }

        int16_t message_size    = 0;
        int32_t iInLength       = 0;
        int32_t decode_length   = 0;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message_size);
        iInLength += decode_length;


        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iSrcUIN);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&message.m_cSrcGender);
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iDestUIN);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&message.m_cItemTypeCount);
		iInLength += decode_length;
		message_size -= decode_length;

		if ((message.m_cItemTypeCount > 0) && ((uint8_t)message.m_cItemTypeCount <= max_item_count_in_goods))
		{
			for(int8_t i=0; i<message.m_cItemTypeCount; ++i)
			{
				decode_length = decode_common_item_info_message(pszIn, message.m_astCommonItemInfo[i]);
				iInLength += decode_length;
				message_size -= decode_length;
			}
		}

        decode_length = CCodeEngine::decode_string(pszIn, message.m_szPresentMessage, sizeof(message.m_szPresentMessage));
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iHappyBeanCount);
		iInLength += decode_length;
		message_size -= decode_length;

        if (message_size < 0)
        {
            return 0;
        }
        else //对于不能解码的字节流，跳过
        {
            (*pszIn)  += message_size;
            iInLength += message_size;
        }

        return iInLength;
    }


    int32_t encode_bomb_blast_message(char** pszOut, stBombBlast& message)
    {
        if (NULL == *pszOut || NULL == pszOut)
        {
            return 0;
        }

        int16_t message_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength      = 0;
        int32_t encode_length   = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间


        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iSrcUIN);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		message_size += encode_length;

        encode_length = CCodeEngine::encode_string(pszOut, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        message_size += encode_length;

		encode_length = CCodeEngine::encode_int8(pszOut, message.m_cSrcGender);
		message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iDestUIN);
        message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iDeltaCharmValue);
        message_size += encode_length;

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_iItemID);
        message_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, message.m_szItemName, sizeof(message.m_szItemName));
		message_size += encode_length;

        encode_length = CCodeEngine::encode_int16(pszOut, message.m_nDeltaItemCount);
        message_size += encode_length;


        *pszOut -= (message_size + sizeof(int16_t));
        encode_length = CCodeEngine::encode_int16(pszOut, message_size);

        iOutLength = (message_size + encode_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_bomb_blast_message(char** pszIn, stBombBlast& message)
    {
        if (NULL == *pszIn || NULL == pszIn)
        {
            return 0;
        }

        int16_t message_size    = 0;
        int32_t iInLength       = 0;
        int32_t decode_length   = 0;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message_size);
        iInLength += decode_length;


        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iSrcUIN);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcAccount, sizeof(message.m_szSrcAccount));
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszIn, message.m_szSrcNick, sizeof(message.m_szSrcNick));
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&message.m_cSrcGender);
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iDestUIN);
        iInLength += decode_length;
        message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iDeltaCharmValue);
        iInLength += decode_length;
        message_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_iItemID);
        iInLength += decode_length;
        message_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, message.m_szItemName, sizeof(message.m_szItemName));
		iInLength += decode_length;
		message_size -= decode_length;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message.m_nDeltaItemCount);
        iInLength += decode_length;
        message_size -= decode_length;


        if (message_size < 0)
        {
            return 0;
        }
        else //对于不能解码的字节流，跳过
        {
            (*pszIn)  += message_size;
            iInLength += message_size;
        }

        return iInLength;
    }

//////////////////////////////////////////////////////////////////////////

    CSubMessage::CSubMessage()
    {
		m_nMessageID = 0;
		m_nBuffSize  = 0;
    }

    CSubMessage::~CSubMessage()
    {
        //do nothing
    }

	int32_t CSubMessage::encode(char *pszOut, int32_t& iOutLength)
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

		encode_length = CCodeEngine::encode_int16(&ptmp, m_nMessageID);
		iOutLength += encode_length;

		switch(m_nMessageID)
		{
		case notify_present_charm_item:
			encode_length = encode_charm_item_message(&ptmp, m_stCharmItem);
			break;

		case notify_present_common_item:
			encode_length = encode_common_item_message(&ptmp, m_stCommonItem);
			break;

		case notify_use_bomb:
			encode_length = encode_bomb_blast_message(&ptmp, m_stBombBlast);
			break;

		case notify_present_chest:
			encode_length = encode_common_item_info_message(&ptmp, m_stChestInfo);
			break;

		default:
			encode_length = 0;
			break;
		}

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

	int32_t CSubMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nMessageID);
		remain_length -= decode_length;

		switch(m_nMessageID)
		{
		case notify_present_charm_item:
			decode_length = decode_charm_item_message(&ptmp, m_stCharmItem);
			break;

		case notify_present_common_item:
			decode_length = decode_common_item_message(&ptmp, m_stCommonItem);
			break;

		case notify_use_bomb:
			decode_length = decode_bomb_blast_message(&ptmp, m_stBombBlast);
			break;

		case notify_present_chest:
			decode_length = decode_common_item_info_message(&ptmp, m_stChestInfo);
			break;

		default:
			decode_length = 0;
			break;
		}

		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nBuffSize);
		remain_length -= decode_length;
		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		if (m_nBuffSize > 0)
		{
			decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentBuff, m_nBuffSize);
			remain_length -= decode_length;
		}

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}




	void CSubMessage::dump()
	{
		//do nothing
	}

/*
    int32_t encode_sub_message(char** pszOut, CSubMessage& message)
    {
        if (NULL == *pszOut || NULL == pszOut)
        {
            return 0;
        }

        int16_t message_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength      = 0;
        int32_t encode_length   = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

        encode_length = CCodeEngine::encode_int32(pszOut, message.m_nMessageID);
        message_size += encode_length;

        switch(message.m_nMessageID)
        {
        case notify_present_charm_item:
            encode_length = encode_charm_item_message(pszOut, message.m_stCharmItem);
            break;

        case notify_present_common_item:
        	encode_length = encode_common_item_message(pszOut, message.m_stCommonItem);
            break;

        case notify_use_bomb:
            encode_length = encode_bomb_blast_message(pszOut, message.m_stBombBlast);
            break;

		case notify_present_chest:
			encode_length = encode_common_item_info_message(pszOut, message.m_stChestInfo);
			break;

        default:
            encode_length = 0;
            break;
        }

        message_size += encode_length;       


        *pszOut -= (message_size + sizeof(int16_t));
        encode_length = CCodeEngine::encode_int16(pszOut, message_size);

        iOutLength = (message_size + encode_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }


    int32_t decode_sub_message(char** pszIn, CSubMessage& message)
    {
        if (NULL == *pszIn || NULL == pszIn)
        {
            return 0;
        }

        int16_t message_size    = 0;
        int32_t iInLength       = 0;
        int32_t decode_length   = 0;

        decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&message_size);
        iInLength += decode_length;


        decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&message.m_nMessageID);
        iInLength += decode_length;
        message_size -= decode_length;

        switch(message.m_nMessageID)
        {
        case notify_present_charm_item:
            decode_length = decode_charm_item_message(pszIn, message.m_stCharmItem);
        	break;

        case notify_present_common_item:
            decode_length = decode_common_item_message(pszIn, message.m_stCommonItem);
        	break;

        case notify_use_bomb:
            decode_length = decode_bomb_blast_message(pszIn, message.m_stBombBlast);
            break;

		case notify_present_chest:
			decode_length = decode_common_item_info_message(pszIn, message.m_stChestInfo);
			break;

        default:
            decode_length = 0;
            break;
        }

        iInLength += decode_length;
        message_size -= decode_length;


        if (message_size < 0)
        {
            return 0;
        }
        else //对于不能解码的字节流，跳过
        {
            (*pszIn)  += message_size;
            iInLength += message_size;
        }

        return iInLength;
    }
*/

	void skip_byte_of(char** pOut, int32_t iByteCount)
	{
		if (pOut == NULL || NULL == *pOut || iByteCount <= 0)
	 	{
	 		return;
	 	}
	 
	 	(*pOut) += iByteCount;
	 
	 	return;
	 }


	CNotifySystemMessage::CNotifySystemMessage()
	{
		m_nMessageSize = 0;
		m_szSystemMessage[0] = '\0';
	}

	CNotifySystemMessage::~CNotifySystemMessage()
	{
		//do nothing
	}

	void CNotifySystemMessage::dump()
	{
		//do nothing
	}

	int32_t CNotifySystemMessage::encode(char *pszOut, int32_t& iOutLength)
	{
		if ((NULL == pszOut) || (m_nMessageSize > max_system_message_length))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nMessageSize);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_memory(&ptmp, m_szSystemMessage, (const int32_t)m_nMessageSize);
		iOutLength += encode_length;

		return success;
	}

	int32_t CNotifySystemMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nMessageSize);
		remain_length -= decode_length;

		if (m_nMessageSize > max_system_message_length)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szSystemMessage, (const int32_t)m_nMessageSize);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}


////////////////////////////////////////////////////////////////////////


	int32_t encrypt_profile_info(char* pszOut, int16_t& nOutBuffLength, stCryptUserBaseWebProfile& info)
	{
		char szUnCryptBuff[max_profile_crypt_length] = {0};
		int16_t nUnCryptLength = 0;

//		int16_t info_size = 0;
		char* ptmp = &szUnCryptBuff[0];
		int32_t encode_length = 0;

// 		ptmp += sizeof(int16_t);
// 
// 		encode_length = CCodeEngine::encode_int32(&ptmp, info.m_iFlag);
// 		info_size += encode_length;
// 
// 		encode_length = CCodeEngine::encode_int32(&ptmp, info.m_iVIPScore);
// 		info_size += encode_length;
// 		
// 		
// 		encode_length = CCodeEngine::encode_int32(&ptmp, info.m_iVIPLevel);
// 		info_size += encode_length;
// 		
// 		ptmp = &szUnCryptBuff[0];
// 		encode_length = CCodeEngine::encode_int16(&ptmp, info_size);
// 		nUnCryptLength = info_size + encode_length;

		encode_length = encode_profile_info(&ptmp, info);
		nUnCryptLength = encode_length;

		if ((nUnCryptLength + 16) > nOutBuffLength)
		{
			return fail;
		}

		nOutBuffLength = (int32_t)CXTEA::Encrypt(szUnCryptBuff, nUnCryptLength, pszOut, nOutBuffLength, szProfileKey);

		if (nOutBuffLength <= 0)
		{
			return fail;
		}

		return success;

	}

	int32_t decrypt_profile_info(char* pszIn, int16_t nInBuffLength, stCryptUserBaseWebProfile& info)
	{
		char szUnCryptBuff[max_profile_crypt_length] = {0};
		int16_t nUnCryptLength = 0;

		nUnCryptLength = (int32_t)CXTEA::Decrypt(pszIn, nInBuffLength, szUnCryptBuff, max_profile_crypt_length, szProfileKey);

		if (nUnCryptLength <= 0)
		{
			return fail;
		}

		char* ptmp = &szUnCryptBuff[0];
		int32_t decode_length = 0;
//		int16_t info_size = 0;

		decode_length = decode_profile_info(&ptmp, info);
		if (decode_length <= 0)
		{
			return fail;
		}

// 		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&info_size);
// 		if (nUnCryptLength != (info_size + decode_length))
// 		{
// 			return fail;
// 		}
// 
// 		decode_length = CCodeEngine ::decode_int32(&ptmp, (uint32_t*)&info.m_iFlag);
// 		info_size -= decode_length;
// 
// 		if (info_size < 0)
// 		{
// 			return fail;
// 		}
// 		
// 		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&info.m_iVIPScore);
// 		info_size -= decode_length;
// 		if (info_size < 0)
// 		{
// 			return fail;
// 		}
// 		
// 		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&info.m_iVIPLevel);
// 		info_size -= decode_length;
// 		if (info_size < 0)
// 		{
// 			return fail;
// 		}
		return success;
	}


//返回值为0则成功，返回其它则失败(注：传入的iOutBuffLength是这个buff的size，返回时候是buff内容的长度)
int32_t encrypt_profile_webscore(char* pszOut, int16_t& nOutBuffLength, stCryptUserWebScore& info)
{
	char szUnCryptBuff[max_profile_crypt_length] = {0};
	int16_t nUnCryptLength = 0;

//	int16_t info_size = 0;
	char* ptmp = &szUnCryptBuff[0];
	int32_t encode_length = 0;

// 	ptmp += sizeof(int16_t);
// 
// 	encode_length = CCodeEngine::encode_int32(&ptmp, info.m_i51Score);
// 	info_size += encode_length;
// 		
// 		
// 	encode_length = CCodeEngine::encode_int32(&ptmp, info.m_i51Level);
// 	info_size += encode_length;
// 		
// 	ptmp = &szUnCryptBuff[0];
// 	encode_length = CCodeEngine::encode_int16(&ptmp, info_size);
// 	nUnCryptLength = info_size + encode_length;

	encode_length = encode_profile_webscore(&ptmp, info);
	nUnCryptLength = encode_length;

	if ((nUnCryptLength + 16) > nOutBuffLength)
	{
		return fail;
	}

	nOutBuffLength = (int32_t)CXTEA::Encrypt(szUnCryptBuff, nUnCryptLength, pszOut, nOutBuffLength, szProfileKey);

	if (nOutBuffLength <= 0)
	{
		return fail;
	}
	
	return success;

}

int32_t decrypt_profile_webscore(char* pszIn, int16_t nInBuffLength, stCryptUserWebScore& info)
{
	char szUnCryptBuff[max_profile_crypt_length] = {0};
	int16_t nUnCryptLength = 0;

	nUnCryptLength = (int32_t)CXTEA::Decrypt(pszIn, nInBuffLength, szUnCryptBuff, max_profile_crypt_length, szProfileKey);

	if (nUnCryptLength <= 0)
	{
		return fail;
	}

	char* ptmp = &szUnCryptBuff[0];
	int32_t decode_length = 0;
//	int16_t info_size = 0;

// 	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&info_size);
// 	if (nUnCryptLength != (info_size + decode_length))
// 	{
// 		return fail;
// 	}
// 
// 	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&info.m_i51Score);
// 	info_size -= decode_length;
// 	if (info_size < 0)
// 	{
// 		return fail;
// 	}
// 
// 	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&info.m_i51Level);
// 	info_size -= decode_length;

// 	if (info_size < 0)
// 	{
// 		return fail;
// 	}

	decode_length = decode_profile_webscore(&ptmp, info);
	if (decode_length <= 0)
	{
		return fail;
	}

	return success;
}


	int32_t encode_qun_info(char** pszOut, stQunDetailInfo& qun_info)
	{
		if (NULL == *pszOut || NULL == pszOut)
		{
			return 0;
		}

		int16_t qun_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t encode_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		encode_length = CCodeEngine::encode_string(pszOut, qun_info.m_szQunUID, sizeof(qun_info.m_szQunUID));
		qun_size += encode_length;

		encode_length = CCodeEngine::encode_int16(pszOut, qun_info.m_nQunRole);
		qun_size += encode_length;

		*pszOut -= (qun_size + sizeof(int16_t));
		encode_length = CCodeEngine::encode_int16(pszOut, qun_size);

		iOutLength = (qun_size + encode_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_qun_info(char** pszIn, stQunDetailInfo& qun_info)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t qun_size = 0;
		int32_t iInLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&qun_size);
		iInLength += decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, qun_info.m_szQunUID, sizeof(qun_info.m_szQunUID));
		iInLength += decode_length;
		qun_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&qun_info.m_nQunRole);
		iInLength += decode_length;
		qun_size -= decode_length;

		if (qun_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn)  += qun_size;
			iInLength += qun_size;
		}

		return iInLength;
	}


	int32_t encode_player_qun_data(char* pszOut, int16_t& nOutBuffLength, stPlayerQunData& player_qun_data)
	{
		if (NULL == pszOut)
		{
			return 0;
		}

		if (player_qun_data.m_nQunCount > max_qun_count)
		{
			return fail;
		}

		char* ptmp = pszOut;

		int32_t iOutLength = 0;
		int32_t encode_length = 0;

		encode_length = CCodeEngine::encode_int16(&ptmp, player_qun_data.m_nQunCount);
		iOutLength += encode_length;

		for(int16_t i=0; i<player_qun_data.m_nQunCount; ++i)
		{
			encode_length = encode_qun_info(&ptmp, player_qun_data.m_astQunDetailInfo[i]);
			iOutLength += encode_length;
		}

		nOutBuffLength = iOutLength;

		return success;
	}

	int32_t decode_player_qun_data(char* pszIn, int16_t nInBuffLength, stPlayerQunData& player_qun_data)
	{
		if (NULL == pszIn )
		{
			return 0;
		}

		int32_t decode_length = 0;
		int16_t info_size = nInBuffLength;
		char* ptmp = pszIn;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&player_qun_data.m_nQunCount);
		info_size -= decode_length;

		if (player_qun_data.m_nQunCount > max_qun_count)
		{
			return fail;
		}

		for (int16_t i=0; i<player_qun_data.m_nQunCount; ++i)
		{
			decode_length = decode_qun_info(&ptmp, player_qun_data.m_astQunDetailInfo[i]);
			info_size -= decode_length;
		}

		if (info_size < 0)
		{
			return fail;
		}

		return success;
	}

	int32_t encrypt_qun_info(char* pszOut, int16_t& nOutBuffLength, stCryptUser51Qun& info)
	{
		if (info.m_nQunCount > max_qun_count)
		{
			return fail;
		}

		char szUnCryptBuff[max_qun_crypt_length] = {0};
		int16_t nUnCryptLength = 0;

		int16_t info_size = 0;
		char* ptmp = &szUnCryptBuff[0];
		int32_t encode_length = 0;

		ptmp += sizeof(int16_t);

		encode_length = CCodeEngine::encode_string(&ptmp, info.m_szAccount, sizeof(info.m_szAccount));
		info_size += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)info.m_iValidTime);
		info_size += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, info.m_nQunCount);
		info_size += encode_length;

		for(int16_t i=0; i<info.m_nQunCount; ++i)
		{
			encode_length = encode_qun_info(&ptmp, info.m_astQunInfo[i]);
			info_size += encode_length;
		}

		ptmp = &szUnCryptBuff[0];
		encode_length = CCodeEngine::encode_int16(&ptmp, info_size);
		nUnCryptLength = info_size + encode_length;

		if ((nUnCryptLength + 16) > nOutBuffLength)
		{
			return fail;
		}

		nOutBuffLength = (int32_t)CXTEA::Encrypt(szUnCryptBuff, nUnCryptLength, pszOut, nOutBuffLength, szProfileKey);

		if (nOutBuffLength <= 0)
		{
			return fail;
		}
		else
		{
			return success;
		}
	}

	int32_t decrypt_qun_info(char* pszIn, int16_t nInBuffLength, stCryptUser51Qun& info)
	{
		char szUnCryptBuff[max_qun_crypt_length] = {0};
		int16_t nUnCryptLength = 0;

		nUnCryptLength = (int32_t)CXTEA::Decrypt(pszIn, nInBuffLength, szUnCryptBuff, max_qun_crypt_length, szProfileKey);

		if (nUnCryptLength <= 0)
		{
			return fail;
		}

		char* ptmp = &szUnCryptBuff[0];
		int32_t decode_length = 0;
		int16_t info_size = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&info_size);
		if (nUnCryptLength != (info_size + decode_length))
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_string(&ptmp, info.m_szAccount, sizeof(info.m_szAccount));
		info_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&info.m_iValidTime);
		info_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&info.m_nQunCount);
		info_size -= decode_length;

		if (info.m_nQunCount > max_qun_count)
		{
			return fail;
		}

		for (int16_t i=0; i<info.m_nQunCount; ++i)
		{
			decode_length = decode_qun_info(&ptmp, info.m_astQunInfo[i]);
			info_size -= decode_length;
		}

		if (info_size < 0)
		{
			return fail;
		}
		else
		{
			return success;
		}
	}

	int16_t get_crypt_algorithm(int16_t flag)
	{
		return flag & enmCSPackageFlag_Crypt_Mask;
	}

	int16_t get_compress_algorithm(int16_t flag)
	{
		return (flag & enmCSPackageFlag_Compress_Mask);
	}

	int16_t get_signature_algorithm()
	{
		return enmCryptAlgorithm_XTEA; //目前签名的加密算法是固定的
	}

	int32_t compress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength,
		char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszIn || iInLength <= 0 || NULL == pszOut || iOutLength <= 0)
		{
			return fail;
		}

		switch (nCompressAlgorithm)
		{
		case enmCompressAlgorithm_None:
			{
				if (iInLength > iOutLength)
				{
					return fail;
				}

				iOutLength = iInLength;
				memcpy((void*)pszOut, (const void*)pszIn, size_t(iInLength));
				break;
			}

		case enmCompressAlgorithm_7Zip: //
			{
				//字典大小，可以一直设置为16M
				uint32_t dictSize= (1<<24);

				SRes result = SZ_OK;
				size_t nCompressedSize = (size_t)iInLength / 20 * 21 + (1 << 16);
				Byte* pCompressed = NULL;

				// we allocate 105% of original size for output buffer
				//得到压缩后的大小 
				pCompressed = new Byte[nCompressedSize];
				if (NULL == pCompressed)
				{
					return fail;
				}
				//压缩
				result = Lzma86_Encode(pCompressed, &nCompressedSize, (Byte*)pszIn, size_t(iInLength), 5, dictSize, SZ_FILTER_AUTO);
				if (SZ_OK != result)
				{
					delete []pCompressed;
					return fail;
				}
				memcpy(pszOut, pCompressed, nCompressedSize);
				iOutLength = (int32_t)nCompressedSize;
				delete []pCompressed;
			}
			break;

		default:
			return fail;
			break;
		}

		return success;
	}

	int32_t uncompress_buffer(int16_t nCompressAlgorithm, const char* pszIn, const int32_t iInLength, 
		char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszIn || iInLength <= 0 || NULL == pszOut || iOutLength <= 0)
		{
			return fail;
		}

		switch (nCompressAlgorithm)
		{
		case enmCompressAlgorithm_None:
			{
				if (iOutLength < iInLength)
				{
					return fail;
				}

				iOutLength = iInLength;
				memcpy((void*)pszOut, (const void*)pszIn, size_t(iInLength));
				break;
			}
		case enmCompressAlgorithm_7Zip: //暂未实现
			{
				UInt64 xdestlen = 0;
				SizeT nUnCompressedSize = 0;
				if(SZ_OK == Lzma86_GetUnpackSize((const Byte*)pszIn, iInLength, &xdestlen)) //得到解压后数据的大小 
				{
					if (xdestlen <= (UInt64)iOutLength)
					{
						char *pUnCompressedBuf = new char[xdestlen]; //准备存放解压后的数据 
						
						SRes sRet = Lzma86_Decode((Byte*)pUnCompressedBuf, &nUnCompressedSize, (Byte*)pszIn, (SizeT*)&iInLength);
						if((SZ_OK == sRet) && (nUnCompressedSize <= xdestlen)) //解压 
						{
							memcpy((void*)pszOut, (const void*)pUnCompressedBuf, size_t(nUnCompressedSize));
							iOutLength = nUnCompressedSize;
							delete []pUnCompressedBuf; 
						} 
						else
						{
							delete []pUnCompressedBuf; 
							return fail;
						}						
					}
					else
					{
						return fail;
					}
				} 
			}
			break;
		default:
			return fail;
			break;
		}

		return success;
	}

	int32_t crypt_buffer(int16_t nCryptAlgorithm, const char* pszKey, const char* pszIn, 
		const int32_t iInLength, char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszKey || NULL == pszIn || iInLength <= 0 || NULL == pszOut || iOutLength <= 0)
		{
			return fail;
		}

		switch (nCryptAlgorithm)
		{
		case enmCryptAlgorithm_None:
			{
				if (iOutLength < iInLength)
				{
					return fail;
				}

				iOutLength = iInLength;
				memcpy((void*)pszOut, (const void*) pszIn, size_t(iInLength));

				break;
			}

		case enmCryptAlgorithm_XTEA: 
			{
				if ((size_t)iOutLength < CXTEA::GetSafeEncryptOutBufferLength((size_t)iInLength))
				{
					return fail;
				}

				iOutLength = CXTEA::Encrypt((char*)pszIn, size_t(iInLength), pszOut, size_t(iOutLength), (char*)pszKey);
				if (0 == iOutLength)
				{
					return fail;
				}

				break;
			}

		case enmCryptAlgorithm_XTEA_ROUND8: 
			{
				if ((size_t)iOutLength < CXTEA::GetSafeEncryptOutBufferLength((size_t)iInLength))
				{
					return fail;
				}

				iOutLength = CXTEA::EncryptRoundN((char*)pszIn, size_t(iInLength), pszOut, 
						size_t(iOutLength), (char*)pszKey, 8);
				if (0 == iOutLength)
				{
					return fail;
				}

				break;
			}

		default:
			return fail;
			break;
		}

		return success;
	}

	int32_t decrypt_buffer(int16_t nUnCryptAlgorithm,  const char* pszKey, const char* pszIn, const int32_t iInLength, 
		char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszIn || iInLength <= 0 || NULL == pszOut || iOutLength <= 0)
		{
			return fail;
		}

		switch (nUnCryptAlgorithm)
		{
		case enmCryptAlgorithm_None:
			{
				if (iOutLength < iInLength)
				{
					return fail;
				}

				iOutLength = iInLength;
				memcpy((void*)pszOut, (const void*) pszIn, size_t(iInLength));
				break;
			}

		case enmCryptAlgorithm_XTEA: 
			{
				size_t iTmpSize = CXTEA::Decrypt((char*)pszIn, (size_t)iInLength, pszOut, (size_t)iOutLength, (char*)pszKey);
				if (0 == iTmpSize)
				{
					return fail;
				}

				iOutLength = (int32_t)iTmpSize;

				break;
			}

		case enmCryptAlgorithm_XTEA_ROUND8:
			{
				size_t iTmpSize = CXTEA::DecryptRoundN((char*)pszIn, (size_t)iInLength, pszOut, 
						(size_t)iOutLength, (char*)pszKey, -8);
				if (0 == iTmpSize)
				{
					return fail;
				}

				iOutLength = (int32_t)iTmpSize;

				break;
			}

		default:
			return fail;
			break;
		}

		return success;
	}

	//解密玩家签名
	int32_t decrypt_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
		char *pszOut, int32_t& nOutLength)
	{
		if (nInlength > MAX_CS_HEAD_OPTION_LENGTH || NULL == pszOut || NULL == pszIn)	
		{
			return fail;
		}

		if (success != decrypt_buffer(nSignatureCryptAlgorithm, szSSKey, pszIn, nInlength,
			pszOut, nOutLength))
		{
			return fail;
		}

		return success;
	}

	//解码玩家签名
	int32_t decode_player_signature(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
		stPlayerSignature &stSignature)
	{
		if (nInlength > max_signature_length || NULL == pszIn)	
		{
			return fail;
		}

		//得到与client通讯时加密密钥
		int16_t unCryptSize = 0;
		int16_t remain_length= 0;
		//得到签名长度
		stSignature.m_shPLen = (int16_t)ntohs(*(uint16_t*)(pszIn));
		remain_length = stSignature.m_shPLen - sizeof(stSignature.m_shPLen);
		unCryptSize += sizeof(stSignature.m_shPLen);
		//得到用户id
		stSignature.m_iUid = (int32_t)ntohl(*(uint32_t*)(pszIn + unCryptSize));
		unCryptSize += sizeof(stSignature.m_iUid);
		remain_length -= sizeof(stSignature.m_iUid);
		//得到SessionKey
		memcpy(stSignature.m_szKey, pszIn+unCryptSize, crypt_key_length);
		unCryptSize += crypt_key_length;
		remain_length -= crypt_key_length;
		//得到签名的时间戳
		stSignature.m_iTimeStamp = (int32_t)ntohl(*(uint32_t*)(pszIn + unCryptSize));
		unCryptSize += sizeof(stSignature.m_iTimeStamp);
		remain_length -= sizeof(stSignature.m_iTimeStamp);
		//得到扩展字段
		stSignature.m_iOption = (int32_t)ntohl(*(uint32_t*)(pszIn + unCryptSize));
		unCryptSize += sizeof(stSignature.m_iOption);
		remain_length -= sizeof(stSignature.m_iOption);
		if(remain_length < 0 )
			return fail;
		return success;
	}
	int32_t encode_player_signature(char *pszOut, int32_t& iOutLength,stPlayerSignature stSignature)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;
		coded_length = CCodeEngine::encode_int16(&ptmp,stSignature.m_shPLen);
		iOutLength += sizeof(int16_t);

		//uid
		coded_length = CCodeEngine::encode_int32(&ptmp, stSignature.m_iUid);
		iOutLength += coded_length;
		
		//SessionKey
		coded_length = CCodeEngine::encode_memory(&ptmp, (char*)stSignature.m_szKey, (const int32_t)crypt_key_length);
		iOutLength += coded_length;
		//Time Stamp
		coded_length = CCodeEngine::encode_int32(&ptmp, stSignature.m_iTimeStamp);
		iOutLength += coded_length;

		//Option
		coded_length = CCodeEngine::encode_int32(&ptmp, stSignature.m_iOption);
		iOutLength += coded_length;
		
		return success;
	}

	//解码部分的Message Head以及Message Body
	int32_t decrypt_cs_message(int16_t shFlag, const char* szKey, const char* pszIn, const int32_t iInLength, 
		char* pszOut, int32_t& iOutLength)
	{
		//利用加密密钥 szKey解码出被加密的Sequence以及协议体
		char szDecryptBody[max_cs_package_body_size];
		int32_t iDecryptLength = (int32_t) sizeof(szDecryptBody);

		int16_t nUnCompressAlgorithm = get_compress_algorithm(shFlag);
		int16_t nUnCryptAlgorithm = get_crypt_algorithm(shFlag);

		if (success != decrypt_buffer(nUnCryptAlgorithm, (const char*)szKey, pszIn, iInLength, szDecryptBody, 
			iDecryptLength))
		{
			return fail;
		}

		//更新pszTmp，指向解密后的code部分
		char* pszTmp = szDecryptBody;
		int32_t iTmpSize = iDecryptLength;

		if (nUnCompressAlgorithm != enmCompressAlgorithm_None)
		{
			char szUncompressedBody[max_cs_package_body_size];
			int32_t iUncompressedLength = (int32_t) sizeof(szUncompressedBody);

			if (success != uncompress_buffer(nUnCompressAlgorithm, (const char*)szDecryptBody, (const int32_t)iDecryptLength, szUncompressedBody, 
				iUncompressedLength))
			{
				return fail;
			}

			//解压缩后，再重新更新指针 
			pszTmp = szUncompressedBody;
			iTmpSize = iUncompressedLength;
		}

		//将解密解压缩的数据copy到输出Buffer
		if (iTmpSize <= iOutLength)
		{
			memcpy(pszOut, pszTmp, iTmpSize);
			iOutLength = iTmpSize;
		}
		else
		{
			return fail;
		}

		return success;
	}

	
	int32_t encode_profile_info( char** pszOut, stCryptUserBaseWebProfile& info )
	{	
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, (uint32_t)info.m_iFlag);
		size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, (uint32_t)info.m_iVIPScore);
		size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, (uint32_t)info.m_iVIPLevel);
		size += coded_length;

		*pszOut -= (size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, size);

		iOutLength = (size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}
	
	int32_t decode_profile_info( char** pszIn, stCryptUserBaseWebProfile& info )
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t size = 0;
		int32_t iInLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&size);
		iInLength += decode_length;

		if (size <= 0)
		{
			return 0;
		}

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iFlag);
		iInLength += decode_length;
		size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iVIPScore);
		iInLength += decode_length;
		size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iVIPLevel);
		iInLength += decode_length;
		size -= decode_length;

		if (size < 0)
		{
			return 0;
		}
		else if (size > 0)
		{
			(*pszIn) += size;
			iInLength += size;
		}

		return iInLength;
	}

	int32_t encode_profile_webscore( char** pszOut, stCryptUserWebScore& info )
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, (uint32_t)info.m_i51Score);
		size += coded_length;
		
		coded_length = CCodeEngine::encode_int32(pszOut, (uint32_t)info.m_i51Level);
		size += coded_length;

		*pszOut -= (size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, size);

		iOutLength = (size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;

	}

	int32_t decode_profile_webscore( char** pszIn, stCryptUserWebScore& info )
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t size = 0;
		int32_t iInLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&size);
		iInLength += decode_length;

		if (size <= 0)
		{
			return 0;
		}

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_i51Score);
		size -= decode_length;
		iInLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_i51Level);
		size -= decode_length;
		iInLength += decode_length;

		if (size < 0)
		{
			return 0;
		}
		else if (size > 0)
		{
			(*pszIn) += size;
			iInLength += size;
		}

		return iInLength;
	}

	//解码200906新版本签名
	int32_t decode_player_signature_ex_200906(char *pszIn, int16_t nInlength, int16_t nSignatureCryptAlgorithm, 
		CPlayerSignatureEx200906 &stSignature)
	{
		if (nInlength > max_signature_length || NULL == pszIn)	
		{
			return fail;
		}

		//得到与client通讯时加密密钥
		int16_t nDecodedLength = 0;
		int16_t nCrypKeyLength = crypt_key_length;

		memcpy(stSignature.m_szKey, &pszIn[0], nCrypKeyLength);
		nDecodedLength += crypt_key_length;

		//得到用户uin
		stSignature.m_iUin = (int32_t)ntohl(*(uint32_t*)(pszIn + nDecodedLength));
		nDecodedLength += sizeof(stSignature.m_iUin);

		//得到用户UID
		int8_t uid_length = (int8_t)pszIn[nDecodedLength];
		if (uid_length <= 0 || uid_length > (int8_t)sizeof(stSignature.m_szUid))
		{
			return fail;
		}
		nDecodedLength += sizeof(int8_t);

		memcpy(stSignature.m_szUid, &pszIn[nDecodedLength], size_t(uid_length));
		nDecodedLength += uid_length;

		//得到用户ExtraUID
		int8_t extra_uid_length = (int8_t)pszIn[nDecodedLength];
		if (extra_uid_length <= 0 || extra_uid_length > (int8_t)sizeof(stSignature.m_szExtraUid))
		{
			return fail;
		}
		nDecodedLength += sizeof(int8_t);

		memcpy(stSignature.m_szExtraUid, &pszIn[nDecodedLength], size_t(extra_uid_length));
		nDecodedLength += extra_uid_length;

		//goodId
		stSignature.m_nGoodId = (int32_t)ntohl(*(uint32_t*)(pszIn + nDecodedLength));
		nDecodedLength += sizeof(stSignature.m_nGoodId);

		//得到用户的服务标识信息
		memcpy(stSignature.m_szServiceBitmap, (const void*)&pszIn[nDecodedLength], sizeof(stSignature.m_szServiceBitmap));
		nDecodedLength += sizeof(stSignature.m_szServiceBitmap);

		//得到签名的时间戳
		stSignature.m_iTimeStamp = (int32_t)ntohl(*(uint32_t*)(pszIn + nDecodedLength));
		nDecodedLength += sizeof(stSignature.m_iTimeStamp);

		if (nDecodedLength > nInlength)
		{
			return fail;
		}

		return success;
	}

	CPlayerSignatureEx200906::CPlayerSignatureEx200906()
	{
		memset(m_szKey, 0, sizeof(m_szKey));
		memset(m_szUid, 0, sizeof(m_szUid));
		memset(m_szExtraUid, 0, sizeof(m_szExtraUid));
		memset(m_szServiceBitmap, 0, sizeof(m_szServiceBitmap));

		m_iUin = 0;
		m_nGoodId = 0;
		m_iTimeStamp = 0;
		m_nSignatureFlag = 0;
	}

	CPlayerSignatureEx200906::~CPlayerSignatureEx200906()
	{

	}

	int32_t CPlayerSignatureEx200906::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		//size
		ptmp += sizeof(int16_t); //为长度预留2Byte的空间
		iOutLength += sizeof(int16_t);

		//key
		coded_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szKey, (const int32_t)crypt_key_length);
		iOutLength += coded_length;

		//uin
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		//UID
		coded_length = CCodeEngine::encode_string(&ptmp, m_szUid, (const int16_t)sizeof(m_szUid));
		iOutLength += coded_length;

		//ExtraUID
		coded_length = CCodeEngine::encode_string(&ptmp, m_szExtraUid, (const int16_t)sizeof(m_szExtraUid));
		iOutLength += coded_length;

		//good id
		coded_length = CCodeEngine::encode_int32(&ptmp, m_nGoodId);
		iOutLength += coded_length;

		//Service Bitmap
		int8_t nServiceBitmapLen = sizeof(m_szServiceBitmap);
		coded_length = CCodeEngine::encode_memory(&ptmp, m_szServiceBitmap, nServiceBitmapLen);
		iOutLength += coded_length;

		//Time Stamp
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iTimeStamp);
		iOutLength += coded_length;

		//size
		ptmp -= iOutLength;
		CCodeEngine::encode_int16(&ptmp, iOutLength);	//长度包含size自身

		return success;
	}

	int32_t CPlayerSignatureEx200906::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = 0;
		int32_t decoded_length = 0;

		//size
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nSize);
		//初始化
		remained_length = m_nSize - decoded_length;

		//key
		decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)m_szKey, (const int32_t)crypt_key_length);
		remained_length -= decoded_length;

		//uin
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
		remained_length -= decoded_length;

		//UID
		decoded_length = CCodeEngine::decode_string(&ptmp, (char*)m_szUid, (const int16_t)sizeof(m_szUid));
		remained_length -= decoded_length;

		//ExtraUID
		decoded_length = CCodeEngine::decode_string(&ptmp, (char*)m_szExtraUid, (const int16_t)sizeof(m_szExtraUid));
		remained_length -= decoded_length;

		//good id
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nGoodId);
		remained_length -= decoded_length;

		//Service Bitmap
		int8_t nServiceBitmapLen = sizeof(m_szServiceBitmap);
		decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)m_szServiceBitmap, (const int32_t)nServiceBitmapLen);
		remained_length -= decoded_length;

		//Time Stamp
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTimeStamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	bool is_game_vip(int32_t nVipType, int32_t iVipExpireTime)
	{
		if ( (vip_type_honorary & nVipType) ||
				(vip_type_mobile & nVipType) )
		{
			return true;
		}
		else if (vip_type_normal & nVipType)
		{
			time_t tmNow = 0;
			time(&tmNow);
			if (iVipExpireTime > tmNow )
			{
				return true;
			}
		}

		return false;
	}
}}
