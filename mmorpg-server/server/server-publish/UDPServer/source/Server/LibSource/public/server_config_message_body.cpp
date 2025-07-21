/**
@file   server_config_message_body.cpp
@brief  
@author sjij@mail.51.com
@date Begin 2008.9
@date Dnd 2008.9
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "Common/common_codeengine.h"
#include "public/server_config_message_body.h"

namespace KingNet { namespace Server {

	//////////////////////////////////////////////////////////////////////////
	//Catalog VS Config Server
	//////////////////////////////////////////////////////////////////////////
	/**	
	@brief  登录拉取初始数据块消息体
	*/
	CSSRequestGetFile::CSSRequestGetFile()
	{
		
	}

	CSSRequestGetFile::~CSSRequestGetFile()
	{

	}

	int32_t CSSRequestGetFile::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nConfigID);
		iOutLength += coded_length;

		return success;
	}


	int32_t CSSRequestGetFile::decode(const char *pszIn, const int32_t iInLength)
	{		
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		//
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nServerID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_byServerType));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nConfigID));
		remained_length -= decoded_length;

		if(remained_length<0)
		{
			return fail;
		}

		return success;
	}

	int32_t CSSRequestGetFile::dump()
	{
		return success;
	}

	/**	
	@brief  返回结果 响应
	*/
	CSSResponseGetFile::CSSResponseGetFile()
	{

	}

	CSSResponseGetFile::~CSSResponseGetFile()
	{

	}


	int32_t CSSResponseGetFile::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResult);
		iOutLength += coded_length;

		if(0 == m_shResult)
		{
			coded_length = CCodeEngine::encode_int32(&ptmp, m_nServerID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int8(&ptmp, m_byServerType);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_nConfigID);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int16(&ptmp, m_shIsCompressed);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_nCompressedLen);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_int32(&ptmp, m_nDeCompressedLen);
			iOutLength += coded_length;

			coded_length = CCodeEngine::encode_memory(&ptmp, &m_szFileContent[0], m_nCompressedLen);
			iOutLength += coded_length;
		}

		return success;
	}

	int32_t CSSResponseGetFile::decode(const char *pszIn, const int32_t iInLength)
	{
		if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_shResult));
		remained_length -= decoded_length;
		
		if(0 == m_shResult)
		{
			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nServerID));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_byServerType));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nConfigID));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_shIsCompressed));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nCompressedLen));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nDeCompressedLen));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_szFileContent[0]), m_nCompressedLen);
			remained_length -= decoded_length;
		}
		if(remained_length<0)
		{
			return fail;
		}
		return success;
	}

	int32_t CSSResponseGetFile::dump()
	{
		return success;
	}
	
	
	
	CSSRequestPushFile::CSSRequestPushFile()
	{
		
	}

	CSSRequestPushFile::~CSSRequestPushFile()
	{

	}
	int32_t CSSRequestPushFile::dump()
	{
		return success;
	}
	int32_t CSSRequestPushFile::encode(char *pszOut, int32_t& iOutLength)
	{
	    if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		
        coded_length = CCodeEngine::encode_int32(&ptmp, m_nServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_byServerType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nConfigID);
		iOutLength += coded_length;
        
        coded_length = CCodeEngine::encode_int16(&ptmp, m_shIsCompressed);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_nCompressedLen);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_nDeCompressedLen);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_memory(&ptmp, &m_szFileContent[0], m_nCompressedLen);
        iOutLength += coded_length;        
        
		
        
		return success;
	    
	}
    int32_t CSSRequestPushFile::decode(const char *pszIn, const int32_t iInLength)
    {
        if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;


        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nServerID));
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)(&m_byServerType));
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nConfigID));
        remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_shIsCompressed));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nCompressedLen));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_nDeCompressedLen));
			remained_length -= decoded_length;

			decoded_length = CCodeEngine::decode_memory(&ptmp, (char*)(&m_szFileContent[0]), m_nCompressedLen);
			remained_length -= decoded_length;        
        
	   
		
		if(remained_length<0)
		{
			return fail;
		}
		return success;
    }
    
    CSSResponsePushFile::CSSResponsePushFile()
	{
		
	}

	CSSResponsePushFile::~CSSResponsePushFile()
	{

	}
	int32_t CSSResponsePushFile::dump()
	{
		return success;
	}
	int32_t CSSResponsePushFile::encode(char *pszOut, int32_t& iOutLength)
	{
	    if(NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResult);
		iOutLength += coded_length;
		return success ;
	}
	int32_t CSSResponsePushFile::decode(const char *pszIn, const int32_t iInLength)
	{
	    if(NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_shResult));
		remained_length -= decoded_length;
		if(remained_length<0)
		{
			return fail;
		}
		return success;
	}
}}

