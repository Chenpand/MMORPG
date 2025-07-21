#include "gameplatform_public.h"
#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/server_group_chat_message_body.h"

namespace KingNet { namespace Server {  
	//TIMEOUT
	//COtherTimeout::COtherTimeout()
	//{
	//	m_nTimerMark = -1;
	//	m_cAdditionSize = 0;
	//}

	//COtherTimeout::~COtherTimeout()
	//{
	//	//do nothing
	//}

	//void COtherTimeout::dump()
	//{
	//	//do nothing
	//}

	//int32_t COtherTimeout::encode(char* pszOut, int32_t& iOutLength)
	//{
	//	//该函数编码时，头2B是整个body的长度，包含这两个字节本身
	//	//

	//	if (NULL == pszOut)
	//	{
	//		return fail;
	//	}

	//	if (m_cAdditionSize > (int8_t)sizeof(m_stAdditionInfo))
	//	{
	//		return fail;
	//	}

	//	char* ptmp = pszOut;
	//	int32_t encode_length = 0;

	//	iOutLength = 0;

	//	//此处预留2B
	//	iOutLength += sizeof(int16_t);
	//	ptmp += iOutLength;

	//	encode_length = CCodeEngine::encode_int16(&ptmp, m_nTimerMark);
	//	iOutLength += encode_length;

	//	encode_length = CCodeEngine::encode_int8(&ptmp, m_cAdditionSize);
	//	iOutLength += encode_length;

	//	if (m_cAdditionSize > 0)
	//	{
	//		encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_stAdditionInfo.m_abyOtherInfo, (const int32_t)m_cAdditionSize);
	//		iOutLength += encode_length;
	//	}

	//	ptmp = pszOut;
	//	encode_length = CCodeEngine::encode_int16(&ptmp, iOutLength); //把总长度打入buffer

	//	return success;
	//}

	//int32_t COtherTimeout::decode(const char* pszIn, const int32_t iInLength)
	//{
	//	if (NULL == pszIn || iInLength <= 0)
	//	{
	//		return fail;
	//	}

	//	char* ptmp = const_cast<char*>(pszIn);
	//	int16_t remain_length  = iInLength;
	//	int32_t decode_length = 0;

	//	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&remain_length);
	//	if (remain_length > iInLength)
	//	{
	//		return fail;
	//	}

	//	remain_length -= decode_length;

	//	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTimerMark);
	//	remain_length -= decode_length;

	//	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cAdditionSize);
	//	remain_length -= decode_length;

	//	if ((size_t)m_cAdditionSize > sizeof(m_stAdditionInfo))
	//	{
	//		return fail;
	//	}

	//	if (m_cAdditionSize > 0)
	//	{
	//		decode_length = CCodeEngine::decode_memory(&ptmp, (char*)m_stAdditionInfo.m_abyOtherInfo, (const int32_t)m_cAdditionSize);
	//		remain_length -= decode_length;
	//	}

	//	if (remain_length < 0)
	//	{
	//		return fail;
	//	}

	//	return success;
	//}

	CCSRequestGetChatRoomOnlineCount::CCSRequestGetChatRoomOnlineCount()
	{
		m_iUid = 0;
		m_iChatRoomIDCount = 0;
		memset(m_szChatRoomID,0,sizeof(m_szChatRoomID));
	}

	CCSRequestGetChatRoomOnlineCount::~CCSRequestGetChatRoomOnlineCount()
	{

	}

	int32_t CCSRequestGetChatRoomOnlineCount::encode(char *pszOut, int32_t& iOutLength)
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

		if( m_iChatRoomIDCount > max_chat_room_count )
		{
			m_iChatRoomIDCount = max_chat_room_count;
		}
		encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_iChatRoomIDCount);
		iOutLength += encode_length;

		for( int32_t i=0;i<m_iChatRoomIDCount;++i)
		{
			encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_szChatRoomID[i]);
			iOutLength += encode_length;
		}

		return success;
	}
	int32_t CCSRequestGetChatRoomOnlineCount::decode(const char *pszIn, const int32_t iInLength)
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

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChatRoomIDCount);
		remain_length -= decode_length;

		if( m_iChatRoomIDCount > max_chat_room_count )
		{
			m_iChatRoomIDCount = max_chat_room_count;
		}
		
		for(int32_t i=0; i<m_iChatRoomIDCount;++i)
		{
			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szChatRoomID[i]);
			remain_length -= decode_length;
		}

		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CCSRequestGetChatRoomOnlineCount::dump()
	{

	}


	CCSResponseGetChatRoomOnlineCount::CCSResponseGetChatRoomOnlineCount()
	{
		m_shResultID = 0;
		m_iGameRoomCount = 0;
		memset(m_szGroupRoom,0,sizeof(m_szGroupRoom));
		memset(m_szReason,0,sizeof(m_szReason));
	}
	CCSResponseGetChatRoomOnlineCount::~CCSResponseGetChatRoomOnlineCount()
	{

	}
	int32_t CCSResponseGetChatRoomOnlineCount::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shResultID);
		iOutLength += encode_length;

		if( result_id_success != m_shResultID)
		{
			encode_length = CCodeEngine::encode_string(&ptmp,m_szReason,sizeof(m_szReason));
			iOutLength += encode_length;
		}
		else
		{
			if( m_iGameRoomCount > max_chat_room_count)
			{
				m_iGameRoomCount = max_chat_room_count;
			}
			encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameRoomCount);
			iOutLength += encode_length;

			for(int32_t i=0; i< m_iGameRoomCount;++i)
			{
				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szGroupRoom[i].m_iGroupID);
				iOutLength += encode_length;
				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szGroupRoom[i].m_iPlayerCount);
				iOutLength += encode_length;
			}
		}
		return success;
	}
	int32_t CCSResponseGetChatRoomOnlineCount::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remain_length -= decode_length;

		if( result_id_success != m_shResultID )
		{
			decode_length = CCodeEngine::decode_string(&ptmp, m_szReason,sizeof(m_szReason));
			remain_length -= decode_length;
		}
		else
		{
			if( m_iGameRoomCount > max_chat_room_count)
			{
				m_iGameRoomCount = max_chat_room_count;
			}
			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameRoomCount);
			remain_length -= decode_length;
			for(int32_t i=0;i<max_chat_room_count;++i)
			{
				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szGroupRoom[i].m_iGroupID);
				remain_length -= decode_length;
				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szGroupRoom[i].m_iPlayerCount);
				remain_length -= decode_length;
			}
		}
		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CCSResponseGetChatRoomOnlineCount::dump()
	{

	}
////CS_MSG_LOGIN_CHAT_ROOM  登录聊天室
CCSRequestLoginGroupChatRoom::CCSRequestLoginGroupChatRoom()
{
	m_iUid = 0;
	m_iChatRoomID = 0;
	m_shPublishInfoLen = 0;
	memset(m_pbyUserBaseInfo,0,sizeof(m_pbyUserBaseInfo));

}
CCSRequestLoginGroupChatRoom::~CCSRequestLoginGroupChatRoom()
{

}

int32_t CCSRequestLoginGroupChatRoom::encode(char *pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_iChatRoomID);
	iOutLength += encode_length;


	if( m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}
	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shPublishInfoLen);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestLoginGroupChatRoom::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iChatRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shPublishInfoLen);
	remain_length -= decode_length;

	if(m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}
	decode_length = CCodeEngine::decode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CCSRequestLoginGroupChatRoom::dump()
{

}


CCSResponseLoginGroupChatRoom::CCSResponseLoginGroupChatRoom()
{
	m_shResultID = result_id_success;
	m_szReason[0] = '\0';

	m_iChatRoomID = 0;
	m_shPlayerCount = 0;
	memset(m_szPlayer,0,sizeof(m_szPlayer));
}
int32_t CCSResponseLoginGroupChatRoom::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shResultID);
	iOutLength += encode_length;

	if( result_id_success != m_shResultID)
	{
		encode_length = CCodeEngine::encode_string(&ptmp,m_szReason,sizeof(m_szReason));
		iOutLength += encode_length;
	}
	else
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChatRoomID);
		iOutLength += encode_length;

		if( m_shPlayerCount > max_player_count )
		{
			m_shPlayerCount = max_player_count;
		}
		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shPlayerCount);
		iOutLength += encode_length;

		for(int32_t i=0; i< m_shPlayerCount ; ++i)
		{
			encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_szPlayer[i].m_iUid);
			iOutLength += encode_length;


			if( m_szPlayer[i].m_shPublishInfoLen > max_user_base_info_length)
			{
				m_szPlayer[i].m_shPublishInfoLen = max_user_base_info_length;
			}
			encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_szPlayer[i].m_shPublishInfoLen);
			iOutLength += encode_length;

			encode_length = CCodeEngine::encode_memory(&ptmp,m_szPlayer[i].m_pbyUserBaseInfo,m_szPlayer[i].m_shPublishInfoLen);
			iOutLength += encode_length;
		}
	}
	return success;
}
int32_t CCSResponseLoginGroupChatRoom::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
	remain_length -= decode_length;

	if( result_id_success != m_shResultID )
	{
		decode_length = CCodeEngine::decode_string(&ptmp, m_szReason,sizeof(m_szReason));
		remain_length -= decode_length;
	}
	else
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChatRoomID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shPlayerCount);
		remain_length -= decode_length;

		if( m_shPlayerCount > max_player_count)
		{
			m_shPlayerCount = max_player_count;
		}

		for(int32_t i=0; i< m_shPlayerCount; ++i)
		{
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_szPlayer[i].m_iUid);
			remain_length -= decode_length;

			decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_szPlayer[i].m_shPublishInfoLen);
			remain_length -= decode_length;

			if(m_szPlayer[i].m_shPublishInfoLen > max_user_base_info_length)
			{
				m_szPlayer[i].m_shPublishInfoLen = max_user_base_info_length;
			}
			decode_length = CCodeEngine::decode_memory(&ptmp,m_szPlayer[i].m_pbyUserBaseInfo,m_szPlayer[i].m_shPublishInfoLen);
			remain_length -= decode_length;
		}

	}
	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSResponseLoginGroupChatRoom::dump()
{

}

CCSResponseLoginGroupChatRoom::~CCSResponseLoginGroupChatRoom()
{

}







//CS_MSG_NOTIFY_START_CHAT 通知开始聊天
CCSNotifyLoginLogoutChatRoom::CCSNotifyLoginLogoutChatRoom()
{
	m_iSrcUid = 0;
	m_cType = 0;
	m_shPublishInfoLen = 0;
	m_pbyUserBaseInfo[0] = '\0';
}
int32_t CCSNotifyLoginLogoutChatRoom::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSrcUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cType);
	iOutLength += encode_length;

	if( m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}
	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shPublishInfoLen);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	iOutLength += encode_length;

	return success;
}
int32_t CCSNotifyLoginLogoutChatRoom::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cType);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shPublishInfoLen);
	remain_length -= decode_length;
	if( m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSNotifyLoginLogoutChatRoom::dump()
{

}

CCSNotifyLoginLogoutChatRoom::~CCSNotifyLoginLogoutChatRoom()
{

}

	//CS_MSG_TRANSPORT_CHAT_INFO 中转聊天记录
CCSNotifyTransportGroupChatInfo::CCSNotifyTransportGroupChatInfo()
{
	m_iSrcUid = 0;
	m_iChatRoomID = 0;
	m_shChatContentLen = 0;
	m_pbyChatContent[0] = '\0';
	m_shPublishInfoLen = 0;
	m_pbyUserBaseInfo[0] = '\0';
}
CCSNotifyTransportGroupChatInfo::~CCSNotifyTransportGroupChatInfo()
{

}
int32_t CCSNotifyTransportGroupChatInfo::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSrcUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChatRoomID);
	iOutLength += encode_length;

	if( m_shChatContentLen > max_chat_content )
	{
		m_shChatContentLen = max_chat_content;
	}
	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shChatContentLen);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_pbyChatContent,m_shChatContentLen);
	iOutLength += encode_length;

	if( m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}
	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shPublishInfoLen);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	iOutLength += encode_length;


	return success;
}
int32_t CCSNotifyTransportGroupChatInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChatRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shChatContentLen);
	remain_length -= decode_length;
	if( m_shChatContentLen > max_chat_content )
	{
		m_shChatContentLen = max_chat_content;
	}
	decode_length = CCodeEngine::decode_memory(&ptmp,m_pbyChatContent,m_shChatContentLen);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shPublishInfoLen);
	remain_length -= decode_length;
	if( m_shPublishInfoLen > max_user_base_info_length)
	{
		m_shPublishInfoLen = max_user_base_info_length;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp,m_pbyUserBaseInfo,m_shPublishInfoLen);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSNotifyTransportGroupChatInfo::dump()
{

}

//CS_MSG_STOP_CHAT 退出聊天,TCP不断开

CCSRequestStopGroupChat::CCSRequestStopGroupChat()
{
	m_iSrcUid = 0;
	m_iChatRoomID = 0;
}
CCSRequestStopGroupChat::~CCSRequestStopGroupChat()
{

}
int32_t CCSRequestStopGroupChat::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSrcUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChatRoomID);
	iOutLength += encode_length;
	return success;
}
int32_t CCSRequestStopGroupChat::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iChatRoomID);
	remain_length -= decode_length;


	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestStopGroupChat::dump()
{

}

CCSResponseStopGroupChat::CCSResponseStopGroupChat()
{
	m_shResultID = result_id_success;
	m_szReason[0] = '\0';

}
int32_t CCSResponseStopGroupChat::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shResultID);
	iOutLength += encode_length;

	if( result_id_success != m_shResultID)
	{
		encode_length = CCodeEngine::encode_string(&ptmp,m_szReason,sizeof(m_szReason));
		iOutLength += encode_length;
	}
	return success;
}
int32_t CCSResponseStopGroupChat::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
	remain_length -= decode_length;

	if( result_id_success != m_shResultID )
	{
		decode_length = CCodeEngine::decode_string(&ptmp, m_szReason,sizeof(m_szReason));
		remain_length -= decode_length;
	}
	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSResponseStopGroupChat::dump()
{

}

CCSResponseStopGroupChat::~CCSResponseStopGroupChat()
{

}



	//CS_MSG_LOGOUT_CHAT_ROOM 退出聊天室，断开TCP
CCSRequestLogoutGroupChatRoom::CCSRequestLogoutGroupChatRoom()
{
	m_iSrcUid = 0;
	m_iChatRoomID = 0;
}
CCSRequestLogoutGroupChatRoom::~CCSRequestLogoutGroupChatRoom()
{

}
int32_t CCSRequestLogoutGroupChatRoom::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSrcUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChatRoomID);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestLogoutGroupChatRoom::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChatRoomID);
	remain_length -= decode_length;


	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestLogoutGroupChatRoom::dump()
{

}




}}
