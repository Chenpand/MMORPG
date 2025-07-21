#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/server_groupdb_message_body.h"

namespace KingNet { namespace Server {

	CGroupRankInfo::CGroupRankInfo()
	{
		if (CObject::m_iCreateMode != object_createmode_init)
		{
			return;
		}
		else
		{
			clear();
		}
	}

	CGroupRankInfo::~CGroupRankInfo()
	{
		//do nothing
	}

	void CGroupRankInfo::clear()
	{
		m_szGroupID[0] = '\0';
		m_iCharming = 0;
		m_iAchievement = 0;
		m_iHappyBean = 0;
		m_i51Point = 0;
		m_iSumPlayGameTime = 0;
	}

	
	CGroupRankInfo & CGroupRankInfo::operator=( const CGroupRankInfo& src)
	{
		if (this == &src)
		{
			return *this;
		}
		
		memset(m_szGroupID, 0, sizeof(m_szGroupID));
		memcpy(m_szGroupID, src.m_szGroupID, sizeof(m_szGroupID));
		m_iCharming = src.m_iCharming;
		m_iAchievement = src.m_iAchievement;
		m_iHappyBean = src.m_iHappyBean;
		m_i51Point = src.m_i51Point;
		m_iSumPlayGameTime = src.m_iSumPlayGameTime;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////

	int32_t encode_rankinfo(char** pszOut, stGroupRank& groupInfo)
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

		coded_length = CCodeEngine::encode_int32(pszOut, groupInfo.m_iRank);
		size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, groupInfo.m_szGroupID, (const int16_t) sizeof(groupInfo.m_szGroupID));
		size += coded_length;

		coded_length = CCodeEngine::encode_int64(pszOut, groupInfo.m_iRankValue);
		size += coded_length;

		*pszOut -= (size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, size);

		iOutLength = (size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_rankinfo(char** pszIn, stGroupRank& groupInfo)
	{
		if (*pszIn == NULL || pszIn == NULL)
		{
			return 0;
		}

		int16_t size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&groupInfo.m_iRank);
		iOutLength += decode_length;
		size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszIn, groupInfo.m_szGroupID, (const int16_t)sizeof(groupInfo.m_szGroupID));
		iOutLength += decode_length;
		size -= decode_length;

		decode_length = CCodeEngine::decode_int64(pszIn, (uint64_t*)&groupInfo.m_iRankValue);
		iOutLength += decode_length;
		size -= decode_length;

		if (size < 0)
		{
			return 0;
		}
		else //对于不能解码的字节流，跳过
		{
			(*pszIn) += size;
			iOutLength += size;
		}

		return iOutLength;
	}


	//////////////////////////////////////////////////////////////////////////
	//请求群排名
	//SS_MSG_GET_GROUP_RANK
	CRequestGetGroupRankInfo::CRequestGetGroupRankInfo()
	{
		memset(m_szGroupID, 0, max_qun_uid_length);
		m_byRankType = 0;
		m_iBeforRankNum = 0;
		m_iAfterRankNum = 0;	
	}

	CRequestGetGroupRankInfo::~CRequestGetGroupRankInfo()
	{

	}

	int32_t CRequestGetGroupRankInfo::encode(char* pszOut, int32_t& iOutLength)
	{	
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupID, (const int16_t)sizeof(m_szGroupID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byRankType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iBeforRankNum);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAfterRankNum);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestGetGroupRankInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupID, (const int16_t)sizeof(m_szGroupID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byRankType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iBeforRankNum);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAfterRankNum);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestGetGroupRankInfo::dump()
	{

	}

	//请求群排名响应
	CResponseGetGroupRankInfo::CResponseGetGroupRankInfo()
	{
		m_nResultID = 0;           //返回值，success为成功，否则为失败 
		m_szGroupID[0] = '\0';
		m_byRankType = 0;
		m_shReturnCount = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseGetGroupRankInfo::~CResponseGetGroupRankInfo()
	{
		//do nothing
	}

	int32_t CResponseGetGroupRankInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGroupID, (const int16_t)sizeof(m_szGroupID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byRankType);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
			return success;
		}
		
		if (MAX_GROUP_RANK_COUNT < m_shReturnCount)
		{
			m_shReturnCount = MAX_GROUP_RANK_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shReturnCount);
		iOutLength += coded_length;

		for (int16_t i = 0; i<m_shReturnCount; ++i)
		{                  
			coded_length = encode_rankinfo(&ptmp, m_astRankInfo[i]);
			iOutLength += coded_length;
		}
		
		return success;
	}

	int32_t CResponseGetGroupRankInfo::decode(const char* pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGroupID, (const int16_t)sizeof(m_szGroupID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byRankType);
		remained_length -= decoded_length;

		if (result_id_success == m_nResultID)
		{
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shReturnCount);
			remained_length -= decoded_length;

			if (MAX_GROUP_RANK_COUNT < m_shReturnCount)
			{
				m_shReturnCount = MAX_GROUP_RANK_COUNT;
			}
			for (int16_t i  = 0; i<m_shReturnCount; ++i)
			{
				decoded_length = decode_rankinfo(&ptmp, m_astRankInfo[i]);
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

	void CResponseGetGroupRankInfo::dump()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	//请求群top排名
	//SS_MSG_GET_GROUP_TOP_RANK
	CRequestGetGroupTopRankInfo::CRequestGetGroupTopRankInfo()
	{
		m_byRankType = 0;
		m_iRankNum = 0;	
	}

	CRequestGetGroupTopRankInfo::~CRequestGetGroupTopRankInfo()
	{

	}

	int32_t CRequestGetGroupTopRankInfo::encode(char* pszOut, int32_t& iOutLength)
	{	
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byRankType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRankNum);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestGetGroupTopRankInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byRankType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRankNum);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestGetGroupTopRankInfo::dump()
	{

	}

	//请求群排名响应
	CResponseGetGroupTopRankInfo::CResponseGetGroupTopRankInfo()
	{
		m_nResultID = 0;           //返回值，success为成功，否则为失败 
		m_byRankType = 0;
		m_shReturnCount = 0;
		m_szReasonMessage[0] = '\0';
	}

	CResponseGetGroupTopRankInfo::~CResponseGetGroupTopRankInfo()
	{
		//do nothing
	}

	int32_t CResponseGetGroupTopRankInfo::encode(char* pszOut, int32_t& iOutLength)
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

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byRankType);
		iOutLength += coded_length;

		if (result_id_success != m_nResultID)
		{
			coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
			iOutLength += coded_length;
			return success;
		}

		if (MAX_GROUP_TOP_RANK_COUNT < m_shReturnCount)
		{
			m_shReturnCount = MAX_GROUP_TOP_RANK_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shReturnCount);
		iOutLength += coded_length;

		for (int16_t i = 0; i<m_shReturnCount; ++i)
		{
			coded_length = encode_rankinfo(&ptmp, m_astRankInfo[i]);
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CResponseGetGroupTopRankInfo::decode(const char* pszIn, const int32_t iInLength)
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
		
		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byRankType);
		remained_length -= decoded_length;

		if (result_id_success == m_nResultID)
		{
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shReturnCount);
			remained_length -= decoded_length;

			if (MAX_GROUP_TOP_RANK_COUNT < m_shReturnCount)
			{
				m_shReturnCount = MAX_GROUP_TOP_RANK_COUNT;
			}
			for (int16_t i  = 0; i<m_shReturnCount; ++i)
			{
				decoded_length = decode_rankinfo(&ptmp, m_astRankInfo[i]);
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

	void CResponseGetGroupTopRankInfo::dump()
	{

	}
}}

