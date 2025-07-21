#ifndef __STAT_LOG_H__
#define __STAT_LOG_H__

#include "gameplatform_types.h"
#include "Common/common_base.h"
#include "Common/common_logengine.h"
#include "Common/common_singleton.h"
#include "Common/common_datetime.h"
#include "public/server_message.h"
using namespace KingNet::Server;
namespace KingNet { namespace Server { namespace LogServer {

class CStatLog
{
public:
	CStatLog();
	~CStatLog();

public:

	int32_t InitLog(int32_t filesize, int32_t filenumber, char* pazFileName);

	int32_t WriteLog(const char* szContent,const char* tablename);

	bool WriteContentToFile(FILE * fpStat,const char* szContent);
	void SetProcessID(int32_t nProcessID);


protected:

	//文件限制
	int32_t m_iMaxSystemLogFileSize;
	int32_t m_iMaxSystemLogFileCount;
	
	char m_szFileName[PATH_MAX];

	char m_szCurrentFileName[PATH_MAX];
	int32_t m_iProcessID;
	int32_t m_iCount;
private:
	FILE* m_pstStatFile;
	stDateTime m_stNowDateTime;
};

//typedef Singleton<CStatLog> LOGSERVER_STATLOGENGINE;
//
//#define TRACELOGSVR LOGSERVER_STATLOGENGINE::Instance().WriteLog
}}}
#endif
