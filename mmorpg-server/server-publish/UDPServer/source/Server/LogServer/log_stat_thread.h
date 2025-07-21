#ifndef __STAT_THREAD_H__
#define __STAT_THREAD_H__
#include "log_common_define.h"
#include "public/server_message.h"
#include "public/server_log_message_body.h"
#include "log_message.h"
#include "log_socket_pipe.h"
#include "log_stat.h"
#include "log_log.h"
namespace KingNet { namespace Server { namespace LogServer {


class CStatThread{
public:
	CStatThread();
	~CStatThread();

	void run(int32_t index);
	void set_index(int32_t index);
	int32_t initialize(int32_t iFd);
//	int32_t send_data(const char *buf,int32_t iLength);
	int32_t	decode_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength);

	int32_t receive_data();
public:
	CSocketPipe m_SocketPipe;
	int32_t		m_iIndex;
	CStatLog	m_statLog;
	int32_t		m_iMaxSystemLogFileSize;
	int32_t		m_iMaxSystemLogFileCount;
	CCSNotifyWriteLog m_stNotifyBody;
	CCSHead		stCSHead;
};

}}}

#endif

