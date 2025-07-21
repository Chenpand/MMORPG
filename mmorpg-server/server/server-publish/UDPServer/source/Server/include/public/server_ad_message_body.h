#ifndef _AD_SERVER_MESSAGE_BODY_H_
#define _AD_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

	enum
	{
		max_adinfo_count = 256,
		max_mac_list_count = 30000,
	};

	enum
	{
		cycle_one_time_forever		 = 1,   //只发一次
		cycle_one_time_for_one_month = 2,   //每月一次
		cycle_one_time_for_one_week  = 3,   //每周一次
		cycle_one_time_for_one_day   = 4,   //每天一次
	};

	enum 
	{
		query_publishing_and_will_publish_ad = -1, //查询正在发送和将要发送的广告列表
		query_publishing_ad					 = 1,  //查询正在发送的广告列表
		query_will_publish_ad				 = 2,  //查询将要发送的广告列表
	};

	typedef struct ADInfo
	{
		int32_t m_iADInfoLength;
		int32_t m_iADID;
		int32_t m_iStartTime;
		int32_t m_iEndTime;
		int16_t m_shCycleType;
		char m_szURL[max_url_length];
		int32_t m_iMacCount;
		int16_t m_shState;
	}stADInfo;

	class CRspADInfo : public CMessageBody
	{
	public:
		CRspADInfo();
		~CRspADInfo();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		char m_szURL[max_url_length];
	};

	class CRequestPublishAD : public CMessageBody
	{
	public:
		CRequestPublishAD();
		~CRequestPublishAD();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		stADInfo m_stADInfo;

	};

	class CResponsePublishAD : public CMessageBody
	{
	public:
		CResponsePublishAD();
		~CResponsePublishAD();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int32_t m_iADID;
		int16_t m_shResultID;
		TReason m_szReasonMessage;
	};

	class CRequestDeleteAD : public CMessageBody
	{
	public:
		CRequestDeleteAD();
		~CRequestDeleteAD();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int32_t m_iADID;
	};
	class CResponseDeleteAD : public CMessageBody
	{
	public:
		CResponseDeleteAD();
		~CResponseDeleteAD();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int32_t m_iADID;
		int16_t m_shResultID;
		TReason m_szReasonMessage;
	};

	class CRequestGetADList : public CMessageBody
	{
	public:
		CRequestGetADList();
		~CRequestGetADList();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int16_t m_shType;
	};

	class CResponseGetADList : public CMessageBody
	{
	public:
		CResponseGetADList();
		~CResponseGetADList();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int16_t m_shResultID;

		int16_t m_shADInfoCount;
		stADInfo m_szADInfo[max_adinfo_count];

		TReason m_szReasonMessage;
	};

	class CRequestSendMacList : public CMessageBody
	{
	public:
		CRequestSendMacList();
		~CRequestSendMacList();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int32_t m_iADID;
		int32_t m_iMacCount;
		int64_t m_szMac[max_mac_list_count];
	};

	class CResponseSendMacList : public CMessageBody
	{
	public:
		CResponseSendMacList();
		~CResponseSendMacList();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int16_t m_shResultID;

		TReason m_szReasonMessage;
	};
}}

#endif //_AD_SERVER_MESSAGE_BODY_H_

