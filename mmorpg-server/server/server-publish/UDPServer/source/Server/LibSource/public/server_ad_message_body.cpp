#include "Common/common_codeengine.h"
#include "Common/common_string.h"
#include "public/server_ad_message_body.h"

namespace KingNet { namespace Server {

	CRspADInfo::CRspADInfo ()
	{
		m_szURL[0] = '\0';
	}

	CRspADInfo::~CRspADInfo ()
	{
		// do nothing
	}

	void CRspADInfo::dump ()
	{
		// do nothing
	}

	int32_t CRspADInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szURL, sizeof(m_szURL));
		iOutLength += encode_length;

		return success;
	}

	int32_t CRspADInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szURL, sizeof(m_szURL));
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	CRequestPublishAD::CRequestPublishAD()
	{
		memset(&m_stADInfo,0,sizeof(m_stADInfo));
	}
	CRequestPublishAD::~CRequestPublishAD()
	{

	}
	int32_t CRequestPublishAD::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stADInfo.m_iADInfoLength);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_stADInfo.m_iADID);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stADInfo.m_iStartTime);
		iOutLength += encode_length;
		encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_stADInfo.m_iEndTime);
		iOutLength += encode_length; 
		encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_stADInfo.m_shCycleType);
		iOutLength += encode_length; 

		encode_length = CCodeEngine::encode_string(&ptmp,m_stADInfo.m_szURL,sizeof(m_stADInfo.m_szURL));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_stADInfo.m_iMacCount);
		iOutLength += encode_length;

		return success;
	}
	int32_t CRequestPublishAD::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stADInfo.m_iADInfoLength);
		remain_length -= decode_length;
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stADInfo.m_iADID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stADInfo.m_iStartTime);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stADInfo.m_iEndTime);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stADInfo.m_shCycleType);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_stADInfo.m_szURL,sizeof(m_stADInfo.m_szURL));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stADInfo.m_iMacCount);
		remain_length -= decode_length;

		if(remain_length<0)
		{
			return fail;
		}
		return success;

	}
	void CRequestPublishAD::dump()
	{

	}
	////////////////////////////////////////////////////////////////////////////////////////////
	CResponsePublishAD::CResponsePublishAD()
	{
		m_iADID = 0;
		m_shResultID = result_id_success;
		m_szReasonMessage[0] = '\0';
	}
	CResponsePublishAD::~CResponsePublishAD()
	{

	}
	int32_t CResponsePublishAD::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iADID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shResultID);
		iOutLength += encode_length;

		if( result_id_success != m_shResultID )
		{
			encode_length = CCodeEngine::encode_string(&ptmp,m_szReasonMessage,sizeof(m_szReasonMessage));
			iOutLength += encode_length;
		}

		return success;
	}
	int32_t CResponsePublishAD::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iADID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remain_length -= decode_length;

		if( result_id_success != m_shResultID )
		{
			decode_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage ,sizeof(m_szReasonMessage));
			remain_length -= decode_length;
		}
		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CResponsePublishAD::dump()
	{

	}
	///////////////////////////////////////////////////////////////////////////////
	CRequestDeleteAD::CRequestDeleteAD()
	{
		m_iADID = 0;
	}
	CRequestDeleteAD::~CRequestDeleteAD()
	{

	}
	int32_t CRequestDeleteAD::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iADID);
		iOutLength += encode_length;

		return success;
	}
	int32_t CRequestDeleteAD::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iADID);
		remain_length -= decode_length;

		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CRequestDeleteAD::dump()
	{

	}
	///////////////////////////////////////////////////////////////////////////
	CResponseDeleteAD::CResponseDeleteAD()
	{
		m_iADID = 0;
		m_shResultID = result_id_success;
		m_szReasonMessage[0] = '\0';
	}
	CResponseDeleteAD::~CResponseDeleteAD()
	{

	}
	int32_t CResponseDeleteAD::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iADID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shResultID);
		iOutLength += encode_length;

		if( result_id_success != m_shResultID )
		{
			encode_length = CCodeEngine::encode_string(&ptmp,m_szReasonMessage,sizeof(m_szReasonMessage));
			iOutLength += encode_length;
		}

		return success;
	}
	int32_t CResponseDeleteAD::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iADID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remain_length -= decode_length;

		if( result_id_success != m_shResultID )
		{
			decode_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage ,sizeof(m_szReasonMessage));
			remain_length -= decode_length;
		}
		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CResponseDeleteAD::dump()
	{

	}

	///////////////////////////////////////////////////////////////////////////
	CRequestGetADList::CRequestGetADList()
	{
		m_shType = 0;
	}
	CRequestGetADList::~CRequestGetADList()
	{

	}
	int32_t CRequestGetADList::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint32_t)m_shType);
		iOutLength += encode_length;

		return success;
	}
	int32_t CRequestGetADList::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shType);
		remain_length -= decode_length;

		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CRequestGetADList::dump()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	CResponseGetADList::CResponseGetADList()
	{
		m_shResultID = 0;
		m_shADInfoCount = 0;
		memset(m_szADInfo,0,sizeof(m_szADInfo));
		m_szReasonMessage[0] = '\0';
	}
	CResponseGetADList::~CResponseGetADList()
	{

	}
	int32_t CResponseGetADList::encode(char* pszOut, int32_t& iOutLength)
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

		
		if( result_id_success != m_shResultID )
		{
			encode_length = CCodeEngine::encode_string(&ptmp,m_szReasonMessage,sizeof(m_szReasonMessage));
			iOutLength += encode_length;
		}
		else
		{
			encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_shADInfoCount);
			iOutLength += encode_length;
			if( m_shADInfoCount > max_adinfo_count)
			{
				m_shADInfoCount = max_adinfo_count;
			}
			for( int16_t iCount = 0; iCount < m_shADInfoCount; ++iCount)
			{
				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szADInfo[iCount].m_iADInfoLength);
				iOutLength += encode_length;
				encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_szADInfo[iCount].m_iADID);
				iOutLength += encode_length;
				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_szADInfo[iCount].m_iStartTime);
				iOutLength += encode_length;
				encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_szADInfo[iCount].m_iEndTime);
				iOutLength += encode_length; 
				encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_szADInfo[iCount].m_shCycleType);
				iOutLength += encode_length; 

				encode_length = CCodeEngine::encode_string(&ptmp,m_szADInfo[iCount].m_szURL,sizeof(m_szADInfo[iCount].m_szURL));
				iOutLength += encode_length;

				encode_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_szADInfo[iCount].m_iMacCount);
				iOutLength += encode_length;

				encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_szADInfo[iCount].m_shState);
				iOutLength += encode_length;
			}
		}

		return success;
	}
	int32_t CResponseGetADList::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;
		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shResultID);
		remain_length -= decode_length;
		if( result_id_success == m_shResultID)
		{
			decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shADInfoCount);
			remain_length -= decode_length;
			if( m_shADInfoCount > max_adinfo_count)
			{
				m_shADInfoCount = max_adinfo_count;
			}
			for (int16_t iCount=0;iCount < m_shADInfoCount; ++iCount)
			{
				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szADInfo[iCount].m_iADInfoLength);
				remain_length -= decode_length;
				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szADInfo[iCount].m_iADID);
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szADInfo[iCount].m_iStartTime);
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szADInfo[iCount].m_iEndTime);
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_szADInfo[iCount].m_shCycleType);
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_string(&ptmp, m_szADInfo[iCount].m_szURL,sizeof(m_szADInfo[iCount].m_szURL));
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szADInfo[iCount].m_iMacCount);
				remain_length -= decode_length;

				decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_szADInfo[iCount].m_shState);
				remain_length -= decode_length;
			}
		}
		else
		{
			decode_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage ,sizeof(m_szReasonMessage));
			remain_length -= decode_length;
		}
		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CResponseGetADList::dump()
	{

	}
	///////////////////////////////////////////////////////////////////////////////
	CRequestSendMacList::CRequestSendMacList()
	{
		m_iADID = 0;
		m_iMacCount = 0;
		memset(&m_szMac,0,sizeof(m_szMac));
	}
	CRequestSendMacList::~CRequestSendMacList()
	{

	}
	int32_t CRequestSendMacList::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		int16_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iADID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMacCount);
		iOutLength += encode_length;

		if( m_iMacCount > max_mac_list_count)
		{
			m_iMacCount = max_mac_list_count;
		}
		for( int32_t iCount=0;iCount < m_iMacCount; ++iCount)
		{
			encode_length = CCodeEngine::encode_int64(&ptmp,(uint64_t)m_szMac[iCount]);
			iOutLength += encode_length;
		}

		return success;
	}
	int32_t CRequestSendMacList::decode(const char* pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remain_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iADID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMacCount);
		remain_length -= decode_length;

		if( m_iMacCount > max_mac_list_count)
		{
			m_iMacCount = max_mac_list_count;
		}
		if( m_iMacCount > 0)
		{
			for(int32_t iCount=0; iCount < m_iMacCount; ++iCount)
			{
				decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_szMac[iCount]);
				remain_length -= decode_length;
			}
		}

		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CRequestSendMacList::dump()
	{

	}
	///////////////////////////////////////////////////////////////////////////////////
	CResponseSendMacList::CResponseSendMacList()
	{
		m_shResultID = 0;
		m_szReasonMessage[0] = '\0';
	}
	CResponseSendMacList::~CResponseSendMacList()
	{

	}
	int32_t CResponseSendMacList::encode(char* pszOut, int32_t& iOutLength)
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

		if( result_id_success != m_shResultID )
		{
			encode_length = CCodeEngine::encode_string(&ptmp,m_szReasonMessage,sizeof(m_szReasonMessage));
			iOutLength += encode_length;
		}

		return success;
	}
	int32_t CResponseSendMacList::decode(const char* pszIn, const int32_t iInLength)
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
			decode_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage ,sizeof(m_szReasonMessage));
			remain_length -= decode_length;
		}
		if(remain_length<0)
		{
			return fail;
		}
		return success;
	}
	void CResponseSendMacList::dump()
	{

	}

}}
