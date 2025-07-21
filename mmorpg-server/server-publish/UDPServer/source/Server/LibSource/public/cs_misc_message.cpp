#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/cs_misc_message.h"

namespace KingNet { namespace Server {  

CCSRequestUpdatePlayerCardID::CCSRequestUpdatePlayerCardID()
{
	m_iUin = 0;
	m_szAccount[0]='\0';
	m_szIDCard[0]='\0';
	m_szUserName[0]='\0';
}

CCSRequestUpdatePlayerCardID::~CCSRequestUpdatePlayerCardID()
{


}

CCSRequestUpdatePlayerCardID& CCSRequestUpdatePlayerCardID::operator =(const CCSRequestUpdatePlayerCardID& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_iUin = original.m_iUin;

	STRNCPY(m_szAccount, original.m_szAccount, sizeof(m_szAccount));

	STRNCPY(m_szIDCard, original.m_szIDCard, sizeof(m_szIDCard));

	STRNCPY(m_szUserName, original.m_szUserName, sizeof(m_szUserName));

	return *this;
}

int32_t  CCSRequestUpdatePlayerCardID::encode(char *pszOut, int32_t& iOutLength) 
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
	iOutLength += encode_length;
	return success;
}

int32_t  CCSRequestUpdatePlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szUserName,sizeof(m_szUserName));
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CCSRequestUpdatePlayerCardID::dump()
{


}

CCSResponseUpdatePlayerCardID::CCSResponseUpdatePlayerCardID()
{
	m_iUin = 0;
	memset(m_szAccount,0,sizeof(m_szAccount));
	m_nResultID =success;
}

CCSResponseUpdatePlayerCardID::~CCSResponseUpdatePlayerCardID()
{

}

CCSResponseUpdatePlayerCardID& CCSResponseUpdatePlayerCardID::operator =(const CCSResponseUpdatePlayerCardID& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_iUin = original.m_iUin;

	STRNCPY(m_szAccount, original.m_szAccount, sizeof(m_szAccount));

	m_nResultID = original.m_nResultID;

	return *this;
}

int32_t  CCSResponseUpdatePlayerCardID::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	return success;
}

int32_t  CCSResponseUpdatePlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CCSResponseUpdatePlayerCardID::dump()
{


}
////////////////////

CCSRequestGetProfileFriendList::CCSRequestGetProfileFriendList()
{
	m_iMyUIN = 0;
	m_iGroupMask = 0;
}
int32_t CCSRequestGetProfileFriendList::encode(char* pszOut, int32_t& iOutLength)
{	
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_iGroupMask);
		iOutLength += coded_length;

		return success;
}
int32_t CCSRequestGetProfileFriendList::decode(const char* pszIn, const int32_t iInLength)
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int16_t remained_length  = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iGroupMask);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CCSRequestGetProfileFriendList::dump()
{

}
CCSRequestGetProfileFriendList& CCSRequestGetProfileFriendList::operator =(const CCSRequestGetProfileFriendList& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_iMyUIN = original.m_iMyUIN;

	m_iGroupMask = original.m_iGroupMask;

	return *this;
}
CCSRequestGetProfileFriendList::~CCSRequestGetProfileFriendList()
{
	//do something
}
/////////////////////////
CCSResponseGetProfileFriendList::CCSResponseGetProfileFriendList()
{
	m_nResultID		= -1;
	m_iMyUIN		= 0;
	m_nFriendCount	= 0;

}

CCSResponseGetProfileFriendList::~CCSResponseGetProfileFriendList()
{
	//do nothing
}

int32_t CCSResponseGetProfileFriendList::encode(char* pszOut, int32_t& iOutLength)
{
	if (NULL == pszOut)
	{
		return fail;
	}

	if (m_nFriendCount > max_friend_list_count)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nFriendCount);
	iOutLength += coded_length;

	for (int16_t i=0; i<m_nFriendCount; ++i)
	{
		coded_length = CCodeEngine::encode_int32(&ptmp, m_astFriendInfo[i].m_iUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_astFriendInfo[i].m_szAccount, sizeof(m_astFriendInfo[i].m_szAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_astFriendInfo[i].m_iGroupFlag);
		iOutLength += coded_length;
	}

	return success;
}

int32_t CCSResponseGetProfileFriendList::decode(const char* pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFriendCount);
	remained_length -= decoded_length;

	if (m_nFriendCount > max_friend_list_count)
	{
		return fail;
	}

	for (int16_t i=0; i<m_nFriendCount; ++i)
	{
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astFriendInfo[i].m_iUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_astFriendInfo[i].m_szAccount, sizeof(m_astFriendInfo[i].m_szAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_astFriendInfo[i].m_iGroupFlag);
		remained_length -= decoded_length;
	}

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CCSResponseGetProfileFriendList::dump()
{
	//do nothing
}

CCSRequestGetPlayerCardID::CCSRequestGetPlayerCardID()
{
	m_iUin = 0;

}

CCSRequestGetPlayerCardID::~CCSRequestGetPlayerCardID()
{


}

int32_t  CCSRequestGetPlayerCardID::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;


	return success;

}

int32_t  CCSRequestGetPlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;
	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CCSRequestGetPlayerCardID::dump()
{



}

CCSResponseGetPlayerCardID::CCSResponseGetPlayerCardID()
{
	m_iUin = 0;
	m_nResultID = 0;  
	memset(m_szIDCard,0,sizeof(m_szIDCard));				//身份证号

}

CCSResponseGetPlayerCardID::~CCSResponseGetPlayerCardID()
{
	//do something
}

int32_t  CCSResponseGetPlayerCardID::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	if(m_nResultID == success)
	{
		encode_length = CCodeEngine::encode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
		iOutLength += encode_length;
	}

	return success;
}

int32_t  CCSResponseGetPlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;


	if(m_nResultID == success)
	{
		decode_length = CCodeEngine::decode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
		remain_length -= decode_length;
	}

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CCSResponseGetPlayerCardID::dump()
{
	//do something
}
//请求拉取玩伴分组
CCSRequestGetFriendGroup::CCSRequestGetFriendGroup()
{
	m_iMyUIN = 0;
}
CCSRequestGetFriendGroup::~CCSRequestGetFriendGroup()
{

}
int32_t CCSRequestGetFriendGroup::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMyUIN);
	iOutLength += encode_length;

	return success;
}
int32_t CCSRequestGetFriendGroup::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}
void CCSRequestGetFriendGroup::dump()
{

}


//响应拉取玩伴分组
CCSResponseGetFriendGroup::CCSResponseGetFriendGroup()
{
	m_nResultID = 0;
	m_iMyUIN = 0;
	m_cGroupCount = 0;
	memset(m_astGroup,0,sizeof(m_astGroup));
	
}
CCSResponseGetFriendGroup::~CCSResponseGetFriendGroup()
{
	
}
int32_t CCSResponseGetFriendGroup::decode(const char* pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
	remained_length -= decoded_length;

	if (result_id_success == m_nResultID)
	{
		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cGroupCount);
		remained_length -= decoded_length;

		if (m_cGroupCount > max_friend_group_list_count)
		{
			m_cGroupCount = max_friend_group_list_count;
		}

		for (int8_t i=0; i<m_cGroupCount; ++i)
		{
			decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_astGroup[i].m_iGroupMask);
			remained_length -= decoded_length;
			decoded_length = CCodeEngine::decode_string(&ptmp,m_astGroup[i].m_szGroupName,sizeof(m_astGroup[i].m_szGroupName));
			remained_length -= decoded_length;

		}
	}

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}
int32_t CCSResponseGetFriendGroup::encode(char* pszOut, int32_t& iOutLength)
{
	if (NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
	iOutLength += coded_length;

	if (result_id_success == m_nResultID)
	{	
		if (m_cGroupCount > max_friend_group_list_count)
		{
			m_cGroupCount = max_friend_group_list_count;
		}

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cGroupCount);
		iOutLength += coded_length;

		for (int8_t i=0; i<m_cGroupCount; ++i)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_astGroup[i].m_iGroupMask);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_string(&ptmp,m_astGroup[i].m_szGroupName,sizeof(m_astGroup[i].m_szGroupName));
			iOutLength += coded_length;
		}
	}

	return success;
}
void CCSResponseGetFriendGroup::dump()
{
	//do something
}


CCSResponseGetPlayerGroupData::CCSResponseGetPlayerGroupData()
{
	m_nResultID = 0;
	m_iMyUIN = 0;
	m_cGroupCount = 0;
	memset(m_stGroupInfo,0,sizeof(m_stGroupInfo));
}

CCSResponseGetPlayerGroupData::~CCSResponseGetPlayerGroupData()
{

}

int32_t CCSResponseGetPlayerGroupData::decode(const char* pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
	remained_length -= decoded_length;

	if (result_id_success == m_nResultID)
	{
		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cGroupCount);
		remained_length -= decoded_length;

		if (m_cGroupCount > max_web_qun_count)
		{
			m_cGroupCount = max_web_qun_count;
		}

		for (int8_t i=0; i<m_cGroupCount; ++i)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp,m_stGroupInfo[i].m_szQunUID,sizeof(m_stGroupInfo[i].m_szQunUID));
			remained_length -= decoded_length;
			
			decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stGroupInfo[i].m_nQunRole);
			remained_length -= decoded_length;

		}
	}

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

int32_t CCSResponseGetPlayerGroupData::encode(char* pszOut, int32_t& iOutLength)
{
	if (NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
	iOutLength += coded_length;

	if (result_id_success == m_nResultID)
	{	
		if (m_cGroupCount > max_web_qun_count)
		{
			m_cGroupCount = max_web_qun_count;
		}

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cGroupCount);
		iOutLength += coded_length;

		for (int8_t i=0; i<m_cGroupCount; ++i)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_stGroupInfo[i].m_szQunUID,sizeof(m_stGroupInfo[i].m_szQunUID));
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int16(&ptmp,m_stGroupInfo[i].m_nQunRole);
			iOutLength += coded_length;
		}
	}
	return success;
}

void CCSResponseGetPlayerGroupData::dump()
{

}

}}
