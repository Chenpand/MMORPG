#ifndef __UDPCONNECTOR_LOG_H__
#define __UDPCONNECTOR_LOG_H__

#include "Common/common_base.h"
#include "Common/common_logengine.h"
#include "Common/common_singleton.h"

namespace KingNet { namespace Server {

	class CLogEngine;

}}//namespace KingNet { namespace Server {

namespace KingNet { namespace Server { namespace UdpConnectorServer{

enum
{
	log_mask_everything		= 0xffffffff,
};

class CUdpConnectorLog
{
public:
	CUdpConnectorLog();
	~CUdpConnectorLog();

	int32_t WriteServerLog(int iLogLevel,
		const char* szContent, ...);

	int32_t WriteServerLog(
		const char* szContent, ...);

	int32_t PrintServerBin(int iLogLevel,		//日志级别
		char *pcBuffer,
		int iLength);

	void SetLogLevel(int iLogLevel);

protected:

private:
	CLogEngine m_stLogEngine;

	//文件限制
	int32_t m_iMaxSystemLogFileSize;
	int32_t m_iMaxSystemLogFileCount;


private:
};

typedef Singleton<CUdpConnectorLog> UDPCONNECTORSERVER_LOGENGINE;

#define TRACESVR UDPCONNECTORSERVER_LOGENGINE::Instance().WriteServerLog


#define TRACEBIN UDPCONNECTORSERVER_LOGENGINE::Instance().PrintServerBin

#define SETTRACELEVEL UDPCONNECTORSERVER_LOGENGINE::Instance().SetLogLevel


}}}//namespace KingNet { namespace Server { namespace UdpConnectorServer {



#endif

