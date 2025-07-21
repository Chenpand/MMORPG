#include "public/server_vipdb_message_body.h"
#include "Common/common_codeengine.h"
#include "Common/common_string.h"

namespace KingNet { namespace Server {  

CRefreshVIPData::CRefreshVIPData()
{
	m_iUin = 0;	
	m_szAccount[0] ='\0';
	m_iGameVipExpireTime = 0 ;	 
	m_iGameVIPScore = 0;	 
	m_iGameVIPLevel = 0;	 
	m_szVIPTips[0] = '\0';
	m_nNextUpdateTime = 0; 
	m_nLastBecomeVIPTime = 0;   
	m_nNofifyTransparentDataSize = 0;

}
CRefreshVIPData::~CRefreshVIPData()
{

}

int32_t  CRefreshVIPData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,max_account_string_length);
	iOutLength += coded_length;


	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVipExpireTime);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVIPScore);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVIPLevel);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_szVIPTips,sizeof(m_szVIPTips));
	iOutLength += coded_length;


	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nNextUpdateTime);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nNofifyTransparentDataSize);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nVipType);
	iOutLength += coded_length;
	
	 
	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nLastBecomeVIPTime);
	iOutLength += coded_length;
	return success;

}

int32_t  CRefreshVIPData::decode(const char *pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,max_account_string_length);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVipExpireTime);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVIPScore);
	remained_length -= decoded_length;
	
	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVIPLevel);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szVIPTips,sizeof(m_szVIPTips));
	remained_length -= decoded_length;


	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nNextUpdateTime);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nNofifyTransparentDataSize);
	remained_length -= decoded_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}
	
	if(m_nNofifyTransparentDataSize>0)
	{
		decoded_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
		remained_length -= decoded_length;
	}
	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nVipType);
	remained_length -= decoded_length;
	
	
	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nLastBecomeVIPTime);
	remained_length -= decoded_length;
	
	if (remained_length < 0)
	{
		return fail;
	}
	return success;
}

void CRefreshVIPData::dump()
{



}


CRequestUpdatePlayerVIPData::CRequestUpdatePlayerVIPData()
{
	 m_iUin = 0;	
	 m_szAccount[0] = '\0';

	m_szTransTag[0] ='\0';	
	m_nDeltaScore=0; //表示保抚持不变
	m_nTransparentDataSize = 0;
}

CRequestUpdatePlayerVIPData::~CRequestUpdatePlayerVIPData()
{

}

int32_t  CRequestUpdatePlayerVIPData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,max_account_string_length);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,max_game_tag_length);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nDeltaScore);
	iOutLength += coded_length;

	if(m_nTransparentDataSize>max_sub_message_size)
		return fail;
	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	iOutLength += coded_length;

	return success;	
}

int32_t  CRequestUpdatePlayerVIPData::decode(const char *pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,max_account_string_length);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nDeltaScore);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decoded_length;

	if(m_nTransparentDataSize>max_sub_message_size)
		return fail;


	decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}
	return success;

}
void CRequestUpdatePlayerVIPData::dump()
{

}



CResponseUpdatePlayerVIPData::CResponseUpdatePlayerVIPData()
{
	m_iUin = 0;	
	m_szAccount[0] = '\0';
	m_szTransTag[0] = '\0';
	m_nResultID = 0;
	m_nVipType = 0;  
	m_iGameVipExpireTime = 0;//游戏VIP的过期时间(如果没有开通过，时间为0)
	m_iGameVIPScore = 0;	//游戏VIP积分
	m_iGameVIPLevel = 0;	//游戏VIP等级
	m_nNextUpdateTime = 0;      //下次升级的绝对时间

	m_nTransparentDataSize = 0;
}

CResponseUpdatePlayerVIPData::~CResponseUpdatePlayerVIPData()
{

}

int32_t  CResponseUpdatePlayerVIPData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,max_account_string_length);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,max_game_tag_length);
	iOutLength += coded_length;

	

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += coded_length;

	if(m_nResultID == result_id_success)
	{
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nVipType);
		iOutLength += coded_length;
	
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVipExpireTime);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVIPScore);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameVIPLevel);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nNextUpdateTime);
		iOutLength += coded_length;
	}

	if(m_nTransparentDataSize>max_sub_message_size)
		return fail;
	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	iOutLength += coded_length;
	return success;

}

int32_t  CResponseUpdatePlayerVIPData::decode(const char *pszIn, const int32_t iInLength)
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

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,max_account_string_length);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	remained_length -= decoded_length;
	
	

 	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remained_length -= decoded_length;

	if(m_nResultID == result_id_success)
	{
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nVipType);
		remained_length -= decoded_length;
	
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVipExpireTime);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVIPScore);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameVIPLevel);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nNextUpdateTime);
		remained_length -= decoded_length;
	}

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decoded_length;
	
	if(m_nTransparentDataSize>max_sub_message_size)
		return fail;

	decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	remained_length -= decoded_length;


	if (remained_length < 0)
	{
		return fail;
	}
	return success;
}

void CResponseUpdatePlayerVIPData::dump()
{

}

}}

