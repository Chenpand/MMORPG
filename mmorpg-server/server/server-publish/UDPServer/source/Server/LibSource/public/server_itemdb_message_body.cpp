#include "public/server_itemdb_message_body.h"
#include "Common/common_codeengine.h"

namespace KingNet { namespace Server {  
/*

CNotifyPlayerStatusToItemDB::CNotifyPlayerStatusToItemDB()
{
	m_iUin = 0;
	m_cState = player_logout;
}

CNotifyPlayerStatusToItemDB::~CNotifyPlayerStatusToItemDB()
{
    //do nothing
}

void CNotifyPlayerStatusToItemDB::dump()
{
    //do nothing
}

*/

 CNotifyItemDBTransferMessage::CNotifyItemDBTransferMessage()
{
	m_nSourceUin = 0;
	m_nDestUin = 0;
	m_nDataSize = 0;
	memset(&m_szTransparentData[0],0,sizeof(m_szTransparentData));
}

CNotifyItemDBTransferMessage::~CNotifyItemDBTransferMessage()
{
 	
}
 
int32_t CNotifyItemDBTransferMessage::encode(char *pszOut, int32_t& iOutLength)
{
		if ((NULL == pszOut) || (m_nDataSize > max_transparent_data_size))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nSourceUin);
		iOutLength += coded_length;
	 
		coded_length = CCodeEngine::encode_int32(&ptmp, m_nDestUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataSize);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		iOutLength += coded_length;

		return success;
}
 	
 int32_t  CNotifyItemDBTransferMessage::decode(const char *pszIn, const int32_t iInLength)
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_nSourceUin);
	remained_length -= decoded_length;


	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_nDestUin);
	remained_length -= decoded_length;


	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
	remained_length -= decoded_length;

	if (m_nDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CNotifyItemDBTransferMessage::dump()
{
	return ;
}
 /*
int32_t CNotifyPlayerStatusToItemDB::encode( char *pszOut, int32_t& iOutLength )
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

	coded_length = CCodeEngine::encode_int8(&ptmp, m_cState);
	iOutLength += coded_length;

	return success;

}

int32_t CNotifyPlayerStatusToItemDB::decode( const char *pszIn, const int32_t iInLength )
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

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cState);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}
*/

int32_t encode_item_info(char** pszOut, stItemInfo& item_info)
{
    if (*pszOut == NULL || pszOut == NULL)
    {
        return 0;
    }


    int16_t item_size = 0;
    char* ptmp = *pszOut;

    int32_t iOutLength = 0;
    int32_t coded_length = 0;

    *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_nItemID);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int16(pszOut, item_info.m_nItemType);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_iCount);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int16(pszOut, item_info.m_nGameID);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_nExpiredTime);
    item_size += coded_length;
    
	coded_length = CCodeEngine::encode_int8(pszOut, item_info.m_cUpdateMode);
	item_size += coded_length;

    *pszOut -= (item_size + sizeof(int16_t));
    coded_length = CCodeEngine::encode_int16(pszOut, item_size);

    iOutLength = (item_size + coded_length);

    *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

    return iOutLength;
}

int32_t decode_item_info(char** pszIn, stItemInfo& item_info)
{
    if (*pszIn == NULL || pszIn == NULL)
    {
        return 0;
    }
    
    int16_t item_size = 0;
    int32_t iInLength = 0;
    int32_t decode_length = 0;

    decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_size);
    iInLength += decode_length;

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_nItemID);
    iInLength += decode_length;
    item_size -= decode_length;

    decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_info.m_nItemType);
    iInLength += decode_length;
    item_size -= decode_length;

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_iCount);
    iInLength += decode_length;
    item_size -= decode_length;
    
    decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_info.m_nGameID);
    iInLength += decode_length;
    item_size -= decode_length;


    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_nExpiredTime);
    iInLength += decode_length;
    item_size -= decode_length;
    

	decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&item_info.m_cUpdateMode);
	iInLength += decode_length;
	item_size -= decode_length;


    if (item_size < 0)
    {
        return 0;
    }
    else //对于不能解码的字节流，跳过
    {
        (*pszIn)  += item_size;
        iInLength += item_size;
    }

    return iInLength;
}

int32_t encode_goods_info(char** pszOut, stGoodsInfo& good_info)
{
    if (*pszOut == NULL || pszOut == NULL)
    {
        return 0;
    }


    int16_t item_size = 0;
    char* ptmp = *pszOut;

    int32_t iOutLength = 0;
    int32_t coded_length = 0;

    *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

    coded_length = CCodeEngine::encode_int32(pszOut, good_info.m_nGoodsID);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int32(pszOut, good_info.m_nCount);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int32(pszOut, good_info.m_nPrice);
    item_size += coded_length;
    
    *pszOut -= (item_size + sizeof(int16_t));
    coded_length = CCodeEngine::encode_int16(pszOut, item_size);

    iOutLength = (item_size + coded_length);

    *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

    return iOutLength;
}


int32_t decode_goods_info(char** pszIn, stGoodsInfo& good_info)
{
    if (*pszIn == NULL || pszIn == NULL)
    {
        return 0;
    }
    
    int16_t item_size = 0;
    int32_t iInLength = 0;
    int32_t decode_length = 0;

    decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_size);
    iInLength += decode_length;

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&good_info.m_nGoodsID);
    iInLength += decode_length;
    item_size -= decode_length;

     

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&good_info.m_nCount);
    iInLength += decode_length;
    item_size -= decode_length;
    


    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&good_info.m_nPrice);
    iInLength += decode_length;
    item_size -= decode_length;
    

    if (item_size < 0)
    {
        return 0;
    }
    else //对于不能解码的字节流，跳过
    {
        (*pszIn)  += item_size;
        iInLength += item_size;
    }

    return iInLength;
}

int32_t encode_item_info2(char** pszOut, stItemInfo2& item_info)
{
    if (*pszOut == NULL || pszOut == NULL)
    {
        return 0;
    }


    int16_t item_size = 0;
    char* ptmp = *pszOut;

    int32_t iOutLength = 0;
    int32_t coded_length = 0;

    *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_nItemID);
    item_size += coded_length;

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_iCount);
    item_size += coded_length;

  

    coded_length = CCodeEngine::encode_int32(pszOut, item_info.m_nExpiredTime);
    item_size += coded_length;
    
    *pszOut -= (item_size + sizeof(int16_t));
    coded_length = CCodeEngine::encode_int16(pszOut, item_size);

    iOutLength = (item_size + coded_length);

    *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

    return iOutLength;
}

int32_t decode_item_info2(char** pszIn, stItemInfo2& item_info)
{
    if (*pszIn == NULL || pszIn == NULL)
    {
        return 0;
    }
    
    int16_t item_size = 0;
    int32_t iInLength = 0;
    int32_t decode_length = 0;

    decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_size);
    iInLength += decode_length;

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_nItemID);
    iInLength += decode_length;
    item_size -= decode_length;

     

    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_iCount);
    iInLength += decode_length;
    item_size -= decode_length;
    


    decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&item_info.m_nExpiredTime);
    iInLength += decode_length;
    item_size -= decode_length;
    

    if (item_size < 0)
    {
        return 0;
    }
    else //对于不能解码的字节流，跳过
    {
        (*pszIn)  += item_size;
        iInLength += item_size;
    }

    return iInLength;
}

//////////////////////////////////////////////////////////////////////////

int32_t encode_EquipmentUpdate(char** pszOut, stEquipmentUpdate& equipment)
{
	if (*pszOut == NULL || pszOut == NULL)
	{
		return 0;
	}


	int16_t item_size = 0;
	char* ptmp = *pszOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

	coded_length = CCodeEngine::encode_int8(pszOut, equipment.m_nEquipmentUpdateMod);
	item_size += coded_length;

 
	coded_length = CCodeEngine::encode_int32(pszOut, equipment.m_nEquipmentPosi);
	item_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pszOut, equipment.m_nItemId);
	item_size += coded_length;

	*pszOut -= (item_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pszOut, item_size);

	iOutLength = (item_size + coded_length);

	*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置
	return iOutLength;

}
int32_t decode_EquipmentUpdate(char** pszIn, stEquipmentUpdate& equipment)
{
	if (*pszIn == NULL || pszIn == NULL)
	{
		return 0;
	}

	int16_t item_size = 0;
	int32_t iInLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&item_size);
	iInLength += decode_length;


	decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&equipment.m_nEquipmentUpdateMod);
	iInLength += decode_length;
	item_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&equipment.m_nEquipmentPosi);
	iInLength += decode_length;
	item_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&equipment.m_nItemId);
	iInLength += decode_length;
	item_size -= decode_length;
 

	if (item_size < 0)
	{
		return 0;
	}
	else //对于不能解码的字节流，跳过
	{
		(*pszIn)  += item_size;
		iInLength += item_size;
	}
	return iInLength;
}
//////////////////////////////////////////////////////////////////////////

CResquestUpdateItemInfo::CResquestUpdateItemInfo()
{
    m_nRealUIN = 0;         
    m_szRealAccount[0] = '\0';  
    m_nActorUIN = 0;        
    m_szActorAccount[0] = '\0';
    m_nServiceTag = 0;      
    m_nSourceTag = -1;
    m_nIP = -1;              
    m_nItemCount = 0;       
    m_nGoodsCount = 0;
    m_nTransparentDataSize = 0;
    m_szOperateDescription[0] = '\0';
    m_szTransTag[0]='\0';
    memset(&m_stGoodsInfo[0],0,sizeof(m_stGoodsInfo));

	m_nNofifyTransparentDataSize = 0;

}



CResquestUpdateItemInfo::~CResquestUpdateItemInfo()
{
    //do nothing
}

int32_t CResquestUpdateItemInfo::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
    {
        return fail;
    }

	if (((uint16_t)m_nItemCount > MAX_ITEM_UPDATE_COUNT) || ( (uint16_t)m_nGoodsCount > MAX_GOODS_UPDATE_COUNT))
	{
		return fail;
	}

	if ((m_nTransparentDataSize > max_transparent_data_size) || (m_nNofifyTransparentDataSize > max_sub_message_size))
	{
		return fail;
	}

    int16_t encode_length = 0;
    char* ptmp = pszOut;

    iOutLength = 0;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nRealUIN);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szRealAccount, (const int16_t)sizeof(m_szRealAccount));
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nActorUIN);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szActorAccount, (const int16_t)sizeof(m_szActorAccount));
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nServiceTag);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nSourceTag);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nIP);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int16(&ptmp, m_nItemCount);
    iOutLength += encode_length;

    for (int16_t i=0; i<m_nItemCount; ++i)
    {
        encode_length = encode_item_info(&ptmp, m_stItemInfos[i]);
        iOutLength   += encode_length;
    }

    encode_length = CCodeEngine::encode_int16(&ptmp, m_nGoodsCount);
	iOutLength  += encode_length;

    for (int16_t i=0; i<m_nGoodsCount; ++i)
    {
        encode_length = encode_goods_info(&ptmp, m_stGoodsInfo[i]);
        iOutLength   += encode_length;
    }

    encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
    iOutLength += encode_length;

    if (m_nTransparentDataSize > 0)
    {
        encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szTransparentData, (const int32_t)m_nTransparentDataSize);
        iOutLength += encode_length;
    }

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szOperateDescription, (const int16_t)sizeof(m_szOperateDescription));
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szTransTag, (const int16_t)sizeof(m_szTransTag));
    iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	if (m_nNofifyTransparentDataSize > 0)
	{
		encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szNofifyTransparentData, (const int32_t)m_nNofifyTransparentDataSize);
		iOutLength += encode_length;
	}
     return success;
}

int32_t CResquestUpdateItemInfo::decode(const char *pszIn, const int32_t iInLength)
{
    if(NULL == pszIn || iInLength <= 0)
    {
        return fail;
    }

    int16_t remain_length = iInLength;
    int16_t decode_length = 0;

    char* ptmp = const_cast<char*>(pszIn);

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nRealUIN);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szRealAccount, (const int16_t) sizeof(m_szRealAccount));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nActorUIN);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szActorAccount, (const int16_t) sizeof(m_szActorAccount));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nServiceTag);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nSourceTag);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nIP);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nItemCount);
    remain_length -= decode_length;

    if (MAX_ITEM_UPDATE_COUNT <  (uint16_t)m_nItemCount)
    {
        m_nItemCount = MAX_ITEM_UPDATE_COUNT; //其实这样处理若编码进去 的m_stItemInfos元素个数真的超过MAX_ITEM_UPDATE_COUNT,下面的解码处理是不正确的
											  //正确的处理应该是跳过超过MAX_ITEM_UPDATE_COUNT的数目的元素，再进行后面的解码
	}

    for (int16_t i=0; i<m_nItemCount; ++i)
    {
        decode_length = decode_item_info(&ptmp, m_stItemInfos[i]);
        remain_length -= decode_length;
    }

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGoodsCount);
	remain_length -= decode_length;

	if (m_nGoodsCount > MAX_GOODS_UPDATE_COUNT)
	{
		return fail;
	}
   
     for (int16_t i=0; i<m_nGoodsCount; ++i)
    {
        decode_length = decode_goods_info(&ptmp, m_stGoodsInfo[i]);
        remain_length -= decode_length;
    }
   
    decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
    remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

    if (m_nTransparentDataSize > 0)
    {
        decode_length = CCodeEngine::decode_memory(&ptmp, &m_szTransparentData[0], m_nTransparentDataSize);
        remain_length -= decode_length;
    }

    decode_length = CCodeEngine::decode_string(&ptmp, m_szOperateDescription, (const int16_t) sizeof(m_szOperateDescription));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, (const int16_t) sizeof(m_szTransTag));
    remain_length -= decode_length;
	

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if (m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, &m_szNofifyTransparentData[0], m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}
 
    if(remain_length < 0)
    {
        return fail;
    }

    return success;
}

void CResquestUpdateItemInfo::dump()
{
    //do nothing
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CResponseUpdateItemInfo::CResponseUpdateItemInfo()
{
    m_nResultID = result_id_fail;
    m_nRealUIN	= 0;         
    m_szRealAccount[0] = '\0';  
    m_nActorUIN = 0;        
    m_szActorAccount[0] = '\0';
    m_nItemCount = 0;       
    m_nTransparentDataSize = 0;
    m_szReasonMsg[0] = '\0';
	m_szTransTag[0] = '\0';
}

CResponseUpdateItemInfo::~CResponseUpdateItemInfo()
{
    //do nothing
}

int32_t CResponseUpdateItemInfo::encode(char *pszOut, int32_t& iOutLength)
{
    if((NULL == pszOut) || (m_nTransparentDataSize > max_transparent_data_size))
    {
        return fail;
    }

    int16_t encode_length = 0;
    char* ptmp = pszOut;

    iOutLength = 0;

    encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nRealUIN);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szRealAccount, (const int16_t)sizeof(m_szRealAccount));
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nActorUIN);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szActorAccount, (const int16_t)sizeof(m_szActorAccount));
    iOutLength += encode_length;


    if (MAX_ITEM_UPDATE_COUNT <  (uint16_t)m_nItemCount)
    {
        m_nItemCount = MAX_ITEM_UPDATE_COUNT;
    }
    encode_length = CCodeEngine::encode_int16(&ptmp, m_nItemCount);
    iOutLength += encode_length;


    for (int8_t i=0; i<m_nItemCount; ++i)
    {
        encode_length = encode_item_info(&ptmp, m_stItemInfos[i]);
        iOutLength   += encode_length;
    }

    encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szTransparentData, (const int32_t)m_nTransparentDataSize);
    iOutLength += encode_length;

    if (result_id_success != m_nResultID)
    {
        encode_length = CCodeEngine::encode_string(&ptmp, m_szReasonMsg, (const int16_t)sizeof(m_szReasonMsg));
        iOutLength += encode_length;
    }

	encode_length = CCodeEngine::encode_string(&ptmp,m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	iOutLength += encode_length;
    return success;
}

int32_t CResponseUpdateItemInfo::decode(const char *pszIn, const int32_t iInLength)
{
    if(NULL == pszIn || iInLength <= 0)
    {
        return fail;
    }

    int16_t remain_length = iInLength;
    int16_t decode_length = 0;

    char* ptmp = const_cast<char*>(pszIn);

    decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nRealUIN);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szRealAccount, (const int16_t) sizeof(m_szRealAccount));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nActorUIN);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szActorAccount, (const int16_t) sizeof(m_szActorAccount));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nItemCount);
    remain_length -= decode_length;

    if (MAX_ITEM_UPDATE_COUNT < (uint16_t)m_nItemCount)
    {
        m_nItemCount = MAX_ITEM_UPDATE_COUNT;
    }

    for (int16_t i=0; i<m_nItemCount; ++i)
    {
        decode_length = decode_item_info(&ptmp, m_stItemInfos[i]);
        remain_length -= decode_length;
    }

    decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
    remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

    if (m_nTransparentDataSize > 0)
    {
        decode_length = CCodeEngine::decode_memory(&ptmp, &m_szTransparentData[0], m_nTransparentDataSize);
        remain_length -= decode_length;
    }

    if (result_id_success != m_nResultID)
    {
        decode_length = CCodeEngine::decode_string(&ptmp, m_szReasonMsg, (const int16_t)sizeof(m_szReasonMsg));
        remain_length -= decode_length;
    }

	decode_length = CCodeEngine::decode_string(&ptmp,m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	remain_length -= decode_length;

    if(remain_length < 0)
    {
        return fail;
    }

    return success;
}

void CResponseUpdateItemInfo::dump()
{
    //do nothing
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CNotifyUpdateItemInfo::CNotifyUpdateItemInfo()
{
    m_nDstUIN = 0;
    m_szDstAccount[0] = '\0';
    m_nItemCount = 0;

	m_nNofifyTransparentDataSize = 0;
}

CNotifyUpdateItemInfo::~CNotifyUpdateItemInfo()
{
    //do nothing
}

int32_t CNotifyUpdateItemInfo::encode(char *pszOut, int32_t& iOutLength)
{
    if((NULL == pszOut) || (m_nNofifyTransparentDataSize > max_sub_message_size))
    {
        return fail;
    }

    int16_t encode_length = 0;
    char* ptmp = pszOut;

    iOutLength = 0;

    encode_length = CCodeEngine::encode_int32(&ptmp, m_nDstUIN);
    iOutLength += encode_length;

    encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
    iOutLength += encode_length;

    if (MAX_ITEM_UPDATE_COUNT < (uint16_t)m_nItemCount)
    {
        return fail;
    }
    encode_length = CCodeEngine::encode_int16(&ptmp, m_nItemCount);
    iOutLength += encode_length;


    for (int16_t i=0; i<m_nItemCount; ++i)
    {
        encode_length = encode_item_info(&ptmp, m_stItemInfos[i]);
        iOutLength   += encode_length;
    }

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t CNotifyUpdateItemInfo::decode(const char *pszIn, const int32_t iInLength)
{
    if(NULL == pszIn || iInLength <= 0)
    {
        return fail;
    }

    int16_t remain_length = iInLength;
    int16_t decode_length = 0;

    char* ptmp = const_cast<char*>(pszIn);

    decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nDstUIN);
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t) sizeof(m_szDstAccount));
    remain_length -= decode_length;

    decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nItemCount);
    remain_length -= decode_length;

    if (MAX_ITEM_UPDATE_COUNT <  (uint16_t)m_nItemCount)
    {
        m_nItemCount = MAX_ITEM_UPDATE_COUNT;
    }

    for (int16_t i=0; i<m_nItemCount; ++i)
    {
        decode_length = decode_item_info(&ptmp, m_stItemInfos[i]);
        remain_length -= decode_length;
    }


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}

    if(remain_length < 0)
    {
        return fail;
    }

    return success;
}

void CNotifyUpdateItemInfo::dump()
{
    //do nothing
}

////////////////////////////////
CReqeustGetPlayerItemData::CReqeustGetPlayerItemData()
{
	m_nUIN = 0;
	m_nDataSize = 0;
}
CReqeustGetPlayerItemData& CReqeustGetPlayerItemData::operator=(const CReqeustGetPlayerItemData& obj)
{
    if ( this == &obj ) return *this ;
    
    m_nUIN = obj.m_nUIN ;
	m_nDataSize = obj.m_nDataSize;
    return *this ;
}

CReqeustGetPlayerItemData::~CReqeustGetPlayerItemData()
{
	
	
}
int32_t CReqeustGetPlayerItemData::encode(char *pszOut, int32_t& iOutLength)
{
    if(NULL == pszOut)
   	 {
        	return fail;
   	 }

    int16_t encode_length = 0;
    char* ptmp = pszOut;
   	iOutLength = 0;

   	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
   	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData,m_nDataSize);
	iOutLength += encode_length;
	return success;
}

int32_t  CReqeustGetPlayerItemData::decode(const char *pszIn, const int32_t iInLength)
{
	 if(NULL == pszIn || iInLength <= 0)
    {
       	 	return fail;
   	 }

   	 int16_t remain_length = iInLength;
   	 int16_t decode_length = 0;

     char* ptmp = const_cast<char*>(pszIn);

   	 decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nUIN);
   	 remain_length -= decode_length;
    
	 decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nDataSize);
	 remain_length -= decode_length;

	 if(m_nDataSize>max_transparent_data_size)
	 {
		return fail;
	 }

	 decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData,m_nDataSize);
	 remain_length -= decode_length;

  	if(remain_length < 0)
  	{
   	     return fail;
  	}
	return success;	
}

void CReqeustGetPlayerItemData::dump()
{
	return ;	
}


CResponseGetPlayerItemData::CResponseGetPlayerItemData()
{
	m_nUIN = 0;
	m_nResultID = 0;

	m_nItemCount = 0;      
	memset(&m_stItemInfos[0],0,sizeof(m_stItemInfos));
	m_nCharmItemCount = 0;       
	memset(&m_stCharmItemInfos[0],0,sizeof(m_stCharmItemInfos));
	m_nDataSize = 0;
}


CResponseGetPlayerItemData::~CResponseGetPlayerItemData()
{
	
	
}
int32_t CResponseGetPlayerItemData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;



	if( m_nResultID == result_id_success)
	{
		if (MAX_ITEM_COUNT < m_nItemCount)
		{
			m_nItemCount = MAX_ITEM_COUNT;
		}
		encode_length = CCodeEngine::encode_int16(&ptmp, m_nItemCount);
		iOutLength += encode_length;


		for (int16_t i=0; i<m_nItemCount; ++i)
		{
			encode_length = encode_item_info2(&ptmp, m_stItemInfos[i]);
			iOutLength   += encode_length;
		}



		if (MAX_ITEM_COUNT < m_nCharmItemCount)
		{
			m_nCharmItemCount = MAX_ITEM_COUNT;
		}
		encode_length = CCodeEngine::encode_int16(&ptmp, m_nCharmItemCount);
		iOutLength += encode_length;


		for (int16_t i=0; i<m_nCharmItemCount; ++i)
		{
			encode_length = encode_item_info2(&ptmp, m_stCharmItemInfos[i]);
			iOutLength   += encode_length;
		}

		encode_length = CCodeEngine::encode_int32(&ptmp, m_nDataSize);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData,m_nDataSize);
		iOutLength += encode_length;

	}

	return success;
}

int32_t  CResponseGetPlayerItemData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nUIN);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length += decode_length;

	if( result_id_success ==  m_nResultID)
	{
		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nItemCount);
		remain_length -= decode_length;
		if (MAX_ITEM_COUNT < m_nItemCount)
		{
			m_nItemCount = MAX_ITEM_COUNT;
		}

		for (int16_t i=0; i<m_nItemCount; ++i)
		{
			decode_length = decode_item_info2(&ptmp, m_stItemInfos[i]);
			remain_length -= decode_length;
		}	
		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nCharmItemCount);
		remain_length -= decode_length;

		if (MAX_ITEM_COUNT < m_nCharmItemCount)
		{
			m_nCharmItemCount = MAX_ITEM_COUNT;
		}

		for (int16_t i=0; i<m_nCharmItemCount; ++i)
		{
			decode_length = decode_item_info2(&ptmp, m_stCharmItemInfos[i]);
			remain_length -= decode_length;
		}	

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nDataSize);
		remain_length -= decode_length;

		if(m_nDataSize>max_transparent_data_size)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData,m_nDataSize);
		remain_length -= decode_length;
	}

	if(remain_length < 0)
	{
		return fail;
	}
	return success;	
}

void CResponseGetPlayerItemData::dump()
{
	return ;	
}

////////////////////////////////////////
CRequestDeleteItem::CRequestDeleteItem()
{
	m_nItemCount = 0;
	memset(m_aryItemID,0,sizeof(m_aryItemID));
	trans_tag[0] = '\0';
}

CRequestDeleteItem::~CRequestDeleteItem()
{

}

int32_t  CRequestDeleteItem::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nItemCount);
	iOutLength += encode_length;

	for(int32_t x=0;x<m_nItemCount;++x)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, m_aryItemID[x]);
		iOutLength += encode_length;
	}

	encode_length = CCodeEngine::encode_string(&ptmp, trans_tag,max_game_tag_length);
	iOutLength += encode_length;
	return success;
}

int32_t  CRequestDeleteItem::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nItemCount);
	remain_length -= decode_length;

	if(m_nItemCount >MAX_ITEM_COUNT)
	{
		m_nItemCount = MAX_ITEM_COUNT;
	}

	for(int32_t x=0;x<m_nItemCount;++x)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryItemID[x]);
		remain_length += decode_length;
	}

	decode_length = CCodeEngine::decode_string(&ptmp, trans_tag,max_game_tag_length);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}

	return success;
}


void CRequestDeleteItem::dump()
{


}


CResponseDeleteItem::CResponseDeleteItem()
{
	m_nResultID = result_id_success;
	trans_tag[0] ='\0';
}

CResponseDeleteItem::~CResponseDeleteItem()
{


}

int32_t  CResponseDeleteItem::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, trans_tag,max_game_tag_length);
	iOutLength += encode_length;
	return success;
}

int32_t  CResponseDeleteItem::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, trans_tag,max_game_tag_length);
	remain_length -= decode_length;

	return success;
}


void CResponseDeleteItem::dump()
{


}
/////////////
CRequestCleanExpireItem::CRequestCleanExpireItem()
{
		trans_tag[0] = '\0';
}
CRequestCleanExpireItem::~CRequestCleanExpireItem()
{

}
int32_t  CRequestCleanExpireItem::encode(char *pszOut, int32_t& iOutLength)
{

	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;
	encode_length = CCodeEngine::encode_string(&ptmp, trans_tag,max_game_tag_length);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestCleanExpireItem::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_string(&ptmp, trans_tag,max_game_tag_length);
	remain_length -= decode_length;

	if(remain_length<0)
	{

		return fail;
	}
	return success;

}
void CRequestCleanExpireItem::dump()
{


}


CResponseCleanExpireItem::CResponseCleanExpireItem()
{
	trans_tag[0] = '\0';
	m_nResultID = result_id_success;
}

CResponseCleanExpireItem::~CResponseCleanExpireItem()
{


}

int32_t  CResponseCleanExpireItem::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, trans_tag,max_game_tag_length);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponseCleanExpireItem::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, trans_tag,max_game_tag_length);
	remain_length -= decode_length;

	if(remain_length<0)
	{

		return fail;
	}
	return success;
}

void CResponseCleanExpireItem::dump()
{


}


// add by luozhen,2009-06-11...............
CRequestQueryPlayerItemData::CRequestQueryPlayerItemData()
{
	m_nUIN = 0;
	m_nSuperTypeCount = 0 ;
	memset(m_nSuperType,0,sizeof(m_nSuperType));
	m_bNeedEquipmentInfo = 0;
	return ;
}

CRequestQueryPlayerItemData::~CRequestQueryPlayerItemData()
{
	return ;
}

int32_t  CRequestQueryPlayerItemData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;

	if (MAX_QUERY_COUNT < m_nSuperTypeCount || 0 > m_nSuperTypeCount)
	{
		return fail;
	}

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nSuperTypeCount);
	iOutLength += encode_length;

	for(int32_t i=0;i<m_nSuperTypeCount; ++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, m_nSuperType[i]);
		iOutLength += encode_length;
	}

	encode_length = CCodeEngine::encode_int8(&ptmp, m_bNeedEquipmentInfo);
	iOutLength += encode_length;

	return success;
	 
}

int32_t  CRequestQueryPlayerItemData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUIN);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nSuperTypeCount);
	remain_length -= decode_length;

	if (m_nSuperTypeCount > MAX_QUERY_COUNT || m_nSuperTypeCount < 0)
	{
		return fail;
	}

	if(m_nSuperTypeCount > MAX_QUERY_COUNT)
	{
		m_nSuperTypeCount = MAX_QUERY_COUNT;
	}

	for(int32_t i=0; i<m_nSuperTypeCount; ++i)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nSuperType[i]);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*) &m_bNeedEquipmentInfo);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestQueryPlayerItemData::dump()
{

}


CResponseQueryPlayerItemData::CResponseQueryPlayerItemData()
{
	m_nUIN = 0;
	m_nResultID = result_id_success;
 
	m_nSuperTypeCount =0 ;
	memset(m_stSuperTypeInfo,0,sizeof(m_stSuperTypeInfo));

	m_cHaveEquipmentInfo = 0;
	memset(m_aryEquipmentInfo,0,sizeof(m_aryEquipmentInfo));
}

CResponseQueryPlayerItemData::~CResponseQueryPlayerItemData()
{

}

int32_t  CResponseQueryPlayerItemData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	//以下的编码 是否要根据m_nResultID的值来决定是否被编码？
	if (result_id_success == m_nResultID)
	{
		if (m_nSuperTypeCount < 0 || m_nSuperTypeCount > MAX_QUERY_COUNT)
		{
			return fail;
		}

		encode_length = CCodeEngine::encode_int32(&ptmp, m_nSuperTypeCount);
		iOutLength += encode_length;

		for(int32_t i=0; i<m_nSuperTypeCount; ++i)
		{
			encode_length = CCodeEngine::encode_int32(&ptmp, m_stSuperTypeInfo[i].m_nItemSuperType);
			iOutLength += encode_length;

			encode_length = CCodeEngine::encode_int32(&ptmp, m_stSuperTypeInfo[i].m_nItemCount);
			iOutLength += encode_length;

			if (m_stSuperTypeInfo[i].m_nItemCount > MAX_ITEM_COUNT)
			{
				return fail;
			}

			for(int32_t x=0; x<m_stSuperTypeInfo[i].m_nItemCount; ++x)
			{
				encode_length = encode_item_info2(&ptmp, m_stSuperTypeInfo[i].m_stItemInfos[x]);
				iOutLength += encode_length;
			}
		}

		encode_length = CCodeEngine::encode_int8(&ptmp, m_cHaveEquipmentInfo);
		iOutLength += encode_length;

		if (0 != m_cHaveEquipmentInfo)
		{
			for(int32_t i=0;i<MAX_EQUIPMENT_COUNT; ++i)
			{
				encode_length = CCodeEngine::encode_int32(&ptmp, m_aryEquipmentInfo[i]);
				iOutLength += encode_length;
			}
		}

	}//if (result_id_success == m_nResultID)

	return success;
}
int32_t  CResponseQueryPlayerItemData::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUIN);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if (result_id_success == m_nResultID)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nSuperTypeCount);
		remain_length -= decode_length;

		if(m_nSuperTypeCount > MAX_QUERY_COUNT)
		{
			return fail;
		}

		for(int32_t i=0;i<m_nSuperTypeCount;++i)
		{
			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stSuperTypeInfo[i].m_nItemSuperType);
			remain_length -= decode_length;

			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stSuperTypeInfo[i].m_nItemCount);
			remain_length -= decode_length;

			if(m_stSuperTypeInfo[i].m_nItemCount > MAX_ITEM_COUNT)
			{
				return fail;
			}

			for(int32_t x=0; x<m_stSuperTypeInfo[i].m_nItemCount; ++x)
			{
				decode_length = decode_item_info2(&ptmp, m_stSuperTypeInfo[i].m_stItemInfos[x]);
				remain_length -= decode_length;
			}
		}

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cHaveEquipmentInfo);
		remain_length -= decode_length;

		if (0 != m_cHaveEquipmentInfo)
		{
			for(int32_t i=0;i<MAX_EQUIPMENT_COUNT;++i)
			{
				decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_aryEquipmentInfo[i]);
				remain_length -= decode_length;
			}
		}
	
	}//if (result_id_success == m_nResultID)

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CResponseQueryPlayerItemData::dump()
{


}

CRequestUpdatePlayerEquipmentInfo::CRequestUpdatePlayerEquipmentInfo()
{

	m_nUIN = 0;
	m_nEquipmentUpdateCount = 0 ;
	memset(m_stEquipmentUpdate,0,sizeof(m_stEquipmentUpdate));

	m_nTransparentDataSize = 0;
	//memset(m_szTransparentData,0,sizeof(m_szTransparentData));

	m_nNofifyTransparentDataSize = 0;
	//memset(m_szNofifyTransparentData,0,sizeof(m_szNofifyTransparentData));
}

CRequestUpdatePlayerEquipmentInfo::~CRequestUpdatePlayerEquipmentInfo()
{


}

int32_t  CRequestUpdatePlayerEquipmentInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nEquipmentUpdateCount);
	iOutLength += encode_length;

	if (m_nEquipmentUpdateCount < 0 || m_nEquipmentUpdateCount > MAX_EQUIPMENT_COUNT)
	{
		return fail;
	}

	for(int32_t i=0; i< m_nEquipmentUpdateCount; ++i)
	{
		encode_length = encode_EquipmentUpdate(&ptmp,m_stEquipmentUpdate[i]);
		iOutLength += encode_length;
	}

	if ( m_nTransparentDataSize < 0 || m_nTransparentDataSize > (int16_t)sizeof(m_szTransparentData))
	{
		return fail;
	}

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	if (m_nNofifyTransparentDataSize < 0 || m_nNofifyTransparentDataSize > (int16_t)sizeof(m_szNofifyTransparentData))
	{
		return fail;
	}

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestUpdatePlayerEquipmentInfo::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUIN);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nEquipmentUpdateCount);
	remain_length -= decode_length;

	if (m_nEquipmentUpdateCount < 0 || m_nEquipmentUpdateCount > MAX_EQUIPMENT_COUNT)
	{
		return fail;
	}

	if(m_nEquipmentUpdateCount > MAX_EQUIPMENT_COUNT)
	{
		m_nEquipmentUpdateCount = MAX_EQUIPMENT_COUNT;
	}

	for(int32_t i=0; i<m_nEquipmentUpdateCount; ++i)
	{
		decode_length = decode_EquipmentUpdate(&ptmp,m_stEquipmentUpdate[i]);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize < 0 || m_nTransparentDataSize > (int16_t)sizeof(m_szTransparentData))
	{
		return fail;
	}

	if(m_nTransparentDataSize>max_transparent_data_size)
	{
		m_nTransparentDataSize = max_transparent_data_size;
	}
	decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize < 0 || m_nNofifyTransparentDataSize > (int16_t)sizeof(m_szNofifyTransparentData))
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		m_nNofifyTransparentDataSize = max_sub_message_size;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CRequestUpdatePlayerEquipmentInfo::dump()
{

}


CResponseUpdatePlayerEquipmentInfo::CResponseUpdatePlayerEquipmentInfo()
{

	m_nUIN = 0;
	m_nResultID = result_id_success;
	memset(m_aryEquipmentInfo,0,sizeof(m_aryEquipmentInfo));

	m_nTransparentDataSize = 0;
	//memset(m_szTransparentData,0,sizeof(m_szTransparentData));
}
CResponseUpdatePlayerEquipmentInfo::~CResponseUpdatePlayerEquipmentInfo()
{


}

int32_t  CResponseUpdatePlayerEquipmentInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	for(int32_t i=0;i<MAX_EQUIPMENT_COUNT;++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, m_aryEquipmentInfo[i]);
		iOutLength += encode_length;
	}

	if ( m_nTransparentDataSize < 0 || m_nTransparentDataSize > (int16_t)sizeof(m_szTransparentData))
	{
		return fail;
	}

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponseUpdatePlayerEquipmentInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUIN);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	for(int32_t i=0;i<MAX_EQUIPMENT_COUNT;++i)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryEquipmentInfo[i]);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp,  (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;
	
	if (m_nTransparentDataSize < 0 ||m_nTransparentDataSize > (int16_t)sizeof(m_szTransparentData))
	{
		return fail;
	}

	if(m_nTransparentDataSize >max_transparent_data_size)
		m_nTransparentDataSize = max_transparent_data_size;

	decode_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CResponseUpdatePlayerEquipmentInfo::dump()
{
	return ;
}



CNotifyPlayerEquipmentInfo::CNotifyPlayerEquipmentInfo()
{
	m_nUIN = 0;
	memset(m_aryEquipmentInfo,0,sizeof(m_aryEquipmentInfo));

	m_nNofifyTransparentDataSize = 0;
	//memset(m_szNofifyTransparentData,0,sizeof(m_szNofifyTransparentData));
}

CNotifyPlayerEquipmentInfo::~CNotifyPlayerEquipmentInfo()
{

}

int32_t  CNotifyPlayerEquipmentInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_nUIN);
	iOutLength += encode_length;

	for(int32_t i = 0; i < MAX_EQUIPMENT_COUNT; ++i)
	{
			encode_length = CCodeEngine::encode_int32(&ptmp, m_aryEquipmentInfo[i]);
			iOutLength += encode_length;
	}
	 
	
	if ( m_nNofifyTransparentDataSize < 0 || m_nNofifyTransparentDataSize > (int16_t)sizeof(m_szNofifyTransparentData))
	{
		return fail;
	}
	
	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CNotifyPlayerEquipmentInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*) &m_nUIN);
	remain_length -= decode_length;
 
	for(int32_t i = 0; i < MAX_EQUIPMENT_COUNT; ++i)
	{
			decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_aryEquipmentInfo[i]);
			remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if ( m_nNofifyTransparentDataSize < 0 || m_nNofifyTransparentDataSize > (int16_t)sizeof(m_szNofifyTransparentData))
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > max_sub_message_size)
		m_nNofifyTransparentDataSize = max_sub_message_size;

	decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CNotifyPlayerEquipmentInfo::dump()
{

}

}} //namespace KingNet { namespace Server {  

