#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/server_frienddb_message_body.h"

namespace KingNet { namespace Server {

	CTagInfo::CTagInfo()
	{
		//do nothing
	}

	CTagInfo::~CTagInfo()
	{
		//do nothing
	}

	void CTagInfo::clear()
	{
		m_iTagID = INVALID_TAG_ID;
		m_iTimestamp = 0;
		m_szTagName[0] = '\0';
	}

	bool CTagInfo::valid_tag_id()
	{
		return true;
	}

	CTagInfo & CTagInfo::operator=(const CTagInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iTagID = src.m_iTagID;
		m_iTimestamp = src.m_iTimestamp;
		STRNCPY(m_szTagName, src.m_szTagName, sizeof(m_szTagName));
		return *this;
	}


	CFriendInfo::CFriendInfo()
	{
		//do nothing
	}

	CFriendInfo::~CFriendInfo()
	{
		//do nothing
	}

	CFriendInfo& CFriendInfo::operator= (const CFriendInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iFriendUIN = src.m_iFriendUIN;
		STRNCPY(m_szFriendAccount, src.m_szFriendAccount, sizeof(m_szFriendAccount));
		m_iTimestamp = m_iTimestamp;
		m_nTagCount  = src.m_nTagCount;
		if (m_nTagCount > MAX_TAG_COUNT_PER_FRIEND)
		{
			m_nTagCount = MAX_TAG_COUNT_PER_FRIEND;
		}

		for (int16_t i=0; i<m_nTagCount; ++i)
		{
			m_astTag[i] = src.m_astTag[i];
		}

		STRNCPY(m_szRemarks, src.m_szRemarks, sizeof(m_szRemarks));
		m_iGroupFlag = src.m_iGroupFlag;

		return *this;
	}

	void CFriendInfo::clear()
	{
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_iTimestamp = 0;
		m_nTagCount  = 0;
		m_szRemarks[0] = '\0';
		m_iGroupFlag = 1;
	}

	CStatTagInfo::CStatTagInfo()
	{
		//do nothing
	}

	CStatTagInfo::~CStatTagInfo()
	{
		//do nothing
	}

	CStatTagInfo& CStatTagInfo::operator = (const CStatTagInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iTagID = src.m_iTagID;
		m_iTimestamp = src.m_iTimestamp;
		STRNCPY(m_szTagName, src.m_szTagName, sizeof(m_szTagName));
		m_iCount = src.m_iCount;

		return *this;
	}

	bool CStatTagInfo::operator < (const CStatTagInfo& tag2)
	{
		if (m_iCount < tag2.m_iCount)
		{
			return true;
		}
		else if(m_iCount == tag2.m_iCount)
		{
			if (m_iTimestamp < tag2.m_iTimestamp)
			{
				return true;
			}
		}

		return false;
	}

	void CStatTagInfo::clear()
	{
		m_iTagID = INVALID_TAG_ID;
		m_szTagName[0] = '\0';
		m_iCount = 0;
		m_iTimestamp = 0;
	}

	CPlayerBaseInfo::CPlayerBaseInfo()
	{
		//do nothing
	}

	CPlayerBaseInfo::~CPlayerBaseInfo()
	{
		//do nothing
	}

	CPlayerBaseInfo& CPlayerBaseInfo::operator= (const CPlayerBaseInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iUIN = src.m_iUIN;
		STRNCPY(m_szAccount, src.m_szAccount, sizeof(m_szAccount));
		m_iGroupFlag = src.m_iGroupFlag;

		return *this;
	}

	void CPlayerBaseInfo::clear()
	{
		m_iUIN = 0;
		m_szAccount[0] = '\0';
		m_iGroupFlag = 0;
	}

	//请求正向玩伴列表
	CRequestPositiveFriendList::CRequestPositiveFriendList()
	{
		m_iUIN = 0;
		m_iDestUIN = 0;
	}

	CRequestPositiveFriendList::~CRequestPositiveFriendList()
	{
		//do nothing
	}

	int32_t CRequestPositiveFriendList::encode(char* pszOut, int32_t& iOutLength)
	{	
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDestUIN);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestPositiveFriendList::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUIN);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestPositiveFriendList::dump()
	{
		//do nothing
	}


	//正向玩伴列表应答
	CResponsePositiveFriendList::CResponsePositiveFriendList()
	{
		m_nFriendCount = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponsePositiveFriendList::~CResponsePositiveFriendList()
	{
		//do nothing
	}

	int32_t CResponsePositiveFriendList::encode(char* pszOut, int32_t& iOutLength)
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

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
			return success;
		}
		
		if (MAX_RESPONSE_FRIEND_COUNT < m_nFriendCount)
		{
			m_nFriendCount = MAX_RESPONSE_FRIEND_COUNT;
		}
		coded_length = CCodeEngine::encode_int16(&ptmp, m_nFriendCount);
		iOutLength += coded_length;

		for (int16_t i = 0; i<m_nFriendCount; ++i)
		{
			coded_length = encode_friendinfo(&ptmp, m_astFriendInfo[i]);
			iOutLength += coded_length;
		}
		
		return success;
	}

	int32_t CResponsePositiveFriendList::decode(const char* pszIn, const int32_t iInLength)
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
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFriendCount);
			remained_length -= decoded_length;

			if (MAX_RESPONSE_FRIEND_COUNT < m_nFriendCount)
			{
				m_nFriendCount = MAX_RESPONSE_FRIEND_COUNT;
			}
			for (int16_t i  = 0; i<m_nFriendCount; ++i)
			{
				decoded_length = decode_friendinfo(&ptmp, m_astFriendInfo[i]);
				remained_length -= decoded_length;
			}
			
		}
		else
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}
		
		if (remained_length < 0)
		{
			return fail;
		}

		
		return success;
	}

	void CResponsePositiveFriendList::dump()
	{
		//do nothing
	}

	int32_t encode_taginfo(char** pszOut, CTagInfo& taginfo)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t tag_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, taginfo.m_iTagID);
		tag_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, taginfo.m_szTagName, (const int16_t) sizeof(taginfo.m_szTagName));
		tag_size += coded_length;

		*pszOut -= (tag_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, tag_size);

		iOutLength = (tag_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_taginfo(char** pszIn, CTagInfo& taginfo)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t tag_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&tag_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&taginfo.m_iTagID);
		iOutLength += decode_length;
		tag_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, taginfo.m_szTagName, (const int16_t)sizeof(taginfo.m_szTagName));
		iOutLength += decode_length;
		tag_size -= decode_length;

		if (tag_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += tag_size;
			iOutLength += tag_size;
		}

		return iOutLength;
	}


	int32_t encode_friendinfo(char** pszOut, CFriendInfo& friendinfo)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}
		
		int16_t friend_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, friendinfo.m_iFriendUIN);
		friend_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, friendinfo.m_szFriendAccount, (const int16_t)sizeof(friendinfo.m_szFriendAccount));
		friend_size += coded_length;

		if (MAX_TAG_COUNT_PER_FRIEND < friendinfo.m_nTagCount)
		{
			friendinfo.m_nTagCount = MAX_TAG_COUNT_PER_FRIEND; 
		}
		coded_length = CCodeEngine::encode_int16(pszOut, friendinfo.m_nTagCount);
		friend_size += coded_length;

		for (int16_t i = 0; i<friendinfo.m_nTagCount; ++i)
		{
			coded_length = encode_taginfo(pszOut, friendinfo.m_astTag[i]);
			friend_size += coded_length;
		}

		coded_length = CCodeEngine::encode_string(pszOut, friendinfo.m_szRemarks, (const int16_t)sizeof(friendinfo.m_szRemarks));
		friend_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, friendinfo.m_iGroupFlag);
		friend_size += coded_length;
		
		*pszOut -= (friend_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, friend_size);

		iOutLength = (friend_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;

	}

	int32_t decode_friendinfo(char** pszIn, CFriendInfo& friendinfo)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t friend_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&friend_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&friendinfo.m_iFriendUIN);
		iOutLength += decode_length;
		friend_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, friendinfo.m_szFriendAccount, (const int16_t)sizeof(friendinfo.m_szFriendAccount));
		iOutLength += decode_length;
		friend_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&friendinfo.m_nTagCount);
		iOutLength += decode_length;
		friend_size -= decode_length;

		if (MAX_TAG_COUNT_PER_FRIEND < friendinfo.m_nTagCount)
		{
			friendinfo.m_nTagCount = MAX_TAG_COUNT_PER_FRIEND; 
		}

		for (int16_t i = 0; i<friendinfo.m_nTagCount; ++i)
		{
			decode_length = decode_taginfo(pszIn, friendinfo.m_astTag[i]);
			iOutLength += decode_length;
			friend_size -= decode_length;
		}

		decode_length = CCodeEngine::decode_string(pszIn, friendinfo.m_szRemarks, (const int16_t)sizeof(friendinfo.m_szRemarks));
		iOutLength += decode_length;
		friend_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&friendinfo.m_iGroupFlag);
		iOutLength += decode_length;
		friend_size -= decode_length;

		if (friend_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += friend_size;
			iOutLength += friend_size;
		}

		return iOutLength;
	}

	//////////////////////////////////////////////////////////////////////////

	CResponseReverseList::CResponseReverseList()
	{
		m_nResultID		= -1;
		m_iMyUIN		= 0;
		m_nFriendCount	= 0;
	}

	CResponseReverseList::~CResponseReverseList()
	{
		//do nothing
	}

	int32_t CResponseReverseList::encode(char* pszOut, int32_t& iOutLength)
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

		if (result_id_success != m_nResultID)
		{
			return success;
		}

		if (MAX_REVERSE_FRIEND_COUNT < m_nFriendCount)
		{
			m_nFriendCount = MAX_REVERSE_FRIEND_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nFriendCount);
		iOutLength += coded_length;

		for (int16_t i=0; i<m_nFriendCount; ++i)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_aiFriendUIN[i]);
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseReverseList::decode(const char* pszIn, const int32_t iInLength)
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
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFriendCount);
			remained_length -= decoded_length;

			if (MAX_REVERSE_FRIEND_COUNT < m_nFriendCount)
			{
				m_nFriendCount = MAX_REVERSE_FRIEND_COUNT;
			}

			for (int16_t i=0; i<m_nFriendCount; ++i)
			{
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aiFriendUIN[i]);
				remained_length -= decoded_length;
			}

		}

		if (remained_length < 0)
		{
			return fail;
		}


		return success;
	}

	void CResponseReverseList::dump()
	{
		//do nothing
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int32_t encode_tag_statistics(char** pszOut, CStatTagInfo& statistic)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t statistic_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, statistic.m_iTagID);
		statistic_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, statistic.m_szTagName, (const int16_t) sizeof(statistic.m_szTagName));
		statistic_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, statistic.m_iCount);
		statistic_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, statistic.m_iTimestamp);
		statistic_size += coded_length;

		*pszOut -= (statistic_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, statistic_size);

		iOutLength = (statistic_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_tag_statistics(char** pszIn, CStatTagInfo& statistic)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t statistic_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&statistic_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&statistic.m_iTagID);
		iOutLength += decode_length;
		statistic_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, statistic.m_szTagName, (const int16_t)sizeof(statistic.m_szTagName));
		iOutLength += decode_length;
		statistic_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&statistic.m_iCount);
		iOutLength += decode_length;
		statistic_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&statistic.m_iTimestamp);
		iOutLength += decode_length;
		statistic_size -= decode_length;

		if (statistic_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += statistic_size;
			iOutLength += statistic_size;
		}

		return iOutLength;
	}

	//请求被贴标签统计信息的应答
	CResponseTagStatisticalInfo::CResponseTagStatisticalInfo()
	{
		m_nResultID = -1;
		m_iMyUIN = 0;
		m_iDestUIN = 0;
		m_nTagCount = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseTagStatisticalInfo::~CResponseTagStatisticalInfo()
	{
		//do nothing
	}

	int32_t CResponseTagStatisticalInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDestUIN);
		iOutLength += coded_length;

		if (result_id_success == m_nResultID)
		{
			if (MAX_STAT_TAG_COUNT < m_nTagCount)
			{
				m_nTagCount = MAX_STAT_TAG_COUNT; 
			}
			coded_length = CCodeEngine::encode_int16(&ptmp, m_nTagCount);
			iOutLength += coded_length;

			for (int16_t i=0; i<m_nTagCount; ++i)
			{
				coded_length = encode_tag_statistics(&ptmp, m_astTag[i]);
				iOutLength += coded_length;
			}
			
		}
		else
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;

	}

	int32_t CResponseTagStatisticalInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUIN);
		remained_length -= decoded_length;

		if (result_id_success == m_nResultID)
		{
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTagCount);
			remained_length -= decoded_length;
			
			if (MAX_STAT_TAG_COUNT < m_nTagCount)
			{
				m_nTagCount = MAX_STAT_TAG_COUNT; 
			}
			for (int16_t i=0; i<m_nTagCount; ++i)
			{
				decoded_length = decode_tag_statistics(&ptmp, m_astTag[i]);
				remained_length -= decoded_length;
			}

		}
		else
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}


		return success;
	}

	void CResponseTagStatisticalInfo::dump()
	{
		//do nothing
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求给玩伴贴标签
	CRequestAddTagToFriend::CRequestAddTagToFriend()
	{
		m_iMyUIN = 0;
		m_iFriendUIN = 0;
		m_iTagID = INVALID_TAG_ID;
		m_szTagName[0] = '\0';
		m_szMyAccount[0] = '\0';
		m_szFriendAccount[0] = '\0';
	}

	CRequestAddTagToFriend::~CRequestAddTagToFriend()
	{
		//do nothing
	}

	int32_t CRequestAddTagToFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, (const int16_t)sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, (const int16_t)sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iTagID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szTagName, (const int16_t)sizeof(m_szTagName));
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddTagToFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, (const int16_t)sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, (const int16_t)sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTagID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szTagName, (const int16_t)sizeof(m_szTagName));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestAddTagToFriend::dump()
	{
		//do nothing
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求删除玩伴
	CRequestDeleteFriend::CRequestDeleteFriend()
	{
		m_iMyUIN = 0;
		m_iFriendUIN = 0;
	}

	CRequestDeleteFriend::~CRequestDeleteFriend()
	{
		//do nothing
	}

	int32_t CRequestDeleteFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestDeleteFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestDeleteFriend::dump()
	{
		//do nothing
	}


	//删除玩伴的应答
	CResponseDeleteFriend::CResponseDeleteFriend()
	{
		m_nResultID = -1;
		m_iMyUIN = 0;
		m_iFriendUIN = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseDeleteFriend::~CResponseDeleteFriend()
	{
		//do nothing
	}

	int32_t CResponseDeleteFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;

	}

	int32_t CResponseDeleteFriend::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseDeleteFriend::dump()
	{
		//do nothing
	}

	//给玩伴贴标签的应答
	CResponseAddTagToFriend::CResponseAddTagToFriend()
	{
		m_nResultID = -1;
		m_iMyUIN = 0;
		m_iDestUIN = 0;
		m_iTagID = INVALID_TAG_ID;
		m_szTagName[0] = '\0';
		m_szReasonMessage[0] = '\0';
	}

	CResponseAddTagToFriend::~CResponseAddTagToFriend()
	{
		//do nothing
	}

	int32_t CResponseAddTagToFriend::encode( char *pszOut, int32_t& iOutLength )
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDestUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iTagID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szTagName, (const int16_t)sizeof(m_szTagName));
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseAddTagToFriend::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDestUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTagID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szTagName, (const int16_t)sizeof(m_szTagName));
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseAddTagToFriend::dump()
	{
		//do nothing
	}


	//SS_MSG_NOTIFY_FRIEND_INFO
	CNotifyFriendInfo::CNotifyFriendInfo()
	{
		m_iMyUIN = 0;
		m_nControlCMD = 0;
	}

	CNotifyFriendInfo::~CNotifyFriendInfo()
	{
		//do nothing	
	}

	int32_t CNotifyFriendInfo::encode( char *pszOut, int32_t& iOutLength )
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

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nControlCMD);
		iOutLength += coded_length;

		coded_length = encode_friendinfo(&ptmp, m_stFriend);
		iOutLength += coded_length;

		return success;

	}

	int32_t CNotifyFriendInfo::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nControlCMD);
		remained_length -= decoded_length;

		decoded_length = decode_friendinfo(&ptmp, m_stFriend);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyFriendInfo::dump()
	{
		//do nothing
	}


	//////////////////////////////////////////////////////////////////////////

	CResponseSimpleFriendList::CResponseSimpleFriendList()
	{
		m_nResultID		= -1;
		m_iMyUIN		= 0;
		m_nFriendCount	= 0;
	}

	CResponseSimpleFriendList::~CResponseSimpleFriendList()
	{
		//do nothing
	}

	int32_t CResponseSimpleFriendList::encode(char* pszOut, int32_t& iOutLength)
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

		if (result_id_success != m_nResultID)
		{
			return success;
		}

		if (MAX_POSITIVE_FRIEND_COUNT < m_nFriendCount)
		{
			m_nFriendCount = MAX_POSITIVE_FRIEND_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nFriendCount);
		iOutLength += coded_length;

		for (int16_t i=0; i<m_nFriendCount; ++i)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_aiFriendUIN[i]);
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseSimpleFriendList::decode(const char* pszIn, const int32_t iInLength)
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
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFriendCount);
			remained_length -= decoded_length;

			if (MAX_POSITIVE_FRIEND_COUNT < m_nFriendCount)
			{
				m_nFriendCount = MAX_POSITIVE_FRIEND_COUNT;
			}

			for (int16_t i=0; i<m_nFriendCount; ++i)
			{
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aiFriendUIN[i]);
				remained_length -= decoded_length;
			}

		}

		if (remained_length < 0)
		{
			return fail;
		}


		return success;
	}

	void CResponseSimpleFriendList::dump()
	{
		//do nothing
	}


	//////////////////////////////////////////////////////////////////////////


	CRequestAddGroup::CRequestAddGroup()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_szGroupName[0] = '\0';
	}

	CRequestAddGroup::~CRequestAddGroup()
	{
		//do nothing
	}

	int32_t CRequestAddGroup::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddGroup::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestAddGroup::dump()
	{
		//do nothing
	}


	CResponseAddGroup::CResponseAddGroup()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_iGroupMask = 0;
		m_szGroupName[0] = '\0';
		m_szReasonMessage[0] = '\0';
	}

	CResponseAddGroup::~CResponseAddGroup()
	{
		//do nothing
	}

	int32_t CResponseAddGroup::encode(char* pszOut, int32_t& iOutLength)
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
			coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupMask);
			iOutLength += coded_length;
		}

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseAddGroup::decode(const char* pszIn, const int32_t iInLength)
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
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupMask);
			remained_length -= decoded_length;
		}

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseAddGroup::dump()
	{
		//do nothing
	}


	//////////////////////////////////////////////////////////////////////////

	CRequestDeleteGroup::CRequestDeleteGroup()
	{
		m_iMyUIN = 0;
		m_iGroupMask = 0;
	}

	CRequestDeleteGroup::~CRequestDeleteGroup()
	{
		//do nothing
	}

	int32_t CRequestDeleteGroup::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupMask);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestDeleteGroup::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupMask);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestDeleteGroup::dump()
	{
		//do nothing
	}

	CResponseDeleteGroup::CResponseDeleteGroup()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_iGroupMask = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseDeleteGroup::~CResponseDeleteGroup()
	{
		//do nothing
	}

	int32_t CResponseDeleteGroup::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupMask);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseDeleteGroup::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupMask);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseDeleteGroup::dump()
	{
		//do nothing
	}


	//////////////////////////////////////////////////////////////////////////


	CRequestUpdateFriendGroupInfo::CRequestUpdateFriendGroupInfo()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_iGroupFlag = 0;
	}

	CRequestUpdateFriendGroupInfo::~CRequestUpdateFriendGroupInfo()
	{
		//do nothing
	}

	int32_t CRequestUpdateFriendGroupInfo::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupFlag);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestUpdateFriendGroupInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupFlag);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestUpdateFriendGroupInfo::dump()
	{
		//do nothing
	}


	CResponseUpdateFriendGroupInfo::CResponseUpdateFriendGroupInfo()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_iFriendUIN = 0;
		m_iGroupFlag = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseUpdateFriendGroupInfo::~CResponseUpdateFriendGroupInfo()
	{
		//do nothing
	}

	int32_t CResponseUpdateFriendGroupInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupFlag);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseUpdateFriendGroupInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupFlag);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseUpdateFriendGroupInfo::dump()
	{
		//do nothing
	}


	//////////////////////////////////////////////////////////////////////////

	CRequestGroupRename::CRequestGroupRename()
	{
		m_iMyUIN = 0;
		m_iGroupMask = 0;
		m_szGroupName[0] = '\0';
	}

	CRequestGroupRename::~CRequestGroupRename()
	{
		//do nothing
	}

	int32_t CRequestGroupRename::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupMask);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestGroupRename::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupMask);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestGroupRename::dump()
	{
		//do nothing
	}


	CResponseGroupRename::CResponseGroupRename()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_iGroupMask = 0;
		m_szGroupName[0] = '\0';
		m_szReasonMessage[0] = '\0';
	}

	CResponseGroupRename::~CResponseGroupRename()
	{
		//do nothing
	}

	int32_t CResponseGroupRename::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupMask);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseGroupRename::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupMask);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupName, sizeof(m_szGroupName));
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseGroupRename::dump()
	{
		//do nothing
	}

	//////////////////////////////////////////////////////////////////////////

	CGroupInfo::CGroupInfo()
	{
		//do nothing
	}

	CGroupInfo::~CGroupInfo()
	{
		//do nothing
	}

	CGroupInfo& CGroupInfo::operator = (const CGroupInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iGroupMask = src.m_iGroupMask;
		STRNCPY(m_szGroupName, src.m_szGroupName, sizeof(m_szGroupName));
		m_bUsed = src.m_bUsed;
		return *this;
	}

	void CGroupInfo::clear()
	{
		m_iGroupMask = 0;
		m_szGroupName[0] = '\0';
		m_bUsed = false;
	}

	int32_t encode_group_info(char** pszOut, CGroupInfo& group_info)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t encode_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, group_info.m_iGroupMask);
		encode_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, group_info.m_szGroupName, sizeof(group_info.m_szGroupName));
		encode_size += coded_length;

		*pszOut -= (encode_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, encode_size);

		iOutLength = (encode_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_group_info(char** pszIn, CGroupInfo& group_info)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t decode_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&decode_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&group_info.m_iGroupMask);
		iOutLength += decode_length;
		decode_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, group_info.m_szGroupName, sizeof(group_info.m_szGroupName));
		iOutLength += decode_length;
		decode_size -= decode_length;

		if (decode_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn)   += decode_size;
			iOutLength += decode_size;
		}

		return iOutLength;
	}

	CResponseGetGroupInfo::CResponseGetGroupInfo()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_cGroupCount = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseGetGroupInfo::~CResponseGetGroupInfo()
	{
		//do nothing
	}

	void CResponseGetGroupInfo::dump()
	{
		//do nothing
	}

	int32_t CResponseGetGroupInfo::encode(char* pszOut, int32_t& iOutLength)
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
			if (m_cGroupCount > MAX_FRIEND_GROUP_COUNT)
			{
				m_cGroupCount = MAX_FRIEND_GROUP_COUNT;
			}

			coded_length = CCodeEngine::encode_int8(&ptmp, m_cGroupCount);
			iOutLength += coded_length;

			for (int8_t i=0; i<m_cGroupCount; ++i)
			{
				coded_length = encode_group_info(&ptmp, m_astGroup[i]);
				iOutLength += coded_length;
			}
		}
		else
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseGetGroupInfo::decode(const char* pszIn, const int32_t iInLength)
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

			if (m_cGroupCount > MAX_FRIEND_GROUP_COUNT)
			{
				m_cGroupCount = MAX_FRIEND_GROUP_COUNT;
			}

			for (int8_t i=0; i<m_cGroupCount; ++i)
			{
				decoded_length = decode_group_info(&ptmp, m_astGroup[i]);
				remained_length -= decoded_length;
			}
		}
		else	
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestUpdateFriendRemarks::CRequestUpdateFriendRemarks()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_szRemarks[0] = '\0';
	}

	CRequestUpdateFriendRemarks::~CRequestUpdateFriendRemarks()
	{
		//do nothing
	}

	void CRequestUpdateFriendRemarks::dump()
	{
		//do nothing
	}

	int32_t CRequestUpdateFriendRemarks::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestUpdateFriendRemarks::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseUpdateFriendRemarks::CResponseUpdateFriendRemarks()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_iFriendUIN = 0;
		m_szRemarks[0] = '\0';
		m_szReasonMessage[0] = '\0';
	}

	CResponseUpdateFriendRemarks::~CResponseUpdateFriendRemarks()
	{
		//do nothing
	}

	void CResponseUpdateFriendRemarks::dump()
	{
		//do nothing
	}

	int32_t CResponseUpdateFriendRemarks::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseUpdateFriendRemarks::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CPlayWithInfo::CPlayWithInfo()
	{
		//do nothing
	}

	CPlayWithInfo::~CPlayWithInfo()
	{
		//do nothing
	}

	CPlayWithInfo& CPlayWithInfo::operator = (const CPlayWithInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iFriendUIN = src.m_iFriendUIN;
		STRNCPY(m_szFriendAccount, src.m_szFriendAccount, sizeof(m_szFriendAccount));
		m_nGameID    = src.m_nGameID;
		m_iTimestamp = src.m_iTimestamp;
		return *this;
	}

	void CPlayWithInfo::clear()
	{
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_nGameID    = -1;
		m_iTimestamp = 0;
	}

	int32_t encode_play_with_info(char** pszOut, CPlayWithInfo& info)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t encode_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, info.m_iFriendUIN);
		encode_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, info.m_szFriendAccount, sizeof(info.m_szFriendAccount));
		encode_size += coded_length;

		coded_length = CCodeEngine::encode_int16(pszOut, info.m_nGameID);
		encode_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pszOut, info.m_iTimestamp);
		encode_size += coded_length;

		*pszOut -= (encode_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, encode_size);

		iOutLength = (encode_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_play_with_info(char** pszIn, CPlayWithInfo& info)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t decode_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&decode_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iFriendUIN);
		iOutLength += decode_length;
		decode_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, info.m_szFriendAccount, sizeof(info.m_szFriendAccount));
		iOutLength += decode_length;
		decode_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&info.m_nGameID);
		iOutLength += decode_length;
		decode_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iTimestamp);
		iOutLength += decode_length;
		decode_size -= decode_length;

		if (decode_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn)   += decode_size;
			iOutLength += decode_size;
		}

		return iOutLength;
	}

	CResponseGetPlayWithInfo::CResponseGetPlayWithInfo()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_cCount	= 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseGetPlayWithInfo::~CResponseGetPlayWithInfo()
	{
		//do nothing
	}

	void CResponseGetPlayWithInfo::dump()
	{
		//do nothing
	}

	int32_t CResponseGetPlayWithInfo::encode(char* pszOut, int32_t& iOutLength)
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

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}
		else
		{
			if (m_cCount > MAX_PLAY_WITH_COUNT)
			{
				m_cCount = MAX_PLAY_WITH_COUNT;
			}

			coded_length = CCodeEngine::encode_int8(&ptmp, m_cCount);
			iOutLength += coded_length;

			for (int8_t i=0; i<m_cCount; ++i)
			{
				coded_length = encode_play_with_info(&ptmp, m_astInfo[i]);
				iOutLength += coded_length;
			}
		}

		return success;
	}

	int32_t CResponseGetPlayWithInfo::decode(const char* pszIn, const int32_t iInLength)
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

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}
		else
		{
			decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cCount);
			remained_length -= decoded_length;

			if (m_cCount > MAX_PLAY_WITH_COUNT)
			{
				m_cCount = MAX_PLAY_WITH_COUNT;
			}

			for (int8_t i=0; i<m_cCount; ++i)
			{
				decoded_length = decode_play_with_info(&ptmp, m_astInfo[i]);
				remained_length -= decoded_length;
			}
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestAddPlayWithInfo::CRequestAddPlayWithInfo()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_nGameID = -1;
	}

	CRequestAddPlayWithInfo::~CRequestAddPlayWithInfo()
	{
		//do nothing
	}

	void CRequestAddPlayWithInfo::dump()
	{
		//do nothing
	}

	int32_t CRequestAddPlayWithInfo::encode(char *pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nGameID);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddPlayWithInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseAddPlayWithInfo::CResponseAddPlayWithInfo()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseAddPlayWithInfo::~CResponseAddPlayWithInfo()
	{
		//do nothing
	}

	void CResponseAddPlayWithInfo::dump()
	{
		//do nothing
	}

	int32_t CResponseAddPlayWithInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = encode_play_with_info(&ptmp, m_stInfo);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseAddPlayWithInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = decode_play_with_info(&ptmp, m_stInfo);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////
	
	CNotifyDelPlayWithInfo::CNotifyDelPlayWithInfo()
	{
		m_iMyUIN = 0;
	}

	CNotifyDelPlayWithInfo::~CNotifyDelPlayWithInfo()
	{
		//do nothing
	}

	void CNotifyDelPlayWithInfo::dump()
	{
		//do nothing
	}

	int32_t CNotifyDelPlayWithInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = encode_play_with_info(&ptmp, m_stInfo);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyDelPlayWithInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = decode_play_with_info(&ptmp, m_stInfo);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestAddFriend::CRequestAddFriend()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
		m_iTagID = 0;
		m_szTagName[0] = '\0';
		m_szRemarks[0] = '\0';
		m_iGroupFlag = 0;
		m_iGameID = -1;
	}

	CRequestAddFriend::~CRequestAddFriend()
	{
		//do nothing
	}

	void CRequestAddFriend::dump()
	{
		//do nothing
	}

	int32_t CRequestAddFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iTagID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szTagName, sizeof(m_szTagName));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupFlag);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGameID);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTagID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szTagName, sizeof(m_szTagName));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupFlag);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameID);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseAddFriend::CResponseAddFriend()
	{
		m_nResultID    = -1;
		m_iMyUIN	   = 0;
		m_iFriendUIN   = 0;
		m_iTagID       = -1;
		m_szTagName[0] = '\0';
		m_szRemarks[0] = '\0';
		m_iGroupFlag   = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseAddFriend::~CResponseAddFriend()
	{
		//do nothing
	}

	void CResponseAddFriend::dump()
	{
		//do nothing
	}

	int32_t CResponseAddFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iTagID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szTagName, sizeof(m_szTagName));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGroupFlag);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseAddFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTagID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szTagName, sizeof(m_szTagName));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szRemarks, sizeof(m_szRemarks));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGroupFlag);
		remained_length -= decoded_length;

		if (result_id_success != m_nResultID)
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CResponseFriendBaseInfo::CResponseFriendBaseInfo()
	{
		m_nResultID    = -1;
		m_iMyUIN	   = 0;
		m_nFriendCount = 0;
	}

	CResponseFriendBaseInfo::~CResponseFriendBaseInfo()
	{
		//do nothing
	}

	void CResponseFriendBaseInfo::dump()
	{
		//do nothing
	}

	int32_t CResponseFriendBaseInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		if (m_nFriendCount > MAX_POSITIVE_FRIEND_COUNT)
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

			coded_length = CCodeEngine::encode_int32(&ptmp, m_astFriendInfo[i].m_iGroupFlag);
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseFriendBaseInfo::decode(const char* pszIn, const int32_t iInLength)
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

		if (m_nFriendCount > MAX_POSITIVE_FRIEND_COUNT)
		{
			return fail;
		}
		
		for (int16_t i=0; i<m_nFriendCount; ++i)
		{
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astFriendInfo[i].m_iUIN);
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_string(&ptmp, m_astFriendInfo[i].m_szAccount, sizeof(m_astFriendInfo[i].m_szAccount));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astFriendInfo[i].m_iGroupFlag);
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	BlackListInfo& BlackListInfo::operator= (const BlackListInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iUIN = src.m_iUIN;
		STRNCPY(m_szAccount, src.m_szAccount, sizeof(m_szAccount));

		return *this;
	}

	int32_t encode_black_list_info(char** pszOut, BlackListInfo& info)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t info_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, info.m_iUIN);
		info_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, info.m_szAccount, (const int16_t) sizeof(info.m_szAccount));
		info_size += coded_length;

		*pszOut -= (info_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, info_size);

		iOutLength = (info_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_black_list_info(char** pszIn, BlackListInfo& info)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t info_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&info_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iUIN);
		iOutLength += decode_length;
		info_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, info.m_szAccount, (const int16_t)sizeof(info.m_szAccount));
		iOutLength += decode_length;
		info_size -= decode_length;

		if (info_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += info_size;
			iOutLength += info_size;
		}

		return iOutLength;
	}

	CResponseBlackList::CResponseBlackList()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_nCount	= 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseBlackList::~CResponseBlackList()
	{
		//do nothing
	}

	void CResponseBlackList::dump()
	{
		//do nothing
	}

	int32_t CResponseBlackList::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		if (m_nCount > MAX_BLACKLIST_PLAYER_COUNT)
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
			coded_length = CCodeEngine::encode_int16(&ptmp, m_nCount);
			iOutLength += coded_length;

			for (int16_t i=0; i<m_nCount; ++i)
			{
				coded_length = encode_black_list_info(&ptmp, m_astBlackList[i]);
				iOutLength += coded_length;
			}
		}
		else
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseBlackList::decode(const char* pszIn, const int32_t iInLength)
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
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nCount);
			remained_length -= decoded_length;

			if (m_nCount > MAX_BLACKLIST_PLAYER_COUNT)
			{
				return fail;
			}

			for (int16_t i=0; i<m_nCount; ++i)
			{
				decoded_length = decode_black_list_info(&ptmp, m_astBlackList[i]);
				remained_length -= decoded_length;
			}
		}
		else
		{
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestAddBlackList::CRequestAddBlackList()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_stBlackList.m_iUIN = 0;
		m_stBlackList.m_szAccount[0] = '\0';
	}

	CRequestAddBlackList::~CRequestAddBlackList()
	{
		//do nothing
	}

	void CRequestAddBlackList::dump()
	{
		//do nothing
	}

	int32_t CRequestAddBlackList::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = encode_black_list_info(&ptmp, m_stBlackList);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddBlackList::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = decode_black_list_info(&ptmp, m_stBlackList);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseAddBlackList::CResponseAddBlackList()
	{
		m_nResultID = -1;
		m_iMyUIN	= 0;
		m_stBlackList.m_iUIN = 0;
		m_stBlackList.m_szAccount[0] = '\0';
	}

	CResponseAddBlackList::~CResponseAddBlackList()
	{
		//do nothing
	}

	void CResponseAddBlackList::dump()
	{
		//do nothing
	}

	int32_t CResponseAddBlackList::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = encode_black_list_info(&ptmp, m_stBlackList);
		iOutLength += coded_length;

		return success;
	}

	int32_t CResponseAddBlackList::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = decode_black_list_info(&ptmp, m_stBlackList);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestAddNotVerifiedFriend::CRequestAddNotVerifiedFriend()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_iFriendUIN = 0;
		m_szFriendAccount[0] = '\0';
	} 

	CRequestAddNotVerifiedFriend::~CRequestAddNotVerifiedFriend()
	{
		//do nothing
	}

	void CRequestAddNotVerifiedFriend::dump()
	{
		//do nothing
	}

	int32_t CRequestAddNotVerifiedFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestAddNotVerifiedFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szFriendAccount, sizeof(m_szFriendAccount));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CRequestCheckNotVerifiedFriend::CRequestCheckNotVerifiedFriend()
	{
		m_iMyUIN = 0;
		m_iFriendUIN = 0;
	}

	CRequestCheckNotVerifiedFriend::~CRequestCheckNotVerifiedFriend()
	{
		//do nothing
	}

	void CRequestCheckNotVerifiedFriend::dump()
	{
		//do nothing
	}

	int32_t CRequestCheckNotVerifiedFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestCheckNotVerifiedFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseCheckNotVerifiedFriend::CResponseCheckNotVerifiedFriend()
	{
		m_nResultID = -1;
		m_iMyUIN = 0;
		m_iFriendUIN = 0;
	}

	CResponseCheckNotVerifiedFriend::~CResponseCheckNotVerifiedFriend()
	{
		//do nothing
	}

	void CResponseCheckNotVerifiedFriend::dump()
	{
		//do nothing
	}

	int32_t CResponseCheckNotVerifiedFriend::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFriendUIN);
		iOutLength += coded_length;

		return success;
	}

	int32_t CResponseCheckNotVerifiedFriend::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFriendUIN);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	stChatInfo& stChatInfo::operator= (const stChatInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}

		m_iUIN = src.m_iUIN;
		STRNCPY(m_szAccount, src.m_szAccount, sizeof(m_szAccount));
		m_iTime= src.m_iTime;

		return *this;
	}

	bool stChatInfo::operator < (const stChatInfo& other)
	{
		if (m_iTime < other.m_iTime)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int32_t encode_chat_info(char** pszOut, stChatInfo& info)
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t info_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t encode_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		encode_length = CCodeEngine::encode_int32(pszOut, info.m_iUIN);
		info_size += encode_length;

		encode_length = CCodeEngine::encode_string(pszOut, info.m_szAccount, sizeof(info.m_szAccount));
		info_size += encode_length;

		encode_length = CCodeEngine::encode_int32(pszOut, info.m_iTime);
		info_size += encode_length;

		*pszOut -= (info_size + sizeof(int16_t));
		encode_length = CCodeEngine::encode_int16(pszOut, info_size);

		iOutLength = (info_size + encode_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_chat_info(char** pszIn, stChatInfo& info)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t info_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&info_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iUIN);
		iOutLength += decode_length;
		info_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, info.m_szAccount, sizeof(info.m_szAccount));
		iOutLength += decode_length;
		info_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&info.m_iTime);
		iOutLength += decode_length;
		info_size -= decode_length;

		if (info_size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += info_size;
			iOutLength += info_size;
		}

		return iOutLength;
	}

	CChatWithOtherInfo::CChatWithOtherInfo()
	{
		m_iMyUIN = 0;
		m_szMyAccount[0] = '\0';
		m_nCount = 0;
	}

	CChatWithOtherInfo::~CChatWithOtherInfo()
	{
		//do nothing
	}

	void CChatWithOtherInfo::dump()
	{
		//do nothing
	}

	int32_t CChatWithOtherInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		if (m_nCount < 0 || m_nCount > MAX_CHAT_WITH_OTHER_COUNT)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iMyUIN);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_nCount);
		iOutLength += encode_length;

		for(int8_t i=0; i<m_nCount; ++i)
		{
			encode_length = encode_chat_info(&ptmp, m_astInfo[i]);
			iOutLength += encode_length;
		}

		return success;
	}

	int32_t CChatWithOtherInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length  = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUIN);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szMyAccount, sizeof(m_szMyAccount));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nCount);
		remain_length -= decode_length;

		if (m_nCount < 0 || m_nCount > MAX_CHAT_WITH_OTHER_COUNT)
		{
			return fail;
		}

		for(int8_t i=0; i<m_nCount; ++i)
		{
			decode_length = decode_chat_info(&ptmp, m_astInfo[i]);
			remain_length -= decode_length;
		}

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

}}

