/**
@file   server_catalog_message_body.cpp
@brief  
@author sjij@mail.51.com
@date Begin 2008.5
@date Dnd 2008.5
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "Common/common_codeengine.h"
#include "public/server_catalog_message_body.h"

namespace KingNet { namespace Server {

//////////////////////////////////////////////////////////////////////////
//Client VS Catalog Server
//////////////////////////////////////////////////////////////////////////
	/**	
	@brief  登录拉取初始数据块消息体
	*/
	CCSRequestGetInitInfo::CCSRequestGetInitInfo()
	{
		//memset(&m_stBody, 0, sizeof(m_stBody));
	}

	CCSRequestGetInitInfo::~CCSRequestGetInitInfo()
	{

	}

	int32_t CCSRequestGetInitInfo::encode(char *pszOut, int32_t& iOutLength)
	{
		
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		//LocalVersions		
		if(m_stBody.shMoudleVersionsCount>MAX_CS_MODULE_VERSION_COUNT)
		{
			m_stBody.shMoudleVersionsCount = MAX_CS_MODULE_VERSION_COUNT;
		}
		
		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shMoudleVersionsCount);
		iOutLength += coded_length;

		for(int32_t i=0; i<m_stBody.shMoudleVersionsCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pMoudleVersions[i].shModuleID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pMoudleVersions[i].shLocalVersion);
			iOutLength += coded_length;
		}

		//ConfigHashs
		if(m_stBody.shConfigHashCount>MAX_CS_CONFIG_FILE_COUNT)
		{
			m_stBody.shConfigHashCount = MAX_CS_CONFIG_FILE_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shConfigHashCount);
		iOutLength += coded_length;

		for(int32_t i=0; i<m_stBody.shConfigHashCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pConfigHash[i].shConfigID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.pConfigHash[i].shLocalHash, HASH_CODE_LEN);
			iOutLength += coded_length;
		}

		//BlockHashs
		if(m_stBody.shBlockHashCount>MAX_CS_BLOCK_COUNT)
		{
			m_stBody.shBlockHashCount = MAX_CS_BLOCK_COUNT;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shBlockHashCount);
		iOutLength += coded_length;

		for(int32_t i=0; i<m_stBody.shBlockHashCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pBlockHash[i].shBlockID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.pBlockHash[i].BlockHash, HASH_CODE_LEN);
			iOutLength += coded_length;
		}

		//Statistics
		if(m_stBody.shStatisticCount>MAX_CS_STATISTIC_COUNT)
		{
			m_stBody.shStatisticCount = MAX_CS_STATISTIC_COUNT;
		}
        coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shStatisticCount);
		iOutLength += coded_length;
		for(int32_t i=0; i<m_stBody.shStatisticCount; i++)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pStatistic[i].nFileLen);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pStatistic[i].nActualFileLen);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.pStatistic[i].FileContent, m_stBody.pStatistic[i].nFileLen);
			iOutLength += coded_length;
		}

		return success;
	}


	int32_t CCSRequestGetInitInfo::decode(const char *pszIn, const int32_t iInLength)
	{		
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		//版本
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.shMoudleVersionsCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		if(m_stBody.shMoudleVersionsCount >0)
		{
			if(m_stBody.shMoudleVersionsCount>MAX_CS_MODULE_VERSION_COUNT)
			{
				m_stBody.shMoudleVersionsCount = MAX_CS_MODULE_VERSION_COUNT;
			}

			for(int32_t i=0; i<m_stBody.shMoudleVersionsCount; i++)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.pMoudleVersions[i].shModuleID));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stBody.pMoudleVersions[i].shLocalVersion));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}

		//配置文件
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.shConfigHashCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		if(m_stBody.shConfigHashCount >0)
		{
			if(m_stBody.shConfigHashCount>MAX_CS_CONFIG_FILE_COUNT)
			{
				m_stBody.shConfigHashCount = MAX_CS_CONFIG_FILE_COUNT;
			}
			for(int32_t i=0; i<m_stBody.shConfigHashCount; i++)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.pConfigHash[i].shConfigID));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_stBody.pConfigHash[i].shLocalHash), HASH_CODE_LEN);
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}

		//本地目录树缓存
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.shBlockHashCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if(m_stBody.shBlockHashCount>0)
		{
			if(m_stBody.shBlockHashCount>MAX_CS_BLOCK_COUNT)
			{
				m_stBody.shBlockHashCount = MAX_CS_BLOCK_COUNT;
			}
			for(int32_t i=0; i<m_stBody.shBlockHashCount; i++)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.pBlockHash[i].shBlockID));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}

				decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_stBody.pBlockHash[i].BlockHash), HASH_CODE_LEN);
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}

		//统计信息
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.shStatisticCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		if(m_stBody.shStatisticCount >0)
		{
			if(m_stBody.shStatisticCount>MAX_CS_STATISTIC_COUNT)
			{
				m_stBody.shStatisticCount = MAX_CS_STATISTIC_COUNT;
			}

			for(int32_t i=0; i<m_stBody.shStatisticCount; i++)
			{
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stBody.pStatistic[i].nFileLen));
				remained_length -= decoded_length;

				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stBody.pStatistic[i].nActualFileLen));
				remained_length -= decoded_length;
				if(max_cs_file_content_length > m_stBody.pStatistic[i].nFileLen)
				{
					decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_stBody.pStatistic[i].FileContent), m_stBody.pStatistic[i].nFileLen);
					remained_length -= decoded_length;
					if(remained_length<0)
					{
						return fail;
					}
				}
			}
		}

		if(remained_length<0)
		{
			return fail;
		}

		return success;
	}

	int32_t CCSRequestGetInitInfo::dump()
	{
		return success;
	}

	/**	
	@brief  返回结果是失败 响应
	*/
	CCSResponseFailedInfo::CCSResponseFailedInfo()
	{
		
	}

	CCSResponseFailedInfo::~CCSResponseFailedInfo()
	{

	}


	int32_t CCSResponseFailedInfo::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, shResult);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, &szReason[0], shReasonLen);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSResponseFailedInfo::decode(const char *pszIn, const int32_t iInLength)
	{
		return success;
	}

	int32_t CCSResponseFailedInfo::dump()
	{
		return success;
	}


	/**	
	@brief  得到目录块数据数包体 响应
	*/
	CCSResponseGetInitInfo::CCSResponseGetInitInfo()
	{
		//memset(&m_stBody, 0, sizeof(m_stBody));
	}

	CCSResponseGetInitInfo::~CCSResponseGetInitInfo()
	{

	}


	int32_t CCSResponseGetInitInfo::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shResult);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.ntimeStamp);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shVersionControlCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_stBody.shVersionControlCount*sizeof(stCSVersionControl));
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		if(m_stBody.shVersionControlCount>MAX_CS_MODULE_VERSION_COUNT)
		{
			m_stBody.shVersionControlCount = MAX_CS_MODULE_VERSION_COUNT;
		}

		for(int32_t i=0; i<m_stBody.shVersionControlCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pVersionControl[i].shModuleID);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pVersionControl[i].shServerVersion);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pVersionControl[i].shUpdateType);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;
		}

		//配置信息
		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shConfigIDCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		if(m_stBody.shConfigIDCount>MAX_CS_CONFIG_FILE_COUNT)
		{
			m_stBody.shConfigIDCount = MAX_CS_CONFIG_FILE_COUNT;
		}
		for(int32_t i=0; i<m_stBody.shConfigIDCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pConfigData[i].shConfigID);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pConfigData[i].shConfigContentLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pConfigData[i].shActualConfigContentLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.pConfigData[i].ConfigContent, m_stBody.pConfigData[i].shConfigContentLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;
		}

		//有效块
		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shValidBlockCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		if(m_stBody.shValidBlockCount>MAX_CS_BLOCK_COUNT)
		{
			m_stBody.shValidBlockCount = MAX_CS_BLOCK_COUNT;
		}
		for(int32_t i=0; i<m_stBody.shValidBlockCount; i++)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pValidBlock[i]);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;
		}

		coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.nHallMMOnlineCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.nHallOtherOnlineCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shPopulationBlockCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		if(m_stBody.shPopulationBlockCount>MAX_NODE_POPULATION_COUNT)
		{
			m_stBody.shPopulationBlockCount = MAX_NODE_POPULATION_COUNT;
		}
		for(int32_t i=0; i<m_stBody.shPopulationBlockCount; i++)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pPopulationBlockData[i].nNode);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pPopulationBlockData[i].nMMOnlinecount);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pPopulationBlockData[i].nOtherOnlinecount);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;
		}

		//根块
		coded_length = CCodeEngine::encode_int8(&ptmp, m_stBody.byHaveRoot);
		iOutLength += coded_length;
		if(m_stBody.byHaveRoot)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shBlockID);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shNodeBlockLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.shDecompressedLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.NodeBlockData, m_stBody.shNodeBlockLen);
			iOutLength += coded_length;
			if(iOutLength > max_cs_package_size)
				return fail;
		}

		return success;
	}

	int32_t CCSResponseGetInitInfo::decode(const char *pszIn, const int32_t iInLength)
	{
		return success;
	}

	int32_t CCSResponseGetInitInfo::dump()
	{
		return success;
	}

	/**	
	@brief  得到目录块数据包体
	*/
	CCSRequestGetBlockInfo::CCSRequestGetBlockInfo()
	{
		//memset(&m_stBody, 0, sizeof(m_stBody));
	}

	CCSRequestGetBlockInfo::~CCSRequestGetBlockInfo()
	{

	}

	int32_t CCSRequestGetBlockInfo::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stBody.shBlockCount);
		iOutLength += coded_length;

		if(m_stBody.shBlockCount>REQUEST_BLOCK_ITEM_COUNT)
		{
			m_stBody.shBlockCount = REQUEST_BLOCK_ITEM_COUNT;
		}

		for(int32_t i=0; i<m_stBody.shBlockCount; ++i)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stBody.pBlock[i].shBlockID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int8(&ptmp,(uint8_t)m_stBody.pBlock[i].byType);
			iOutLength += coded_length;
		}
		
		return success;
	}

	int32_t CCSRequestGetBlockInfo::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.shBlockCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if(m_stBody.shBlockCount > 0)
		{
			if( REQUEST_BLOCK_ITEM_COUNT < m_stBody.shBlockCount )
			{
				m_stBody.shBlockCount = REQUEST_BLOCK_ITEM_COUNT;
			}
			for(int16_t i=0; i<m_stBody.shBlockCount; i++)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stBody.pBlock[i].shBlockID));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				decoded_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)(&m_stBody.pBlock[i].byType));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}		

		if(remained_length<0)
		{
			return fail;
		}
		return success;
	}

	int32_t CCSRequestGetBlockInfo::dump()
	{
		return success;
	}


	/**	
	@brief  得到目录块数据包体
	*/
	CCSResponseGetBlockInfo::CCSResponseGetBlockInfo()
	{
		//memset(&m_stBody, 0, sizeof(m_stBody));
	}

	CCSResponseGetBlockInfo::~CCSResponseGetBlockInfo()
	{

	}

	int32_t CCSResponseGetBlockInfo::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.nHallMMOnlineCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.nHallOtherOnlineCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.shBlockCount);
		iOutLength += coded_length;
		if(iOutLength > max_cs_package_size)
			return fail;

		if(m_stBody.shBlockCount>REQUEST_BLOCK_ITEM_COUNT)
		{
			m_stBody.shBlockCount = REQUEST_BLOCK_ITEM_COUNT;
		}

		for(int32_t i=0; i<m_stBody.shBlockCount; i++)
		{
			coded_length = CCodeEngine::encode_int8(&ptmp, m_stBody.pBlockContent[i].byType);
			iOutLength += coded_length;

			if(iOutLength > max_cs_package_size)
				return fail;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pBlockContent[i].shBlockID);
			iOutLength += coded_length;

			if(iOutLength > max_cs_package_size)
				return fail;

			if(m_stBody.pBlockContent[i].byType == 0) //节点内容
			{
				coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pBlockContent[i].unContent.BlockContent.shBlockLen);
				iOutLength += coded_length;
				if(iOutLength > max_cs_package_size)
					return fail;

				coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pBlockContent[i].unContent.BlockContent.shDecompressedLen);
				iOutLength += coded_length;
				if(iOutLength > max_cs_package_size)
					return fail;

				coded_length = CCodeEngine::encode_memory(&ptmp, m_stBody.pBlockContent[i].unContent.BlockContent.content, m_stBody.pBlockContent[i].unContent.BlockContent.shBlockLen);
				iOutLength += coded_length;
				if(iOutLength > max_cs_package_size)
					return fail;
			}else if(m_stBody.pBlockContent[i].byType == 1)
			{
				coded_length = CCodeEngine::encode_int16(&ptmp, m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount);
				iOutLength += coded_length;
				if(iOutLength > max_cs_package_size)
					return fail;

				if(m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount > ONE_BLOCK_NODE_COUNT)
				{
					m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount = ONE_BLOCK_NODE_COUNT;
				}
				for(int16_t j=0; j<m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount; j++)
				{
					coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nNode);
					iOutLength += coded_length; 
					if(iOutLength > max_cs_package_size)
						return fail;

					coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nMMOnlinecount);
					iOutLength += coded_length;
					if(iOutLength > max_cs_package_size)
						return fail;

					coded_length = CCodeEngine::encode_int32(&ptmp, m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nOtherOnlinecount);
					iOutLength += coded_length;
					if(iOutLength > max_cs_package_size)
						return fail;
				}
			}
		}

		return success;
	}

	int32_t CCSResponseGetBlockInfo::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stBody.nHallMMOnlineCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}

		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stBody.nHallOtherOnlineCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stBody.shBlockCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if(m_stBody.shBlockCount>REQUEST_BLOCK_ITEM_COUNT)
		{
			m_stBody.shBlockCount = REQUEST_BLOCK_ITEM_COUNT;
		}

		for(int16_t i=0; i<m_stBody.shBlockCount; ++i)
		{
			decoded_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)(&m_stBody.pBlockContent[i].byType));
			remained_length -= decoded_length;
			if(remained_length<0)
			{
				return fail;
			}
			decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stBody.pBlockContent[i].shBlockID));
			remained_length -= decoded_length;
			if(remained_length<0)
			{
				return fail;
			}
		
			if(0 == m_stBody.pBlockContent[i].byType)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.pBlockContent[i].unContent.BlockContent.shBlockLen));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stBody.pBlockContent[i].unContent.BlockContent.shDecompressedLen));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_stBody.pBlockContent[i].unContent.BlockContent.content[0]), m_stBody.pBlockContent[i].unContent.BlockContent.shBlockLen);
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}else if(1 == m_stBody.pBlockContent[i].byType)
			{
				decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
				if(m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount > ONE_BLOCK_NODE_COUNT)
				{
					m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount = ONE_BLOCK_NODE_COUNT;
				}
				
				for(int32_t j=0; j<m_stBody.pBlockContent[i].unContent.NodeOnlineCount.shNodeCount; ++j)
				{
					decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nNode));
					remained_length -= decoded_length;
					if(remained_length<0)
					{
						return fail;
					}
					decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nMMOnlinecount));
					remained_length -= decoded_length;
					if(remained_length<0)
					{
						return fail;
					}
					decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stBody.pBlockContent[i].unContent.NodeOnlineCount.pArrNodeOnline[j].nOtherOnlinecount));
					remained_length -= decoded_length;
					if(remained_length<0)
					{
						return fail;
					}
				}
			}

		}

		if(remained_length<0)
		{
			return fail;
		}
		
		return success;
	}

	int32_t CCSResponseGetBlockInfo::dump()
	{
		return success;
	}

	/**	
	@brief  得到节点所在的块
	*/
	CCSRequestSearchBlock::CCSRequestSearchBlock()
	{
		//memset(&m_stBody, 0, sizeof(m_stBody));
	}

	CCSRequestSearchBlock::~CCSRequestSearchBlock()
	{

	}

	int32_t CCSRequestSearchBlock::encode(char *pszOut, int32_t& iOutLength)
	{
		return success;
	}

	int32_t CCSRequestSearchBlock::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stBody.shIDCount));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		if(m_stBody.shIDCount>0)
		{
			if( MAX_CS_BLOCK_COUNT < m_stBody.shIDCount )
			{
				m_stBody.shIDCount = MAX_CS_BLOCK_COUNT;
			}
			for(int16_t i=0; i<m_stBody.shIDCount; ++i)
			{
				decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_stBody.pNodeIDs[i]));
				remained_length -= decoded_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}

		if(remained_length<0)
		{
			return fail;
		}
		return success;
	}

	int32_t CCSRequestSearchBlock::dump()
	{
		return success;
	}

	
//////////////////////////////////////////////////////////////////////////
//Logic Server VS Catalog Server
//////////////////////////////////////////////////////////////////////////

	CRequestRegisterServer::CRequestRegisterServer()
	{
		m_iServerID = -1;
		m_cServerType = -1;
		m_iLocalIP = 0;
		m_nLocalPort = 0;
	}

	CRequestRegisterServer::~CRequestRegisterServer()
	{
		//do nothing
	}

	int32_t CRequestRegisterServer::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iLocalIP);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nLocalPort);
		iOutLength += coded_length;


		return success;
	}

	int32_t CRequestRegisterServer::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*) &m_iServerID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLocalIP);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLocalPort);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		return success;
	}

	void CRequestRegisterServer::dump()
	{
		
	}

	//response
	CResponseRegisterServer::CResponseRegisterServer()
	{
		m_nResultID = 0;
		m_nFlag = 0;

		m_iServerConfigDataSize = 0;
		m_iServerRoomDataSize = 0;
		//m_nDirtyWordsSize = 0;
		//memset(m_szServerConfigData, 0, max_server_config_data_size);
		//memset(m_szServerRoomData, 0, max_server_room_data_size);
		//memset(m_szDirtyWords, 0, max_dirty_words_size);
	}

	CResponseRegisterServer::~CResponseRegisterServer()
	{
		//do nothing 
	}

	int32_t CResponseRegisterServer::encode( char *pszOut, int32_t& iOutLength )
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

		if (0 == m_nResultID)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_nFlag);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerConfigDataSize);
//			coded_length = CCodeEngine::encode_int16(&ptmp, m_nServerConfigDataSize);
			iOutLength += coded_length;

			if ((int32_t)m_iServerConfigDataSize > 0)
			{
				coded_length = CCodeEngine::encode_memory(&ptmp, &m_szServerConfigData[0], (const int32_t)m_iServerConfigDataSize);
				iOutLength += coded_length;
			}

			coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerRoomDataSize);
//			coded_length = CCodeEngine::encode_int16(&ptmp, m_nServerRoomDataSize);
			iOutLength += coded_length;

			if ((int32_t)m_iServerRoomDataSize > 0)
			{
				coded_length = CCodeEngine::encode_memory(&ptmp, &m_szServerRoomData[0], (const int32_t)m_iServerRoomDataSize);
				iOutLength += coded_length;
			}

// 			coded_length = CCodeEngine::encode_int16(&ptmp, m_nDirtyWordsSize);
// 			iOutLength += coded_length;
// 
// 			if (m_nDirtyWordsSize > 0)
// 			{
// 				coded_length = CCodeEngine::encode_memory(&ptmp, &m_szDirtyWords[0], (const int32_t)m_nDirtyWordsSize);
// 				iOutLength += coded_length;
// 			}

		}

		return success;
	}

	int32_t CResponseRegisterServer::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if (0 == m_nResultID)
		{
			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFlag);
			remained_length -= decode_length;
			if(remained_length<0)
			{
				return fail;
			}
			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerConfigDataSize);
//			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nServerConfigDataSize);
			remained_length -= decode_length;
			if(remained_length<0)
			{
				return fail;
			}
			
			if ((int32_t)m_iServerConfigDataSize > 0) //有问题!!!当超过32k时
			{
				decode_length = CCodeEngine::decode_memory(&ptmp, &m_szServerConfigData[0], (const int32_t)m_iServerConfigDataSize);
				remained_length -= decode_length;
				if(remained_length<0)
				{
					return fail;
				}
			}

			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerRoomDataSize);
//			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nServerRoomDataSize);
			remained_length -= decode_length;

			if ((int32_t)m_iServerRoomDataSize > 0)//有问题!!!当超过32k时
			{
				decode_length = CCodeEngine::decode_memory(&ptmp, &m_szServerRoomData[0], (const int32_t)m_iServerRoomDataSize);
				remained_length -= decode_length;
				if(remained_length<0)
				{
					return fail;
				}
			}

// 			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDirtyWordsSize);
// 			remain_length -= decode_length;
// 
// 			if (m_nDirtyWordsSize > 0)
// 			{
// 				decode_length = CCodeEngine::decode_memory(&ptmp, &m_szDirtyWords[0], (const int32_t)m_nDirtyWordsSize);
// 				remain_length -= decode_length;
// 			}


		}//if

		return success;
	}

	void CResponseRegisterServer::dump()
	{
	}

	//上报人数信息
	CSSNotifyReportStatics::CSSNotifyReportStatics()
	{
		m_iServerID = -1;
		m_cServerType = -1;
		m_nRoomCount = 0;
	}

	CSSNotifyReportStatics::~CSSNotifyReportStatics()
	{

	}

	int32_t CSSNotifyReportStatics::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nRoomCount);
		iOutLength += coded_length;

		for (int32_t i = 0; i < m_nRoomCount; ++i)
		{
			coded_length = encode_room_statics(&ptmp, m_stRoomStatics[i]);
			iOutLength += coded_length;
		}
		return success;
	}

	int32_t CSSNotifyReportStatics::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nRoomCount);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		for (int32_t i = 0; i < m_nRoomCount; ++i)
		{
			decode_length = decode_room_statics(&ptmp, m_stRoomStatics[i]);
			remained_length -= decode_length;
			if(remained_length<0)
			{
				return fail;
			}
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CSSNotifyReportStatics::dump()
	{
		return;
	}


	int32_t encode_room_statics(char** pOut, stRoomStatics& statics)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int8_t data_size = 0; //不包含自身的1Byte
		char* ptmp = *pOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pOut += sizeof(data_size); //为元素的长度预留1Byte的空间

		coded_length = CCodeEngine::encode_int32(pOut, statics.m_iRoomID);
		data_size += coded_length;

		coded_length = CCodeEngine::encode_int16(pOut, statics.m_nMMCount);
		data_size += coded_length;

		coded_length = CCodeEngine::encode_int16(pOut, statics.m_nOtherCount);
		data_size += coded_length;

		*pOut -= (data_size + sizeof(data_size));
		coded_length = CCodeEngine::encode_int8(pOut, data_size);

		iOutLength = coded_length + data_size;

		*pOut = ptmp + iOutLength;

		return iOutLength;
	}

	int32_t decode_room_statics(char** pOut, stRoomStatics& statics)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int8_t data_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&statics.m_iRoomID);
		iOutLength += decode_length;
		data_size -= decode_length;
		
		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&statics.m_nMMCount);
		iOutLength += decode_length;
		data_size -= decode_length;

		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&statics.m_nOtherCount);
		iOutLength += decode_length;
		data_size -= decode_length;

		if (data_size < 0)
		{
			return 0;
		}
		else
		{
			//
			skip_byte_of(pOut, data_size);
			iOutLength += data_size;
		}

		return iOutLength;
	}

// 	void skip_byte_of(char** pOut, int32_t iByteCount)
// 	{
// 		if (pOut == NULL || NULL == *pOut || iByteCount <= 0)
// 		{
// 			return;
// 		}
// 
// 		(*pOut) += iByteCount;
// 
// 		return;
// 	}


	//////////////////////////////////////////////////////////////////////////
	//State Server VS Catalog Server
	//////////////////////////////////////////////////////////////////////////
	CResponseRegisterStateServer::CResponseRegisterStateServer()
	{
		m_nResultID = 0;
		m_nFlag = 0;

		m_nServerConfigDataSize = 0;
		
		memset(m_szServerConfigData, 0, max_server_config_data_size);
	}

	CResponseRegisterStateServer::~CResponseRegisterStateServer()
	{
		//do nothing 
	}

	int32_t CResponseRegisterStateServer::encode( char *pszOut, int32_t& iOutLength )
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

		if (0 == m_nResultID)
		{
			coded_length = CCodeEngine::encode_int16(&ptmp, m_nFlag);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_nServerConfigDataSize);
			iOutLength += coded_length;

			if (m_nServerConfigDataSize > 0)
			{
				coded_length = CCodeEngine::encode_memory(&ptmp, &m_szServerConfigData[0], (const int32_t)m_nServerConfigDataSize);
				iOutLength += coded_length;
			}
		}

		return success;
	}

	int32_t CResponseRegisterStateServer::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if (0 == m_nResultID)
		{
			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nFlag);
			remained_length -= decode_length;
			if(remained_length<0)
			{
				return fail;
			}
			
			decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nServerConfigDataSize);
			remained_length -= decode_length;
			if(remained_length<0)
			{
				return fail;
			}
			
			if (m_nServerConfigDataSize > 0)
			{
				decode_length = CCodeEngine::decode_memory(&ptmp, &m_szServerConfigData[0], (const int32_t)m_nServerConfigDataSize);
				remained_length -= decode_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}//if

		return success;
	}

	void CResponseRegisterStateServer::dump()
	{
	
	}


	CSSNotifyReportStateStatics::CSSNotifyReportStateStatics()
	{
		m_iServerID = -1;
		m_cServerType = -1;
		m_nMMCount = -1;
		m_nOtherCount = -1;
	}

	CSSNotifyReportStateStatics::~CSSNotifyReportStateStatics()
	{

	}

	int32_t CSSNotifyReportStateStatics::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nMMCount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nOtherCount);
		iOutLength += coded_length;

		return success;
	}

	int32_t CSSNotifyReportStateStatics::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nMMCount);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nOtherCount);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		
		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CSSNotifyReportStateStatics::dump()
	{
	}

	/*
	*  web节点上报人数 notify消息
	*
	*/
	//上报人数信息
	CWSNotifyReportStatics::CWSNotifyReportStatics()
	{
		m_iServerID = -1;
		m_cServerType = -1;
		m_nRoomCount = 0;
	}

	CWSNotifyReportStatics::~CWSNotifyReportStatics()
	{

	}

	int32_t CWSNotifyReportStatics::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_nRoomCount);
		iOutLength += coded_length;
		if( 0 < m_nRoomCount )
		{
			if( MAX_ROOM_COUNT_PER_SERVER < m_nRoomCount )
			{
				m_nRoomCount = MAX_ROOM_COUNT_PER_SERVER;
			}
			for (int32_t i = 0; i < m_nRoomCount; ++i)
			{
				coded_length = encode_web_room_statics(&ptmp, m_stRoomStatics[i]);
				iOutLength += coded_length;
			}
		}
		return success;
	}

	int32_t CWSNotifyReportStatics::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nRoomCount);
		remained_length -= decode_length;
		if(remained_length<0)
		{
			return fail;
		}
		if( MAX_ROOM_COUNT_PER_SERVER < m_nRoomCount )
		{
			if( MAX_ROOM_COUNT_PER_SERVER > m_nRoomCount )
			{
				m_nRoomCount = MAX_ROOM_COUNT_PER_SERVER;
			}
			for (int32_t i = 0; i < m_nRoomCount; ++i)
			{
				decode_length = decode_web_room_statics(&ptmp, m_stRoomStatics[i]);
				remained_length -= decode_length;
				if(remained_length<0)
				{
					return fail;
				}
			}
		}
		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CWSNotifyReportStatics::dump()
	{
		return;
	}


	int32_t encode_web_room_statics(char** pOut, stWebRoomStatics& statics)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int8_t data_size = 0; //不包含自身的1Byte
		char* ptmp = *pOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pOut += sizeof(data_size); //为元素的长度预留1Byte的空间

		coded_length = CCodeEngine::encode_int32(pOut, statics.m_iRoomID);
		data_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, statics.m_nMMCount);
		data_size += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, statics.m_nOtherCount);
		data_size += coded_length;

		*pOut -= (data_size + sizeof(data_size));
		coded_length = CCodeEngine::encode_int8(pOut, data_size);

		iOutLength = coded_length + data_size;

		*pOut = ptmp + iOutLength;

		return iOutLength;
	}

	int32_t decode_web_room_statics(char** pOut, stWebRoomStatics& statics)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}

		int8_t data_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&statics.m_iRoomID);
		iOutLength += decode_length;
		data_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&statics.m_nMMCount);
		iOutLength += decode_length;
		data_size -= decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&statics.m_nOtherCount);
		iOutLength += decode_length;
		data_size -= decode_length;

		if (data_size < 0)
		{
			return 0;
		}
		else
		{
			//
			skip_byte_of(pOut, data_size);
			iOutLength += data_size;
		}

		return iOutLength;
	}


	}}//namespace KingNet { namespace Server { 

