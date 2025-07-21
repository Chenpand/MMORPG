/**
@file   server_config_message_body.h
@brief  定义了和config server 通讯的包体结构

@author sjij@mail.51.com
@date Begin 2008.9
@date Dnd 2008.9
*/

#ifndef _SERVER_CONFIG_MESSAGE_H_
#define _SERVER_CONFIG_MESSAGE_H_

#include <stdarg.h>
#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "server_message.h"

namespace KingNet { namespace Server { 

	const int32_t MAX_CONFIG_FILE_SIZE = 0x8000;

	//////////////////////////////////////////////////////////////////////////
	//catalog Server VS config Server
	//////////////////////////////////////////////////////////////////////////
	/**	
	@brief  登录拉取配置文件消息体
	*/
	class  CSSRequestGetFile
	{
	public:
		CSSRequestGetFile();
		~CSSRequestGetFile();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		int32_t m_nServerID;
		int8_t	m_byServerType;
		int32_t m_nConfigID;
		
	private:
	};

	
	/**	
	@brief  拉取配置文件响应
	*/
	class  CSSResponseGetFile
	{
	public:
		CSSResponseGetFile();
		~CSSResponseGetFile();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:

		int16_t m_shResult;

		int32_t m_nServerID;
		int8_t	m_byServerType;
		int32_t m_nConfigID;
		
		int16_t m_shIsCompressed;
		int32_t m_nCompressedLen;
		int32_t m_nDeCompressedLen;
		char	m_szFileContent[MAX_CONFIG_FILE_SIZE];

	};
	
	/**	
	@brief  推送配置文件消息体
	*/
	class  CSSRequestPushFile
	{
	public:
		inline CSSRequestPushFile();
		inline ~CSSRequestPushFile();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		int32_t m_nServerID;
		int8_t	m_byServerType;
		int32_t m_nConfigID;
		int16_t m_shIsCompressed;
		int32_t m_nCompressedLen;
		int32_t m_nDeCompressedLen;
		char	m_szFileContent[MAX_CONFIG_FILE_SIZE];
	private:
	};

	
	/**	
	@brief  推送配置文件响应
	*/
	class  CSSResponsePushFile
	{
	public:
		inline CSSResponsePushFile();
		inline ~CSSResponsePushFile();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:

		int16_t m_shResult;

	};

}}//namespace KingNet { namespace Server {
#endif

