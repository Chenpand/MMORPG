/**
@file   cs_recommend_message.cpp
@brief  recommend.game.51.com和recommend通讯消息的定义
*/

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/cs_recommend_message.h"
#include "public/server_message.h"


namespace KingNet { namespace Server { 

	//请求协议
	CCSRequestRecommend::CCSRequestRecommend()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iStartPos = 0;
		m_iEndPos = 0;

		m_iSex = 0;
		m_iAgeStart = 0;
		m_iAgeEnd = 0;

		m_szLocalProvince[0] = '\0';
		m_szLocalCity[0] = '\0';
		m_szLocalTown[0] = '\0';

		m_szHomeProvince[0] = '\0';
		m_szHomeCity[0] = '\0';
		m_szHomeTown[0] = '\0';
	
		m_iGameID = 0;
	}

	CCSRequestRecommend::~CCSRequestRecommend()
	{

	}

	int32_t CCSRequestRecommend::encode( char *pszOut, int32_t& iOutLength )
	{
		/*
		int32_t  m_iStartPos;
		int32_t  m_iEndPos;

		int32_t  m_iSex;
		int32_t  m_iAgeStart;
		int32_t  m_iAgeEnd;

		char m_szLocalProvince[MAX_CITY_NAME_LEN];
		char m_szLocalCity[MAX_CITY_NAME_LEN];
		char m_szLocalTown[MAX_CITY_NAME_LEN];

		char m_szHomeProvince[MAX_CITY_NAME_LEN];
		char m_szHomeCity[MAX_CITY_NAME_LEN];
		char m_szHomeTown[MAX_CITY_NAME_LEN];

		int32_t  m_iGameID;
		*/
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

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iStartPos);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iEndPos);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAgeStart);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAgeEnd);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szLocalProvince, (const int16_t)sizeof(m_szLocalProvince));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szLocalCity, (const int16_t)sizeof(m_szLocalCity));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szLocalTown, (const int16_t)sizeof(m_szLocalTown));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szHomeProvince, (const int16_t)sizeof(m_szHomeProvince));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szHomeCity, (const int16_t)sizeof(m_szHomeCity));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szHomeTown, (const int16_t)sizeof(m_szHomeTown));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iGameID);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSRequestRecommend::decode( const char *pszIn, const int32_t iInLength )
	{
		/*
		int32_t  m_iStartPos;
		int32_t  m_iEndPos;

		int32_t  m_iSex;
		int32_t  m_iAgeStart;
		int32_t  m_iAgeEnd;

		char m_szLocalProvince[MAX_CITY_NAME_LEN];
		char m_szLocalCity[MAX_CITY_NAME_LEN];
		char m_szLocalTown[MAX_CITY_NAME_LEN];

		char m_szHomeProvince[MAX_CITY_NAME_LEN];
		char m_szHomeCity[MAX_CITY_NAME_LEN];
		char m_szHomeTown[MAX_CITY_NAME_LEN];

		int32_t  m_iGameID;
		*/
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

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iStartPos);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iEndPos);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSex);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAgeStart);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAgeEnd);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szLocalProvince, (const int16_t)sizeof(m_szLocalProvince));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szLocalCity, (const int16_t)sizeof(m_szLocalCity));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szLocalTown, (const int16_t)sizeof(m_szLocalTown));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szHomeProvince, (const int16_t)sizeof(m_szHomeProvince));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szHomeCity, (const int16_t)sizeof(m_szHomeCity));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szHomeTown, (const int16_t)sizeof(m_szHomeTown));
		remained_length -= decoded_length;
	
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameID);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestRecommend::dump()
	{
		return;
	}
	

	//响应协议
	CCSResponseRecommend::CCSResponseRecommend()
	{
		m_iSumHitCount = 0;
		m_iUsedTime = 0;

		m_iReturnCount = 0;
		for(int32_t i=0; i<MAX_RETURN_UIN_COUNT; ++i)
		{
			m_aiUin[i] = 0;
		}		
	}

	CCSResponseRecommend::~CCSResponseRecommend()
	{

	}

	int32_t CCSResponseRecommend::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSumHitCount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUsedTime);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iReturnCount);
		iOutLength += coded_length;

		if(m_iReturnCount>MAX_RETURN_UIN_COUNT)
			m_iReturnCount = MAX_RETURN_UIN_COUNT;

		for(int32_t i=0; i<m_iReturnCount; ++i)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_aiUin[i]);
			iOutLength += coded_length;			
		}

		return success;
	}

	int32_t CCSResponseRecommend::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSumHitCount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUsedTime);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iReturnCount);
		remained_length -= decoded_length;

		if(m_iReturnCount > MAX_RETURN_UIN_COUNT)
			m_iReturnCount = MAX_RETURN_UIN_COUNT;

		for(int32_t i=0; i<m_iReturnCount; ++i)
		{
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aiUin[i]);
			remained_length -= decoded_length;		
		}
				
		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseRecommend::dump()
	{
		printf("m_iSumHitCount:%d \n", m_iSumHitCount);
		printf("m_iUsedTime:%d \n", m_iUsedTime);
		printf("m_iReturnCount:%d \n", m_iReturnCount);
		return;
	}
	
}}//namespace KingNet { namespace Server {


