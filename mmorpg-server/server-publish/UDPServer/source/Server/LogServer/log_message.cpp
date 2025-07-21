#include "Common/common_base.h"
#include "Common/common_codeengine.h"

#include "log_common_define.h"
#include "log_message.h"
#include "public/server_log_message_body.h"
#include "public/server_public_define.h"
#include "public/server_message.h"
using namespace KingNet::Server;
namespace KingNet { namespace Server { namespace LogServer{

	CMessage::CMessage()
	{
		m_pstBody = NULL;
	}

	CMessage::~CMessage()
	{
		if (NULL != m_pstBody)
		{
			delete m_pstBody;
			m_pstBody = NULL;
		}
	}

	int32_t CMessage::encode(char *pszOut, int32_t &iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		if (success != m_stHead.encode(ptmp, coded_length))
		{
			return fail;
		}

		iOutLength += coded_length;
		ptmp += coded_length;

		if (NULL == m_pstBody) //如果没有协议体，则直接返回
		{
			return success;
		}

		if (success != m_pstBody->encode(ptmp, coded_length))
		{
			return fail;
		}

		iOutLength += coded_length;
		ptmp += coded_length;

		return success;
	}

	int32_t CMessage::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = (char*)pszIn;
		int32_t remained_length = iInLength;

		//int32_t decoded_length = 0;

		if (success != m_stHead.decode((const char*)ptmp, remained_length))
		{
			return fail;
		}

		remained_length -= MESSAGE_HEAD_SIZE;
		ptmp += MESSAGE_HEAD_SIZE;

		if ( 0 >= remained_length)
		{
			if (NULL != m_pstBody)
			{
				delete m_pstBody;
				m_pstBody = NULL;
			}

			//若，没有参数，则直接返回成功
			if (0 == remained_length)
			{
				return success;
			}

			return fail;
		}

		if (success != create_message_body())
		{
			return fail;
		}

		return m_pstBody->decode(ptmp, remained_length);
	}

	void CMessage::dump()
	{

	}

	int32_t CMessage::create_message_body()
	{
		if (NULL != m_pstBody)
		{
			delete m_pstBody;
			m_pstBody = NULL;
		}

		m_pstBody = create_body(m_stHead.m_iMessageID, m_stHead.m_nMessageType);
		if (NULL == m_pstBody)
		{
			return fail;
		}

		return success;

	}

	CMessageBody* CMessage::create_body(int32_t iMessageID, int8_t iMessageType)
	{
		CMessageBody* result = NULL;
		switch (iMessageType)
		{
		case MSG_TYPE_REQUEST:
			result = create_request_message_body(iMessageID);
			break;
		case MSG_TYPE_RESPONSE:
			result = create_response_message_body(iMessageID);
			break;
		case MSG_TYPE_NOTIFY:
			result = create_notify_message_body(iMessageID);
			break;
		case MSG_TYPE_OTHER:
			result = create_other_message_body(iMessageID);
			break;
		default:
			break;
		}

		return result;
	}

	CMessageBody* CMessage::create_request_message_body(int32_t iMessageID)
	{
		CMessageBody* pstBody = NULL;
		switch(iMessageID)
		{
		
		default:
			pstBody = NULL;
			break;
		}

		return pstBody;
	}

	CMessageBody* CMessage::create_response_message_body(int32_t iMessageID)
	{
		CMessageBody* pstBody = NULL;
		switch(iMessageID)
		{
	

		default:
			pstBody = NULL;
			break;
		}

		return pstBody;
	}

	CMessageBody* CMessage::create_notify_message_body(int32_t iMessageID)
	{
		CMessageBody* pstBody = NULL;
		switch(iMessageID)
		{
	
		default:
			pstBody = NULL;
			break;
		}

		return pstBody;
	}

	CMessageBody* CMessage::create_other_message_body(int32_t iMessageID)
	{
		CMessageBody* pstBody = NULL;
		switch (iMessageID)
		{
	
		default:
			break;
		}

		return pstBody;
	}


	//////////////////////////////////////////////////////////////////////////
	// CMessageEntry
	//////////////////////////////////////////////////////////////////////////

	IMPLEMENT_DYN_CREATE(CMessageEntry)   //与DECLARE_DYN_CREATE(CMessageEntry)对应的实现

		CMessageEntry::CMessageEntry()
	{
		if (object_createmode_init == CObject::m_iCreateMode)
		{
			initialize();
		}
		else
		{
			resume();
		}
	}

	CMessageEntry::~CMessageEntry()
	{
		//do nothing
	}

	int32_t CMessageEntry::initialize()
	{
		m_iMessageID		 = invalid_message_id;
		m_nMessageType		 = invalid_message_type;
		m_bSourceEntityType = entity_type_invalid;
		m_bDestEntityType	 = entity_type_invalid;
		m_iSourceID		 = invalid_object_id;
		m_iDestID			 = invalid_object_id;
		m_iMessageSequence  = -1;

		m_iBodySize = 0;
		m_iBodyOffsetInQueue = -1;

		return success;
	}

	int32_t CMessageEntry::resume()
	{
		return success;
	}

	int32_t CMessageEntry::get_key(void* key, int32_t &keylength)
	{
		return success;
	}

	int32_t CMessageEntry::set_key(const void* key, int32_t keylength)
	{
		return success;
	}

	int32_t CMessageEntry::dump(int32_t handle)
	{
		return success;
	}

}}}//namespace KingNet { namespace Server { namespace LogServer{
