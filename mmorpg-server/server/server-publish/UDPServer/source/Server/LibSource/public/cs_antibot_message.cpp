
#include "public/cs_antibot_message.h"
#include "Common/common_codeengine.h"

namespace KingNet { namespace Server {

CCSAntibotSubHead::CCSAntibotSubHead()
{
	m_shMsgID = 0;
   
}

CCSAntibotSubHead::~CCSAntibotSubHead()
{
	
	
}
int32_t CCSAntibotSubHead::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }


    char* ptmp = pszOut;

    iOutLength = 0;

   	iOutLength += CCodeEngine::encode_int16(&ptmp, m_shMsgID);
   	
	return success;
}

int32_t  CCSAntibotSubHead::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 2 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }

  
    char* ptmp = const_cast<char*>(pszIn);

    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shMsgID);
    
	return success;	
}

void CCSAntibotSubHead::dump()
{
	return ;	
}




CCSNotifyAntibot::CCSNotifyAntibot()
{
	m_iUin = 0;
    m_iMask = 0 ;
	
}

CCSNotifyAntibot::~CCSNotifyAntibot()
{
	
	
}
int32_t CCSNotifyAntibot::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }


    char* ptmp = pszOut;

    iOutLength = 0;

   	iOutLength += CCodeEngine::encode_int32(&ptmp, m_iUin);
   	iOutLength += CCodeEngine::encode_int32(&ptmp, m_iMask);

	return success;
}

int32_t  CCSNotifyAntibot::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 4 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }

  
    char* ptmp = const_cast<char*>(pszIn);

    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMask);
   	
	return success;	
}

void CCSNotifyAntibot::dump()
{
	return ;	
}




CCSRequestAntibotCheck::CCSRequestAntibotCheck()
{
	m_iUin = 0;
	m_iSeq = 0 ;
	m_iType = 0 ;
	m_shStreamLength = 0 ;
}

CCSRequestAntibotCheck::~CCSRequestAntibotCheck()
{
	
	
}
int32_t CCSRequestAntibotCheck::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }

    char* ptmp = pszOut;

    iOutLength = 0;

   	iOutLength += CCodeEngine::encode_int32(&ptmp, m_iUin);
    
    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iSeq);
  
    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iType);
    
    iOutLength += CCodeEngine::encode_int16(&ptmp, m_shStreamLength);
    if ( m_shStreamLength < 0 || m_shStreamLength > max_size_stream_length )
    {
        return fail ;
    }
    iOutLength +=CCodeEngine::encode_memory(&ptmp,m_szStreamContent, m_shStreamLength);
   
	return success;
}

int32_t  CCSRequestAntibotCheck::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 14 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }
    char* ptmp = const_cast<char*>(pszIn);

   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);

    
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iSeq);

   	 
   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iType);

   	 
   	CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shStreamLength);

   	if ( m_shStreamLength < 0 || m_shStreamLength > max_size_stream_length || m_shStreamLength > ( pszIn + iInLength - ptmp) )
   	{
   	    return fail ;
   	}
   	 
    CCodeEngine::decode_memory(&ptmp,m_szStreamContent, m_shStreamLength);

    
    
	return success;	
}

void CCSRequestAntibotCheck::dump()
{
	return ;	
}




CCSResponseAntibotCheck::CCSResponseAntibotCheck()
{
	m_iUin = 0;
	m_iSeq = 0 ;
	m_iResult = 0 ;
	m_iType = 0 ;
	m_shStreamLength = 0 ;
	
}

CCSResponseAntibotCheck::~CCSResponseAntibotCheck()
{
	
	
}
int32_t CCSResponseAntibotCheck::encode(char *pszOut, int32_t& iOutLength)
{
    return fail;
    
}

int32_t  CCSResponseAntibotCheck::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 10 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }

    int16_t offset_uin = 0 , offset_seq = 0 ,  offset_type = 0 ,offset_result = 0 , offset_stream = 0 ;
    
    char* ptmp = const_cast<char*>(pszIn);

    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&offset_uin);
    if ( offset_uin < min_size || offset_uin > iInLength - (int32_t)sizeof(m_iUin) ) return fail ;
    
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&offset_seq);
    if ( offset_seq < min_size || offset_seq > iInLength - (int32_t)sizeof(m_iSeq) ) return fail ;
    
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&offset_type);
    if ( offset_type < min_size || offset_type > iInLength - (int32_t)sizeof(m_iType) ) return fail ;
    
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&offset_result);
    if ( offset_result < min_size || offset_result > iInLength - (int32_t)sizeof(m_iResult) ) return fail ;
    
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&offset_stream);
    if ( offset_stream < min_size || offset_stream > iInLength - (int32_t)sizeof(m_shStreamLength) ) return fail ;
    
    ptmp = const_cast<char*>(pszIn+offset_uin);
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
    
    ptmp = const_cast<char*>(pszIn+offset_seq);
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iSeq);
    
    ptmp = const_cast<char*>(pszIn+offset_type);
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iType);
    
    ptmp = const_cast<char*>(pszIn+offset_result);
    CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iResult);
    
    ptmp = const_cast<char*>(pszIn+offset_stream);
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shStreamLength);
    if ( m_shStreamLength < 0 || m_shStreamLength > max_size_stream_length || m_shStreamLength > (pszIn + iInLength - ptmp))
   	{
   	    return fail ;
   	}
    CCodeEngine::decode_memory(&ptmp,m_szStreamContent, m_shStreamLength);                    
    
	return success;	
}

void CCSResponseAntibotCheck::dump()
{
	return ;	
}

}}//namespace KingNet { namespace Server {


