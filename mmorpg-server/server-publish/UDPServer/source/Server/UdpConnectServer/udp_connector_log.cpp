
#include "udp_connector_log.h"
#include "Common/common_datetime.h"

namespace KingNet { namespace Server { namespace UdpConnectorServer{

	CUdpConnectorLog::CUdpConnectorLog()
	{
		//文件限制
		m_iMaxSystemLogFileSize = 0x2000000;
		m_iMaxSystemLogFileCount = 5;
	}

	CUdpConnectorLog::~CUdpConnectorLog()
	{

	}

	int32_t CUdpConnectorLog::WriteServerLog( int iLogLevel, const char* szContent, ... )
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogLevel))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/udpconnectorsvrd_%s", szCurDate);

		char* title = m_stLogEngine.get_log_level_title(iLogLevel);

		va_list ap;

		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName,
			m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
		va_end(ap);

		return iRet;
	}	

	int32_t CUdpConnectorLog::WriteServerLog( const char* szContent, ... )
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];
		//char szErrInfo[256];

		
		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/udpconnectorsvrd_%s", szCurDate);

		char* title = "detail";

		va_list ap;

		va_start(ap, szContent);
		iRet = m_stLogEngine.log(szFileName,
			m_iMaxSystemLogFileSize, m_iMaxSystemLogFileCount, title, szContent,ap);
		va_end(ap);

		return iRet;
	}

	int32_t CUdpConnectorLog::PrintServerBin( int iLogLevel, /*日志级别 */ char *pcBuffer, int iLength )
	{
		int iRet = success;
		char szFileName[256];
		char szCurDate[32];

		if(!m_stLogEngine.whether_do_log(iLogLevel))
		{
			return fail;
		}

		get_current_string_date((char *)szCurDate);
		sprintf(szFileName, "./log/udpconnectorsvrd_%s", szCurDate);

		iRet = m_stLogEngine.dumpbin(szFileName,pcBuffer,iLength);

		return iRet;
	}

	void CUdpConnectorLog::SetLogLevel( int iLogLevel )
	{
		return m_stLogEngine.set_logmask(iLogLevel);
	}

}}}//namespace KingNet { namespace Server { { namespace ConnectorServer

