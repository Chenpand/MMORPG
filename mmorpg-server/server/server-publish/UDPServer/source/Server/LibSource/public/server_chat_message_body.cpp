#include "gameplatform_public.h"
#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/server_chat_message_body.h"

namespace KingNet { namespace Server {  
	//TIMEOUT
	COtherTimeout::COtherTimeout()
	{
		m_nTimerMark = -1;
		m_cAdditionSize = 0;
	}

	COtherTimeout::~COtherTimeout()
	{
		//do nothing
	}

	void COtherTimeout::dump()
	{
		//do nothing
	}

	int32_t COtherTimeout::encode(char* pszOut, int32_t& iOutLength)
	{
		//该函数编码时，头2B是整个body的长度，包含这两个字节本身
		//

		if (NULL == pszOut)
		{
			return fail;
		}

		if (m_cAdditionSize > (int8_t)sizeof(m_stAdditionInfo))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;

		iOutLength = 0;

		//此处预留2B
		iOutLength += sizeof(int16_t);
		ptmp += iOutLength;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_nTimerMark);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int8(&ptmp, m_cAdditionSize);
		iOutLength += encode_length;

		if (m_cAdditionSize > 0)
		{
			encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_stAdditionInfo.m_abyOtherInfo, (const int32_t)m_cAdditionSize);
			iOutLength += encode_length;
		}

		ptmp = pszOut;
		encode_length = CCodeEngine::encode_int16(&ptmp, iOutLength); //把总长度打入buffer

		return success;
	}

	int32_t COtherTimeout::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&remain_length);
		if (remain_length > iInLength)
		{
			return fail;
		}

		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTimerMark);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cAdditionSize);
		remain_length -= decode_length;

		if ((size_t)m_cAdditionSize > sizeof(m_stAdditionInfo))
		{
			return fail;
		}

		if (m_cAdditionSize > 0)
		{
			decode_length = CCodeEngine::decode_memory(&ptmp, (char*)m_stAdditionInfo.m_abyOtherInfo, (const int32_t)m_cAdditionSize);
			remain_length -= decode_length;
		}

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

////CS_MSG_LOGIN_CHAT_ROOM  登录聊天室
CCSRequestLoginChatRoom::CCSRequestLoginChatRoom()
{
	m_iUid = 0;
	m_cSex = sex_type_no_type;
	m_cPublishInfoFlag = no_publish_info;
	m_shPublishInfoLen = 0;
	memset(m_pbyUserBaseInfo,0,sizeof(m_pbyUserBaseInfo));

}
int32_t CCSRequestLoginChatRoom::encode(char *pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cSex);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cPublishInfoFlag);
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
int32_t CCSRequestLoginChatRoom::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cSex);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cPublishInfoFlag);
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
CCSRequestLoginChatRoom& CCSRequestLoginChatRoom::operator =(const CCSRequestLoginChatRoom& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	m_cSex = original.m_cSex;
	m_cPublishInfoFlag = original.m_cPublishInfoFlag;
	m_shPublishInfoLen = original.m_shPublishInfoLen;
	STRNCPY(m_pbyUserBaseInfo, original.m_pbyUserBaseInfo, sizeof(m_pbyUserBaseInfo));
	return *this;
}
void CCSRequestLoginChatRoom::dump()
{

}
CCSRequestLoginChatRoom::~CCSRequestLoginChatRoom()
{

}

CCSResponseLoginChatRoom::CCSResponseLoginChatRoom()
{
	m_shResultID = result_id_success;
	m_szReason[0] = '\0';
}
int32_t CCSResponseLoginChatRoom::encode(char* pszOut, int32_t& iOutLength)
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
int32_t CCSResponseLoginChatRoom::decode(const char *pszIn, const int32_t iInLength)
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
void CCSResponseLoginChatRoom::dump()
{

}
CCSResponseLoginChatRoom& CCSResponseLoginChatRoom::operator =(const CCSResponseLoginChatRoom& original)
{

	if (this == &original)
	{
		return *this;
	}
	m_shResultID = original.m_shResultID;
	STRNCPY(m_szReason, original.m_szReason, sizeof(m_szReason));
	return *this;
}
CCSResponseLoginChatRoom::~CCSResponseLoginChatRoom()
{

}

	//CS_MSG_START_CHAT   发起聊天 
CCSRequestStartChat::CCSRequestStartChat()
{
	m_iUid = 0;
	m_cDestSex = sex_type_no_type;
	m_iChatContentType = 0;
	m_cPublishInfoFlag = no_publish_info;
}
int32_t CCSRequestStartChat::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cDestSex);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_iChatContentType);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cPublishInfoFlag);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestStartChat::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cDestSex);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iChatContentType);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cPublishInfoFlag);
	remain_length -= decode_length;


	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestStartChat::dump()
{

}
CCSRequestStartChat& CCSRequestStartChat::operator =(const CCSRequestStartChat& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	m_cDestSex = original.m_cDestSex;
	m_iChatContentType = original.m_iChatContentType;
	m_cPublishInfoFlag = original.m_cPublishInfoFlag;
	return *this;
}
CCSRequestStartChat::~CCSRequestStartChat()
{

}

//CS_MSG_NOTIFY_START_CHAT 通知开始聊天
CCSNotifyStartChat::CCSNotifyStartChat()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
	m_cDestSex = sex_type_no_type;
	m_cPublishInfoFlag = no_publish_info;
	m_shPublishInfoLen = 0;
	m_pbyUserBaseInfo[0] = '\0';
}
int32_t CCSNotifyStartChat::encode(char* pszOut, int32_t& iOutLength)
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
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cDestSex);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cPublishInfoFlag);
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
int32_t CCSNotifyStartChat::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cDestSex);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cPublishInfoFlag);
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
void CCSNotifyStartChat::dump()
{

}
CCSNotifyStartChat& CCSNotifyStartChat::operator =(const CCSNotifyStartChat& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	m_cDestSex = original.m_cDestSex;
	m_cPublishInfoFlag = original.m_cPublishInfoFlag;
	m_shPublishInfoLen = original.m_shPublishInfoLen;
	STRNCPY(m_pbyUserBaseInfo, original.m_pbyUserBaseInfo, sizeof(m_pbyUserBaseInfo));
	return *this;
}
CCSNotifyStartChat::~CCSNotifyStartChat()
{

}
	//CS_MSG_TRANSPORT_CHAT_INFO 中转聊天记录
CCSNotifyTransportChatInfo::CCSNotifyTransportChatInfo()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
	m_shChatContentLen = 0;
	m_pbyChatContent[0] = '\0';
}
CCSNotifyTransportChatInfo::~CCSNotifyTransportChatInfo()
{

}
int32_t CCSNotifyTransportChatInfo::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;

	if( m_shChatContentLen > max_chat_content )
	{
		m_shChatContentLen = max_chat_content;
	}
	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shChatContentLen);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_pbyChatContent,m_shChatContentLen);
	iOutLength += encode_length;

	return success;
}
int32_t CCSNotifyTransportChatInfo::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shChatContentLen);
	remain_length -= decode_length;
	if( m_shChatContentLen > max_chat_content )
	{
		m_shChatContentLen = max_chat_content;
	}
	decode_length = CCodeEngine::decode_memory(&ptmp,m_pbyChatContent,m_shChatContentLen);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSNotifyTransportChatInfo::dump()
{

}
CCSNotifyTransportChatInfo& CCSNotifyTransportChatInfo::operator =(const CCSNotifyTransportChatInfo& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	m_shChatContentLen = original.m_shChatContentLen;
	STRNCPY(m_pbyChatContent, original.m_pbyChatContent, sizeof(m_pbyChatContent));
	return *this;
}

//CS_MSG_PUBLISH_PRIVATE_INFO  公开个人信息
CCSRequestPublishPrivateInfo::CCSRequestPublishPrivateInfo()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
	m_cPublishInfoFlag = no_publish_info;
}
CCSRequestPublishPrivateInfo::~CCSRequestPublishPrivateInfo()
{

}
int32_t CCSRequestPublishPrivateInfo::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cPublishInfoFlag);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestPublishPrivateInfo::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cPublishInfoFlag);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestPublishPrivateInfo::dump()
{

}
CCSRequestPublishPrivateInfo& CCSRequestPublishPrivateInfo::operator =(const CCSRequestPublishPrivateInfo& original)
{

	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	m_cPublishInfoFlag = original.m_cPublishInfoFlag;
	return *this;
}

CCSNotifyPublishPrivateInfo::CCSNotifyPublishPrivateInfo()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
	m_cPublishInfoFlag = no_publish_info;
	m_shPublishInfoLen = 0;
	m_pbyUserBaseInfo[0] = '\0';
}
CCSNotifyPublishPrivateInfo::~CCSNotifyPublishPrivateInfo()
{

}
int32_t CCSNotifyPublishPrivateInfo::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_cPublishInfoFlag);
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
int32_t CCSNotifyPublishPrivateInfo::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cPublishInfoFlag);
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
void CCSNotifyPublishPrivateInfo::dump()
{

}
CCSNotifyPublishPrivateInfo& CCSNotifyPublishPrivateInfo::operator =(const CCSNotifyPublishPrivateInfo& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	m_cPublishInfoFlag = original.m_cPublishInfoFlag;
	m_shPublishInfoLen = original.m_shPublishInfoLen;
	STRNCPY(m_pbyUserBaseInfo, original.m_pbyUserBaseInfo, sizeof(m_pbyUserBaseInfo));
	return *this;
}
	//CS_MSG_CHAT_HEART 聊天心跳包
CCSNotifyChatHeart::CCSNotifyChatHeart()
{
	m_iUid = 0;
}
CCSNotifyChatHeart::~CCSNotifyChatHeart()
{

}
int32_t CCSNotifyChatHeart::encode(char* pszOut, int32_t& iOutLength)
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

	return success;
}
int32_t CCSNotifyChatHeart::decode(const char *pszIn, const int32_t iInLength)
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

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSNotifyChatHeart::dump()
{

}
CCSNotifyChatHeart& CCSNotifyChatHeart::operator =(const CCSNotifyChatHeart& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	return *this;
}
	//CS_MSG_STOP_CHAT 退出聊天,TCP不断开

CCSRequestStopChat::CCSRequestStopChat()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
}
CCSRequestStopChat::~CCSRequestStopChat()
{

}
int32_t CCSRequestStopChat::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;
	return success;
}
int32_t CCSRequestStopChat::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDestUid);
	remain_length -= decode_length;


	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestStopChat::dump()
{

}
CCSRequestStopChat& CCSRequestStopChat::operator =(const CCSRequestStopChat& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	return *this;
}

	//CS_MSG_LOGOUT_CHAT_ROOM 退出聊天室，断开TCP
CCSRequestLogoutChatRoom::CCSRequestLogoutChatRoom()
{
	m_iSrcUid = 0;
	m_iDestUid = 0;
}
CCSRequestLogoutChatRoom::~CCSRequestLogoutChatRoom()
{

}
int32_t CCSRequestLogoutChatRoom::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iDestUid);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestLogoutChatRoom::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUid);
	remain_length -= decode_length;


	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestLogoutChatRoom::dump()
{

}
CCSRequestLogoutChatRoom& CCSRequestLogoutChatRoom::operator =(const CCSRequestLogoutChatRoom& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iSrcUid = original.m_iSrcUid;
	m_iDestUid = original.m_iDestUid;
	return *this;
}

//CS_MSG_NOTIFY_CLOSE_CHAT 通知聊天结束
CCSNotifyCloseChat::CCSNotifyCloseChat()
{
	m_iUid = 0;
	m_shReasonID = 0;
	m_szReason[0] = '\0';
}
CCSNotifyCloseChat::~CCSNotifyCloseChat()
{

}
int32_t CCSNotifyCloseChat::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_shReasonID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szReason,sizeof(m_szReason));
	iOutLength += encode_length;

	return success;
}
int32_t CCSNotifyCloseChat::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shReasonID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_string(&ptmp,m_szReason,sizeof(m_szReason));
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSNotifyCloseChat::dump()
{

}
CCSNotifyCloseChat& CCSNotifyCloseChat::operator =(const CCSNotifyCloseChat& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	m_shReasonID = original.m_shReasonID;
	STRNCPY(m_szReason, original.m_szReason, sizeof(m_szReason));
	return *this;
}



CCSRequestGetChatGroups::CCSRequestGetChatGroups()
{
	m_iUid = -1;
	m_iChatContentType = 0;
}
CCSRequestGetChatGroups::~CCSRequestGetChatGroups()
{

}
CCSRequestGetChatGroups& CCSRequestGetChatGroups::operator =(const CCSRequestGetChatGroups& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	m_iChatContentType = original.m_iChatContentType;

	return *this;
}

int32_t CCSRequestGetChatGroups::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChatContentType);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestGetChatGroups::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChatContentType);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestGetChatGroups::dump()
{

}

CCSResponseGetChatGroups::CCSResponseGetChatGroups()
{
	m_iGroupCount = 0;
	memset(m_szChatGroup,0,sizeof(m_szChatGroup)/sizeof(stChatGroup));

}
CCSResponseGetChatGroups::~CCSResponseGetChatGroups()
{

}
CCSResponseGetChatGroups& CCSResponseGetChatGroups::operator =(const CCSResponseGetChatGroups& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iGroupCount = original.m_iGroupCount;

	memcpy(m_szChatGroup, original.m_szChatGroup, sizeof(m_szChatGroup));
	return *this;
}

int32_t CCSResponseGetChatGroups::encode(char* pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGroupCount);
	iOutLength += encode_length;

	for(int32_t i=0;i< m_iGroupCount; ++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szChatGroup[i].m_iGroupID);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szChatGroup[i].m_iPlayerIndex1);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szChatGroup[i].m_iPlayerIndex2);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szChatGroup[i].m_iAdminIndex);
		iOutLength += encode_length;

	}

	return success;
}
int32_t CCSResponseGetChatGroups::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupCount);
	remain_length -= decode_length;

	for(int32_t i=0; i< m_iGroupCount;++i)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szChatGroup[i].m_iGroupID);
		remain_length -= decode_length;
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szChatGroup[i].m_iPlayerIndex1);
		remain_length -= decode_length;
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szChatGroup[i].m_iPlayerIndex2);
		remain_length -= decode_length;
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szChatGroup[i].m_iAdminIndex);
		remain_length -= decode_length;
	}
	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSResponseGetChatGroups::dump()
{

}


CCSRequestAddChatGroup::CCSRequestAddChatGroup()
{
	m_iUid = 0;
	m_iGroupID = 0;
}

CCSRequestAddChatGroup::~CCSRequestAddChatGroup()
{

}

CCSRequestAddChatGroup& CCSRequestAddChatGroup::operator =(const CCSRequestAddChatGroup& original)
{
	if (this == &original)
	{
		return *this;
	}
	m_iUid = original.m_iUid;
	m_iGroupID = original.m_iGroupID;

	return *this;
}

int32_t CCSRequestAddChatGroup::encode(char* pszOut, int32_t& iOutLength)
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

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGroupID);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestAddChatGroup::decode(const char *pszIn, const int32_t iInLength)
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

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupID);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestAddChatGroup::dump()
{

}
}}
