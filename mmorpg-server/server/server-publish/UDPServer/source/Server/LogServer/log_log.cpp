#include "log_log.h"
#include "Common/common_datetime.h"
#include <dirent.h>

namespace KingNet { namespace Server { namespace LogServer {


	CLogServerLog::CLogServerLog()
	{
		m_iMaxSystemLogFileSize  = 0x40000000;
		m_iMaxSystemLogFileCount = 100;
	}

	CLogServerLog::~CLogServerLog()
	{
		//do nothing
	}

	int32_t CLogServerLog::WriteThreadLog(int32_t process, int32_t iLogLevel, const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogLevel))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/logthread%d_%s",process, szCurDate);

		va_list ap;
		va_start(ap, szContent);

		char* pTitle = m_stLogEngine.get_log_level_title(iLogLevel);

		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, pTitle, szContent, ap);
		va_end(ap);

		return iRet;
	}
	int32_t CLogServerLog::WriteServerLog(int32_t iLogLevel, const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogLevel))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/logserver_%s", szCurDate);

		va_list ap;
		va_start(ap, szContent);

		char* pTitle = m_stLogEngine.get_log_level_title(iLogLevel);

		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, pTitle, szContent, ap);
		va_end(ap);

		return iRet;
	}

	int32_t CLogServerLog::WriteServerLog(const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/logserver_%s", szCurDate);

		va_list ap;

		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, "detail", szContent, ap);
		va_end(ap);

		return iRet;
	}
	int32_t CLogServerLog::WriteThreadLog(int32_t process,const char* szContent, ...)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/logthread%d_%s",process, szCurDate);

		va_list ap;

		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName, m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, "detail", szContent, ap);
		va_end(ap);

		return iRet;
	}
	int32_t CLogServerLog::PrintServerBin(int32_t iLogLevel, char* pcBuffer, int32_t iLength)
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogLevel))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/logserver_%s", szCurDate);

		iRet = m_stLogEngine.dumpbin(szFileName, pcBuffer, iLength);

		return iRet;
	}

	int32_t CLogServerLog::SetLogLevel(int32_t iLogLevel)
	{
		m_stLogEngine.set_logmask(iLogLevel);
		return success;
	}

	int32_t CLogServerLog::MakeDir(const char* szPathName)
	{
		DIR* dpLog = opendir((const char *)szPathName);
		if(!dpLog)
		{
			if(mkdir(szPathName, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
			{
				return fail;
			}
		}
		else
		{
			closedir(dpLog);
		}

		return success;
	}


}}}//namespace KingNet { namespace Server { namespace LogServer {


