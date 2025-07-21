/**
@file   cs_market_message.cpp
@brief  fast-cgi和market server通讯消息的定义

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/cs_market_message.h"
#include "public/server_message.h"


namespace KingNet { namespace Server { 

	//购买/赠送物品请求协议
	CCSRequestBuyCommodity::CCSRequestBuyCommodity()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
        m_szSrcNickname[0] = '\0';
		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_shCommodityCount = 0;
		
		m_nTransparentDataSize = 0 ;
	}

	CCSRequestBuyCommodity::~CCSRequestBuyCommodity()
	{

	}
   
    
	int32_t CCSRequestBuyCommodity::encode( char *pszOut, int32_t& iOutLength )
	{
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
        
        coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcNickname, (const int16_t)sizeof(m_szSrcNickname));
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int8(&ptmp, m_cSrcGender);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int8(&ptmp, m_cVipLevel);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shPaymentMode);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iCommodityPrice);
		iOutLength += coded_length;
		
		if(m_shCommodityCount > MAX_COMMODITY_SIZE)
		{
			m_shCommodityCount = MAX_COMMODITY_SIZE;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shCommodityCount);
		iOutLength += coded_length;
				
		
		for(int32_t i=0; i<m_shCommodityCount; ++i)
		{
		    
            coded_length = CCodeEngine::encode_int16(&ptmp, 6 );
			iOutLength += coded_length;
					   
			coded_length = CCodeEngine::encode_int32(&ptmp, m_CommodityData[i].m_iID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_CommodityData[i].m_shCount);
			iOutLength += coded_length;
		}
		
		//留言信息超过规定的大小，则进行截断
		if ((int16_t)strlen(m_szWord) > MAX_USER_WORD_LENGTH)
		{
			m_szWord[MAX_USER_WORD_LENGTH] = '\0';
		}

		coded_length = CCodeEngine::encode_string(&ptmp, m_szWord, (const int16_t)sizeof(m_szWord));
		iOutLength += coded_length;

        if ( m_nTransparentDataSize > MAX_TRANSPARENT_LENGTH ) m_nTransparentDataSize = MAX_TRANSPARENT_LENGTH ;
            
	    coded_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	    iOutLength += coded_length;
 
	    coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	    iOutLength += coded_length;
			
		return success;
	}

	int32_t CCSRequestBuyCommodity::decode( const char *pszIn, const int32_t iInLength )
	{
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
        
        decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcNickname, (const int16_t)sizeof(m_szSrcNickname));
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cSrcGender);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cVipLevel);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shPaymentMode);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCommodityPrice);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shCommodityCount);
		remained_length -= decoded_length;

		if (MAX_COMMODITY_SIZE < m_shCommodityCount)
		{
			m_shCommodityCount = MAX_COMMODITY_SIZE;
		}

		for(int32_t i = 0; i<m_shCommodityCount; ++i)
		{
		    uint16_t tmp = 0 ;
		    decoded_length = CCodeEngine::decode_int16(&ptmp, &tmp );
			remained_length -= decoded_length;
			
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_CommodityData[i].m_iID);
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_CommodityData[i].m_shCount);
			remained_length -= decoded_length;
		}
		
		decoded_length = CCodeEngine::decode_string(&ptmp, m_szWord, (const int16_t)sizeof(m_szWord));
		remained_length -= decoded_length;

	    decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	    remained_length -= decoded_length;

	    if (m_nTransparentDataSize > MAX_TRANSPARENT_LENGTH)
	    {
		    return fail;
	    }

	    if(m_nTransparentDataSize > 0)
	    {
		    decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
		    remained_length -= decoded_length;
	    }
	
		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestBuyCommodity::dump()
	{
		return;
	}
	

	//购买/赠送物品响应协议
	CCSResponseBuyCommodity::CCSResponseBuyCommodity()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_iPrice = 0 ;
		m_shCommodityCount = 0 ;
		m_iBean = 0 ;
		m_nBeanFlag = -1;
		m_shItemCount = 0 ;
		
		m_nTransparentDataSize = 0 ;
	}

	CCSResponseBuyCommodity::~CCSResponseBuyCommodity()
	{

	}

	int32_t CCSResponseBuyCommodity::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iPrice);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int16(&ptmp, m_shCommodityCount);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int64(&ptmp, m_iBean);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int16(&ptmp, m_nBeanFlag);
		iOutLength += coded_length;
        coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;
		
		for(int32_t i=0; i<m_shItemCount; ++i)
		{
		    
                coded_length = CCodeEngine::encode_int16(&ptmp, 6 );
			    iOutLength += coded_length;
					   
			    coded_length = CCodeEngine::encode_int32(&ptmp, m_ItemInfo[i].m_iID);
			    iOutLength += coded_length;

			    coded_length = CCodeEngine::encode_int16(&ptmp, m_ItemInfo[i].m_shCount);
			    iOutLength += coded_length;
		}
		
        if ( m_nTransparentDataSize > MAX_TRANSPARENT_LENGTH ) m_nTransparentDataSize = MAX_TRANSPARENT_LENGTH ;
            
	    coded_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	    iOutLength += coded_length;
 
	    coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
	    iOutLength += coded_length;
	    
		return success;
	}

	int32_t CCSResponseBuyCommodity::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPrice);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shCommodityCount);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_iBean);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nBeanFlag);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;
		for(int32_t i = 0; i<m_shItemCount; ++i)
		{
		    uint16_t tmp = 0 ;
		    decoded_length = CCodeEngine::decode_int16(&ptmp, &tmp );
			remained_length -= decoded_length;
			
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_ItemInfo[i].m_iID);
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_ItemInfo[i].m_shCount);
			remained_length -= decoded_length;
		}
		

	    decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	    remained_length -= decoded_length;

	    if (m_nTransparentDataSize > MAX_TRANSPARENT_LENGTH)
	    {
		    return fail;
	    }

	    if(m_nTransparentDataSize > 0)
	    {
		    decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, m_nTransparentDataSize);
		    remained_length -= decoded_length;
	    }
	    
		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseBuyCommodity::dump()
	{
		return;
	}
	
	
	
	
	
	CCSRequestItemExchange::CCSRequestItemExchange()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shItemCount = 0 ;
		m_iItemID = 0;
		m_iClientIP = 0 ;
	}

	CCSRequestItemExchange::~CCSRequestItemExchange()
	{

	}

	CCSRequestItemExchange& CCSRequestItemExchange::operator =(const CCSRequestItemExchange& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iUin = original.m_iUin;		
		STRNCPY(m_szAccount, original.m_szAccount, sizeof(m_szAccount));

		m_shItemCount = original.m_shItemCount;
		m_iItemID = original.m_iItemID;
        m_iClientIP = original.m_iClientIP ;
		
		return *this;
	}

	int32_t CCSRequestItemExchange::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;


		coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iItemID);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		
		return success;
	}

	int32_t CCSRequestItemExchange::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iItemID);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		

		return success;
	}

	void CCSRequestItemExchange::dump()
	{
		return;
	}



    
    CCSResponseItemExchange::CCSResponseItemExchange()
	{
	    m_shResultID = 0 ;
		m_iUin = 0;
		m_szAccount[0] = '\0';

		m_iGameMoney = 0 ;


	}

	CCSResponseItemExchange::~CCSResponseItemExchange()
	{

	}

	int32_t CCSResponseItemExchange::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iGameMoney);
		iOutLength += coded_length;
		

		

		return success;
	}

	int32_t CCSResponseItemExchange::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameMoney);
		remained_length -= decoded_length;

		

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseItemExchange::dump()
	{
		return;
	}
	
	
	
	CCSRequestRenewService::CCSRequestRenewService()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_iClientIP = 0;
		m_iRenewID = 0 ;
		m_cVipLevel = 0 ;
	}

	CCSRequestRenewService::~CCSRequestRenewService()
	{

	}

	

	int32_t CCSRequestRenewService::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;


		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iRenewID);
		iOutLength += coded_length;
		
        coded_length = CCodeEngine::encode_int8(&ptmp, m_cVipLevel);
		iOutLength += coded_length;
				
		return success;
	}

	int32_t CCSRequestRenewService::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRenewID);
		remained_length -= decoded_length;
		
        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cVipLevel);
		remained_length -= decoded_length;
		
		return success;
	}

	void CCSRequestRenewService::dump()
	{
		return;
	}
	
	
	
	CCSResponseRenewService::CCSResponseRenewService()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_nResultID = 0 ;
		m_iServiceID = 0;
		m_iRenewID = 0 ;
		m_iPrice = 0 ;
		m_iBean = 0 ;
	}

	CCSResponseRenewService::~CCSResponseRenewService()
	{

	}

	

	int32_t CCSResponseRenewService::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServiceID);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iRenewID);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPrice);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int64(&ptmp, m_iBean);
		iOutLength += coded_length;
		return success;
	}

	int32_t CCSResponseRenewService::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServiceID);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRenewID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPrice);
		remained_length -= decoded_length;
        decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_iBean);
		remained_length -= decoded_length;
		return success;
	}

	void CCSResponseRenewService::dump()
	{
		return;
	}
	
	
	CCSRequestLuckyDraw::CCSRequestLuckyDraw()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_iChestID = 0;
		
	}

	CCSRequestLuckyDraw::~CCSRequestLuckyDraw()
	{

	}
	int32_t CCSRequestLuckyDraw::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;


		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iChestID);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSRequestLuckyDraw::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChestID);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		return success;
	}

	void CCSRequestLuckyDraw::dump()
	{
		return;
	}
	
	
	
	CCSResponseLuckyDraw::CCSResponseLuckyDraw()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_iChestID = 0;
		m_shChestCount = 0 ;
		m_iMoney = 0 ;
		m_iBean = 0 ;
		m_iItemID = 0;
		m_shItemCount = 0 ;
		
	}

	CCSResponseLuckyDraw::~CCSResponseLuckyDraw()
	{

	}

	

	int32_t CCSResponseLuckyDraw::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iChestID);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shChestCount);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iMoney);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iBean);
		iOutLength += coded_length;
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iItemID);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;
		
		
		
		return success;
	}

	int32_t CCSResponseLuckyDraw::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChestID);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shChestCount);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iBean);
		remained_length -= decoded_length;
		
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iItemID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;

		return success;
	}

	void CCSResponseLuckyDraw::dump()
	{
		return;
	}
	
	
	
	
	
	
    CCSRequestGetActionInfo::CCSRequestGetActionInfo()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		
		
	}

	CCSRequestGetActionInfo::~CCSRequestGetActionInfo()
	{

	}
	int32_t CCSRequestGetActionInfo::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSRequestGetActionInfo::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		return success;
	}

	void CCSRequestGetActionInfo::dump()
	{
		return;
	}
	
	
	
	CCSResponseGetActionInfo::CCSResponseGetActionInfo()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_iOnlineSeconds = 0;
		m_shMaxHit = 0 ;
		m_shCurHit = 0 ;
		m_iMoney = 0;
		m_iBean = 0 ;
		m_iScore = 0 ;
		m_iItemID = 0;
		m_shItemCount = 0 ;
		
	}

	CCSResponseGetActionInfo::~CCSResponseGetActionInfo()
	{

	}

	

	int32_t CCSResponseGetActionInfo::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iOnlineSeconds);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shMaxHit);
		iOutLength += coded_length;
         coded_length = CCodeEngine::encode_int16(&ptmp, m_shCurHit);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iMoney);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iBean);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iScore);
		iOutLength += coded_length;
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iItemID);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;
		
		
		
		return success;
	}

	int32_t CCSResponseGetActionInfo::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iOnlineSeconds);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shMaxHit);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shCurHit);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iBean);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iScore);
		remained_length -= decoded_length;
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iItemID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;

		return success;
	}

	void CCSResponseGetActionInfo::dump()
	{
		return;
	}
	
	
	/////////////////////////////////////////////////////////////////////////////////////////
	CCSRequestGetActionOnline::CCSRequestGetActionOnline()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		
	}

	CCSRequestGetActionOnline::~CCSRequestGetActionOnline()
	{

	}
	int32_t CCSRequestGetActionOnline::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

		
		return success;
	}

	int32_t CCSRequestGetActionOnline::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		return success;
	}

	void CCSRequestGetActionOnline::dump()
	{
		return;
	}
	
	
	CCSResponseGetActionOnline::CCSResponseGetActionOnline()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_today_value = 0 ;
		m_total_value = 0 ;
		m_hummer_count = 0;
		m_ranking = 0 ;
		
	}

	CCSResponseGetActionOnline::~CCSResponseGetActionOnline()
	{

	}


	int32_t CCSResponseGetActionOnline::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_today_value);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_total_value);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int16(&ptmp, m_hummer_count);
		iOutLength += coded_length;
        coded_length = CCodeEngine::encode_int32(&ptmp, m_ranking);
		iOutLength += coded_length;
		
		return success;
	}	

	int32_t CCSResponseGetActionOnline::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_today_value);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_total_value);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_hummer_count);
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_ranking);
		remained_length -= decoded_length;
		
		return success;
	}

	void CCSResponseGetActionOnline::dump()
	{
		return;
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////
	CCSRequestGetActionOnlineRank::CCSRequestGetActionOnlineRank()
	{
		
	}

	CCSRequestGetActionOnlineRank::~CCSRequestGetActionOnlineRank()
	{

	}
	int32_t CCSRequestGetActionOnlineRank::encode( char *pszOut, int32_t& iOutLength )
	{
		
		return success;
	}

	int32_t CCSRequestGetActionOnlineRank::decode( const char *pszIn, const int32_t iInLength )
	{

		return success;
	}

	void CCSRequestGetActionOnlineRank::dump()
	{
		return;
	}
	

	CCSResponseGetActionOnlineRank::CCSResponseGetActionOnlineRank()
	{
        m_shResultID = 0 ;
		m_iCountRank = 0 ;
		memset(m_szRank,0,sizeof(m_szRank));
	}

	CCSResponseGetActionOnlineRank::~CCSResponseGetActionOnlineRank()
	{

	}

	int32_t CCSResponseGetActionOnlineRank::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
        coded_length = CCodeEngine::encode_int16(&ptmp, m_iCountRank);
		iOutLength += coded_length;
		
		if (m_iCountRank < 0 || m_iCountRank > MAX_ONLINE_RANK)
		{
			return fail;
		}

		for(int32_t i=0;i<m_iCountRank; ++i)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_szRank[i].m_iUin);
			iOutLength += coded_length;
			coded_length = CCodeEngine::encode_string(&ptmp, m_szRank[i].m_szAccount, (const int16_t)sizeof(m_szRank[i].m_szAccount));
			iOutLength += coded_length;
			coded_length = CCodeEngine::encode_int32(&ptmp, m_szRank[i].m_iRank);
			iOutLength += coded_length;
		}

		return success;
	}	

	int32_t CCSResponseGetActionOnlineRank::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iCountRank);
		remained_length -= decoded_length;

		if (m_iCountRank < 0 || m_iCountRank > MAX_ONLINE_RANK)
		{
			return fail;
		}

		for(int32_t i=0;i<m_iCountRank; ++i)
		{
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szRank[i].m_iUin);
			remained_length -= decoded_length;
			decoded_length = CCodeEngine::decode_string(&ptmp, m_szRank[i].m_szAccount, (const int16_t)sizeof(m_szRank[i].m_szAccount));
		    remained_length -= decoded_length;
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_szRank[i].m_iRank);
			remained_length -= decoded_length;
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseGetActionOnlineRank::dump()
	{
		return;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	CCSRequestBalance::CCSRequestBalance()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_iFlag = 0;
		m_iClientIP = 0 ;
		
	}

	CCSRequestBalance::~CCSRequestBalance()
	{

	}
	int32_t CCSRequestBalance::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;


		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iFlag);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSRequestBalance::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iFlag);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		return success;
	}

	void CCSRequestBalance::dump()
	{
		return;
	}
	
	
	
	CCSResponseBalance::CCSResponseBalance()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_iMoney = 0 ;
		m_iBean = 0 ;
		m_iItemID = 0;
		m_shItemCount = 0 ;
		m_iCardID = 0 ;
		m_iChestID = 0 ;
		
	}

	CCSResponseBalance::~CCSResponseBalance()
	{

	}

	

	int32_t CCSResponseBalance::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iMoney);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iBean);
		iOutLength += coded_length;
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iItemID);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iCardID);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iChestID);
		iOutLength += coded_length;
		return success;
	}

	int32_t CCSResponseBalance::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iBean);
		remained_length -= decoded_length;
		
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iItemID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCardID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChestID);
		remained_length -= decoded_length;
		
		return success;
	}

	void CCSResponseBalance::dump()
	{
		return;
	}
	
	





	CCSRequestActionHit::CCSRequestActionHit()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_iHit = 0;
		m_iClientIP = 0 ;
		
	}

	CCSRequestActionHit::~CCSRequestActionHit()
	{

	}
	int32_t CCSRequestActionHit::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;


		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iHit);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSRequestActionHit::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iHit);
		remained_length -= decoded_length;
        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		return success;
	}

	void CCSRequestActionHit::dump()
	{
		return;
	}
	
	
	
	CCSResponseActionHit::CCSResponseActionHit()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_iMoney = 0 ;
		m_iBean = 0 ;
		m_iScore = 0 ;
		m_iItemID = 0;
		m_shItemCount = 0 ;

		
	}

	CCSResponseActionHit::~CCSResponseActionHit()
	{

	}

	

	int32_t CCSResponseActionHit::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iMoney);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iBean);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iScore);
		iOutLength += coded_length;
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iItemID);
		iOutLength += coded_length;
		
		 coded_length = CCodeEngine::encode_int16(&ptmp, m_shItemCount);
		iOutLength += coded_length;
		
		
		return success;
	}

	int32_t CCSResponseActionHit::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iBean);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iScore);
		remained_length -= decoded_length;
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iItemID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shItemCount);
		remained_length -= decoded_length;
        

		
		return success;
	}

	void CCSResponseActionHit::dump()
	{
		return;
	}
	
	
CCSRequestUpdateEquipment::CCSRequestUpdateEquipment()
{

	m_iUin = 0;
	m_nEquipmentUpdateCount = 0 ;
	memset(m_stEquipmentUpdate,0,sizeof(m_stEquipmentUpdate));

}

CCSRequestUpdateEquipment::~CCSRequestUpdateEquipment()
{


}

int32_t  CCSRequestUpdateEquipment::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nEquipmentUpdateCount);
	iOutLength += encode_length;

	if (m_nEquipmentUpdateCount < 0 || m_nEquipmentUpdateCount > MAX_EQUIPMENT_COUNT)
	{
		return fail;
	}
	
	for(int32_t i=0; i< m_nEquipmentUpdateCount; ++i)
    {
		    
        encode_length = CCodeEngine::encode_int16(&ptmp, 5 );
        iOutLength += encode_length;
					   
        encode_length = CCodeEngine::encode_int32(&ptmp, m_stEquipmentUpdate[i].m_iItemID);
        iOutLength += encode_length;

        encode_length = CCodeEngine::encode_int8(&ptmp, m_stEquipmentUpdate[i].m_cPosition);
        iOutLength += encode_length;
    }
		

	return success;
}

int32_t  CCSRequestUpdateEquipment::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nEquipmentUpdateCount);
	remain_length -= decode_length;

	if (m_nEquipmentUpdateCount < 0 || m_nEquipmentUpdateCount > MAX_EQUIPMENT_COUNT)
	{
		return fail;
	}

    for(int32_t i = 0; i<m_nEquipmentUpdateCount; ++i)
    {
        uint16_t tmp = 0 ;
        
        decode_length = CCodeEngine::decode_int16(&ptmp, &tmp );
        remain_length -= decode_length;
		if ( tmp != 5 )
		{
		    return fail ;
		}
			
        decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stEquipmentUpdate[i].m_iItemID);
        remain_length -= decode_length;

        decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stEquipmentUpdate[i].m_cPosition);
        remain_length -= decode_length;
    }
		
		
		
	
	if(remain_length<0)
	{
		return fail;
	}
	return success;
	
}

void CCSRequestUpdateEquipment::dump()
{

}
	
	
	
CCSResponseUpdateEquipment::CCSResponseUpdateEquipment()
{

	m_iUin = 0;
	m_nResultID = 0 ;
	memset(m_aryEquipmentInfo,0,sizeof(m_aryEquipmentInfo));
	
}
CCSResponseUpdateEquipment::~CCSResponseUpdateEquipment()
{


}

int32_t  CCSResponseUpdateEquipment::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	for(int32_t i=0;i<MAX_EQUIPMENT_COUNT;++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, m_aryEquipmentInfo[i]);
		iOutLength += encode_length;
	}

	return success;
}

int32_t  CCSResponseUpdateEquipment::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	for(int32_t i=0;i<MAX_EQUIPMENT_COUNT;++i)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryEquipmentInfo[i]);
		remain_length -= decode_length;
	}


	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CCSResponseUpdateEquipment::dump()
{
	return ;
}


	
	CCSRequestConsign::CCSRequestConsign()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_iConsignType = 0;
		m_iConsignID = 0;
		m_iConsignCount = 0;
		m_iClientIP = 0 ;
		
	}

	CCSRequestConsign::~CCSRequestConsign()
	{

	}
	int32_t CCSRequestConsign::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iConsignType);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iConsignID);
		iOutLength += coded_length;
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iConsignCount);
		iOutLength += coded_length;        
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iClientIP);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSRequestConsign::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;


		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iConsignType);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iConsignID);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iConsignCount);
		remained_length -= decoded_length;		        
        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iClientIP);
		remained_length -= decoded_length;
		return success;
	}

	void CCSRequestConsign::dump()
	{
		return;
	}
	
	
	
	CCSResponseConsign::CCSResponseConsign()
	{

		m_iUin = 0;
		m_szAccount[0] = '\0';
        m_shResultID = 0 ;
		m_iConsignID = 0;
		m_iConsignCount = 0;

		
	}

	CCSResponseConsign::~CCSResponseConsign()
	{

	}

	

	int32_t CCSResponseConsign::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;
		
		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_iConsignID);
		iOutLength += coded_length;
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iConsignCount);
		iOutLength += coded_length;
		
		
		return success;
	}

	int32_t CCSResponseConsign::decode( const char *pszIn, const int32_t iInLength )
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iConsignID);
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iConsignCount);
		remained_length -= decoded_length;
	
		return success;
	}

	void CCSResponseConsign::dump()
	{
		return;
	}
	
				
}}//namespace KingNet { namespace Server {
