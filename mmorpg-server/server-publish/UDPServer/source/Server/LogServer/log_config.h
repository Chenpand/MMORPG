#ifndef __LOGSERVER_CONIFG_H__
#define __LOGSERVER_CONIFG_H__
//
#include "Common/common_base.h"
#include "Common/common_singleton.h"
//
#include "log_common_define.h"

namespace KingNet { namespace Server { namespace LogServer {


class CLogServerConfig
{
public:
	CLogServerConfig();
	~CLogServerConfig();

	int32_t initialize();
	int32_t load_config(const char* pszConfigFile);
	int32_t load_config();
	int32_t reload_config(const char* pszConfigFile);
	int32_t reload_config();
	int32_t read_table_xml(const char* pFile);
	int32_t get_table_name(int32_t id,char* pOutName,int32_t inLen);
	int32_t m_iServerID;

	int32_t m_iCoreKey;
	int32_t m_iCSShareMemoryKey;
	int32_t m_iSCShareMemoryKey;

	int32_t m_iLogMask;

	int32_t m_iUDPCSShareMemoryKey;
	int32_t m_iUDPSCShareMemoryKey;

	TFileName m_pszTableXml;
	TFileName m_szTableName[max_table_count];

	int32_t m_iStatThreadCount;
	int32_t m_iMaxMessageBodyQueueSize;
	int32_t m_iMaxMessageEntryCount;
	
};



#define  LOGSVRCONFIG Singleton<CLogServerConfig>::Instance()


 }}}

#endif

