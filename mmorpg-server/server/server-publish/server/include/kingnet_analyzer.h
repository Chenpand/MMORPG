#ifndef _KINGNET_ANALYZER_H_
#define _KINGNET_ANALYZER_H_

#include "udplog_typedef.h"
#include "udplog_singleton.h"
#include "udplog_config.h"

#include <map>
#include <string>
#include <cstdio>
#include <ctime>

class CUserLogger
{
public:
	Int32 m_nUid;
	Int32 m_nErrorNo;
	char	m_szUserIP[MAX_IPADDR_LENGTH];
	Int32 m_nTime;
	char	m_szUserMsg[MAX_USERMSG_LENGTH];
	bool	m_bForbidden;
	Int32	m_nSendCount;
	char m_szLogBuffer[MAX_LOG_LENGTH];

public:
	CUserLogger(Int32 uid)
	{
		m_nUid		= uid;
		m_nErrorNo	= 0;
		m_bForbidden= false;
		m_szUserIP[0]='\0';
		m_szUserMsg[0] = '\0';
		m_nTime		= (Int32)time(NULL);
		m_nSendCount= 0;
		m_szLogBuffer[0] = '\0';

	}

	void SetUserIP(const char* ip)
	{
#ifndef AVALON_OS_WINDOWS
		strncpy(m_szUserIP, ip, MAX_IPADDR_LENGTH-1);
#else
		strncpy_s(m_szUserIP, sizeof(m_szUserIP), ip, strlen(ip));
#endif
	}

	void SetUserMsg(const char* msg)
	{
#ifndef AVALON_OS_WINDOWS
		strncpy(m_szUserMsg, msg, MAX_USERMSG_LENGTH-1);
#else
		strncpy_s(m_szUserMsg, sizeof(m_szUserMsg), msg, strlen(msg));
#endif

	}
	//send msg to remote
	//Int32 LogMsg(const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char*type = "login",Int32 nCount = 1);

	template <typename... Args>
	Int32 LogMsg(const char* typeInfo, const char* format, const Args & ... args)
	{
		Int32 type = _config.GetLogFileID(typeInfo);
		if (type < 0)
		{
			ErrorLogStream << "type:" << typeInfo << ",format:" << format << LogStream::eos;
			return UL_FAIL;
		}
		m_nTime = (Int32)time(NULL);
#ifndef AVALON_OS_WINDOWS
		snprintf(m_szLogBuffer, sizeof(m_szLogBuffer), format, m_szUserMsg, args...);
#else
		_snprintf_s(m_szLogBuffer, sizeof(m_szLogBuffer), format, m_szUserMsg, args...);
#endif
		m_szLogBuffer[MAX_LOG_LENGTH - 1] = '\0';
		SendUDPLog(m_nUid, type, m_szLogBuffer);

		return UL_OK;
	}


private:

	Int32 SendUDPLog(Int32 uid, Int32 type, const char* msg);
};

class CKingnetAnalyzer
{
private:
	std::map<std::string, CUserLogger*> m_mUserInfo;


public:
	CKingnetAnalyzer()
	{
	}
	Int32 Init(const char* path = DEFAULT_CONFIGFILENAME);

public:
	CUserLogger* GetInstance(const char* uid)
	{
		if(NULL == uid)
		{
			return NULL;
		}
		std::map<std::string, CUserLogger*>::iterator it = m_mUserInfo.find(std::string(uid));
		if(it == m_mUserInfo.end())
		{
			//not found, new one
			Int32 iUid = atoi(uid);
			if(0 == iUid)
			{
				iUid = 1;
			}
			CUserLogger *info = new CUserLogger(iUid);
			m_mUserInfo[std::string(uid)] = info;
		}
		return m_mUserInfo[uid];
	}
	void DestoryInstance(const char* uid)
	{
		std::map<std::string, CUserLogger*>::iterator it = m_mUserInfo.find(std::string(uid));
		if(it != m_mUserInfo.end())
		{
			CUserLogger *info = m_mUserInfo[std::string(uid)];
			delete info;
			m_mUserInfo.erase(uid);
		}
	}
};
#define	CREATE_ANALYZER_INSTANCE	CSingleton<CKingnetAnalyzer>::CreateInstance
#define	GET_ANALYZER_INSTANCE		CSingleton<CKingnetAnalyzer>::GetInstance
#define	DESTROY_ANALYZER_INSTANCE	CSingleton<CKingnetAnalyzer>::DestroyInstance

#define _analyzer GET_ANALYZER_INSTANCE()
#endif
