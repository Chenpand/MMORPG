/**
@file   cs_recommend_message.h
@brief  定义了recommend.game.51.com php 和recommend server通讯的包头和包体结构
*/

#ifndef _CS_RECOMMEND_MESSAGE_H_
#define _CS_RECOMMEND_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"

using namespace KingNet::Server;

namespace KingNet { namespace Server {

const int32_t MAX_CITY_NAME_LEN = 128;
const int32_t MAX_RETURN_UIN_COUNT = 30;

	/**	
	@brief  recommend.game.51.com发送的推荐用户请求协议
	*/
	class  CCSRequestRecommend:public CMessageBody
	{
	public:
		CCSRequestRecommend();
		~CCSRequestRecommend();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();
	
	public:

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

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
		
	private:
	};

	
	/**	
	@brief  recommend.game.51.com发送的推荐用户相应协议
	*/
	class  CCSResponseRecommend:public CMessageBody
	{
	public:
		CCSResponseRecommend();
		~CCSResponseRecommend();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t  m_iSumHitCount;
		int32_t  m_iUsedTime;

		int32_t  m_iReturnCount;
		int32_t  m_aiUin[MAX_RETURN_UIN_COUNT];
	};	
	
}}//namespace KingNet { namespace Server

#endif

