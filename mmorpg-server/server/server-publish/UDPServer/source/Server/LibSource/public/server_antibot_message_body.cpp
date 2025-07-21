
#include "public/server_antibot_message_body.h"
#include "Common/common_codeengine.h"

namespace KingNet { namespace Server {



CNotifyAntibotData::CNotifyAntibotData()
{
	m_iUin = 0;
	m_szAccount[0] = '\0' ;
	m_iStreamLength = 0 ;
}

CNotifyAntibotData::~CNotifyAntibotData()
{
	
	
}
int32_t CNotifyAntibotData::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }

    char* ptmp = pszOut;

    iOutLength = 0;

   	iOutLength += CCodeEngine::encode_int32(&ptmp, m_iUin);
    iOutLength += CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iStreamLength);
    if (m_iStreamLength <0 || m_iStreamLength > max_size_antibot_length )
    {
        return fail ;
    }
    iOutLength +=CCodeEngine::encode_memory(&ptmp,m_szStreamContent, m_iStreamLength);
   
	return success;
}

int32_t  CNotifyAntibotData::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 12 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }
    char* ptmp = const_cast<char*>(pszIn);

   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
    CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t) sizeof(m_szAccount));
  
   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iStreamLength);

   	if ( m_iStreamLength < 0 || m_iStreamLength > max_size_antibot_length || m_iStreamLength > (pszIn+iInLength-ptmp) )
   	{
   	    return fail ;
   	}
   	 
    CCodeEngine::decode_memory(&ptmp,m_szStreamContent, m_iStreamLength);

    
    
	return success;	
}

void CNotifyAntibotData::dump()
{
	return ;	
}




CNotifyPunish::CNotifyPunish()
{
	m_iUin = 0;
	m_szAccount[0] = '\0' ;
	m_shSubType = 0 ;
	m_shMode = 0 ;
	m_iValue = 0 ;
}

CNotifyPunish::~CNotifyPunish()
{
	
	
}
int32_t CNotifyPunish::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }

    char* ptmp = pszOut;

    iOutLength = 0;

   	iOutLength += CCodeEngine::encode_int32(&ptmp, m_iUin);
   	iOutLength += CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
    iOutLength += CCodeEngine::encode_int16(&ptmp, m_shSubType);
    iOutLength += CCodeEngine::encode_int16(&ptmp, m_shMode);
    iOutLength += CCodeEngine::encode_int32(&ptmp, m_iValue);
    
	return success;
}

int32_t  CNotifyPunish::decode(const char *pszIn, const int32_t iInLength)
{
    static const int16_t min_size = 14 ;
    if(NULL == pszIn || iInLength < min_size )
    {
        return fail;
    }
    char* ptmp = const_cast<char*>(pszIn);

   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
   	CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t) sizeof(m_szAccount));
   	CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shSubType);
    CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_shMode);
   	CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iValue);

	return success;	
}

void CNotifyPunish::dump()
{
	return ;	
}

}}//namespace KingNet { namespace Server {


