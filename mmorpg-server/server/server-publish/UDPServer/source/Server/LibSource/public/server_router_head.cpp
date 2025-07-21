#include "public/server_router_head.h"


namespace KingNet { namespace Server {
CRouterHead::CRouterHead()
{
	m_iTotalSize        = 0;
	m_iSourceEntityID   = 0;
	m_cSourceEntityType = 0;
	m_cDestEntityType   = 0;
	m_cTransferType     = trans_type_null;
	m_byOpFlag          = 0;				//
	m_nOptLength        = 0;			    //m_abyOpt字段实际大小
	memset(m_abyOpt,0,max_option_size);
}

CRouterHead::~CRouterHead()
{
	//do nothing
}

int32_t CRouterHead::size()
{
	int32_t nLen = router_head_basic_size;

	switch(m_cTransferType)
	{
	case trans_type_by_key:
		nLen += sizeof(int32_t);
		break;

	case trans_type_p2p:
		nLen += sizeof(int32_t);
		break;

	case trans_type_p2g:
		nLen += sizeof(int16_t);
		nLen += (m_stTransferInfo.stSvrs.m_nDestCount * sizeof(int32_t));
		break;

	case trans_type_broadcast:
		break;

	default:
		return 0;
	}; 

	nLen += m_nOptLength;

	return nLen;
}

int32_t  CRouterHead::encode(char* pszCode, int32_t& iCodeSize)
{
	if (NULL == pszCode)
	{
		return fail;
	}

	char* ptmp = pszCode;
	int32_t coded_length = 0;

	iCodeSize = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iTotalSize);
	iCodeSize += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,(uint32_t)m_iSourceEntityID);
	iCodeSize += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cSourceEntityType);
	iCodeSize += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cDestEntityType);
	iCodeSize += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cTransferType);
	iCodeSize += coded_length;

	//m_stTransferInfo
	switch(m_cTransferType)
	{
	case trans_type_by_key:
        {
		    coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stTransferInfo.m_iUin);
		    iCodeSize += coded_length;
		    
            break;
        }

	case trans_type_p2p:
        {
		    coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stTransferInfo.m_iDestEntityID);
		    iCodeSize += coded_length;
		    
            break;
        }

	case trans_type_p2g:
        {
		    coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stTransferInfo.stSvrs.m_nDestCount);
		    iCodeSize += coded_length;
		    for(int i=0;i<m_stTransferInfo.stSvrs.m_nDestCount;++i)
		    {
			    coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stTransferInfo.stSvrs.m_aiDestsID[i]);
			    iCodeSize += coded_length;
		    }

		    break;
        }

	case trans_type_broadcast:
		break;

	default:
		return fail;
		break;
	};
	

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_byOpFlag);
	iCodeSize += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nOptLength);
	iCodeSize += coded_length;

   if(m_nOptLength>0)
   {
	  memcpy(ptmp,m_abyOpt,m_nOptLength);
	  ptmp += m_nOptLength;
	  iCodeSize += m_nOptLength;
   }

	return success;
}


int32_t CRouterHead::decode(const char *pszCode, const int32_t iCodeSize,int16_t* pHeadLen)
{
	if (NULL == pszCode || iCodeSize <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszCode);
	int32_t remained_length = iCodeSize;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iTotalSize));
	remained_length -= decoded_length;

	//检查包是否完整
	if(m_iTotalSize>iCodeSize)
    {
		return fail;
    }

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iSourceEntityID));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_cSourceEntityType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_cDestEntityType));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_cTransferType));
	remained_length -= decoded_length;

	//m_stTransferInfo
	switch(m_cTransferType)
	{
	case trans_type_by_key:
        {
		    decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stTransferInfo.m_iUin));
		    remained_length -= decoded_length;
    		
            break;
        }

	case trans_type_p2p:
        {
		    decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stTransferInfo.m_iDestEntityID));
		    remained_length -= decoded_length;
    		
            break;
        }

	case trans_type_p2g:
        {
		    decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stTransferInfo.stSvrs.m_nDestCount));
		    remained_length -= decoded_length;
			//add begin,by luozhen,2009-05-15
			if(m_stTransferInfo.stSvrs.m_nDestCount > max_dest_count)
			{
				return fail;
			}
			//add end
		    for(int i=0;i<m_stTransferInfo.stSvrs.m_nDestCount;++i)
		    {
			    decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stTransferInfo.stSvrs.m_aiDestsID[i]));
			    remained_length -= decoded_length;
		    }

		    break;
        }

	case trans_type_broadcast:
		break;

	default:
		return fail;
		break;
	};

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_byOpFlag));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_nOptLength));
	remained_length -= decoded_length;

	if(m_nOptLength > max_option_size)
	{
		return fail;
	}

	if(m_nOptLength>0)
	{
		memcpy(m_abyOpt,ptmp,m_nOptLength);
		remained_length-=m_nOptLength;
	}

	//计算出实际头部长度
	if(pHeadLen!=NULL)
	{
		*pHeadLen = iCodeSize - remained_length;
	}	 

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CRouterHead::dump()
{
    //do nothing
	return ;
}

}}//namespace KingNet { namespace Server {
