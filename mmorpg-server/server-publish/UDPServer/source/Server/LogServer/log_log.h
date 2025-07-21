#ifndef __log_SERVER_LOG_H__
#define __log_SERVER_LOG_H__

#include "gameplatform_types.h"
#include "Common/common_singleton.h"
#include "Common/common_logengine.h"

namespace KingNet { namespace Server { namespace LogServer {

	class CLogServerLog
	{
	public:
		CLogServerLog();
		virtual ~CLogServerLog();

	public:

		int32_t WriteServerLog(int32_t iLogLevel, const char* szContent, ...);

		int32_t WriteServerLog(const char* szContent, ...);
		int32_t WriteThreadLog(int32_t process,int32_t iLogLevel, const char* szContent, ...);
		int32_t WriteThreadLog(int32_t process,const char* szContent, ...);
		int32_t PrintServerBin(int32_t iLogLevel, char* pcBuffer, int32_t iLength);

		int32_t SetLogLevel(int32_t iLogLevel);


	protected:
		int32_t MakeDir(const char* szPathName);

	private:

		CLogEngine m_stLogEngine;

		//文件限制
		int32_t m_iMaxSystemLogFileSize;
		int32_t m_iMaxSystemLogFileCount;
	};

typedef Singleton<CLogServerLog>	LogServerLOG;

#define TRACESVR		LogServerLOG::Instance().WriteServerLog

#define TRACEBIN		LogServerLOG::Instance().PrintServerBin
#define SETTRACELEVEL	LogServerLOG::Instance().SetLogLevel

}}}//namespace KingNet { namespace Server { namespace LogServer {



#endif

