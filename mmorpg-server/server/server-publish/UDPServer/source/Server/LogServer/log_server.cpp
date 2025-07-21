#include <signal.h>
#include "Common/common_base.h"
#include "Common/common_namespace_definition.h"
#include "Common/common_config.h"
#include "Common/common_codequeue.h"
#include "Common/common_singleton.h"
#include "Common/common_flag.h"

#include "log_logicframe.h"
#include "log_config.h"
#include "log_log.h"

#include <pthread.h>

using namespace KingNet;
using namespace KingNet::Server;
using namespace KingNet::Server::LogServer;
typedef struct 
{
	pthread_t thread_tid;  //thread id
	long thread_count;     //connection handled
}Thread;

void sigusr1_handle( int iSigVal )
{
	g_byRunFlag.set(run_flag_reload );
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle( int iSigVal )
{
	g_byRunFlag.set(run_flag_exit);
	signal(SIGUSR2, sigusr2_handle);
}

int32_t initialize(int32_t mode)
{
	//安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);

	g_byRunFlag.initialize((uint32_t) 0);

	//
	if (success != LOGSVRCONFIG.load_config())
	{
		TRACESVR(log_mask_system_error, "[CLogServerConfig::load_config] load config file failed!\n", __FUNCTION__);
		return fail;
	}

	g_pstCSChannelShm = create_shm_memory((key_t)LOGSVRCONFIG.m_iCSShareMemoryKey, CCodeQueue::size(max_cs_channel_size));
	if (NULL == g_pstCSChannelShm)
	{
		TRACESVR(log_mask_system_error, "create c-->s share memory failed!\n");
		return fail;
	}

	g_pstSCChannelShm = create_shm_memory((key_t)LOGSVRCONFIG.m_iSCShareMemoryKey, CCodeQueue::size(max_cs_channel_size));
	if (NULL == g_pstSCChannelShm)
	{
		TRACESVR(log_mask_system_error, "create s--> share memory failed!\n");
		return fail;
	}

	g_pstUDPCSChannelShm = create_shm_memory((key_t)LOGSVRCONFIG.m_iUDPCSShareMemoryKey, CCodeQueue::size(max_udp_channel_size));
	if (NULL == g_pstUDPCSChannelShm)
	{
		TRACESVR(log_mask_system_error, "create c-->s share memory failed!\n");
		return fail;
	}

	g_pstUDPSCChannelShm = create_shm_memory((key_t)LOGSVRCONFIG.m_iUDPSCShareMemoryKey, CCodeQueue::size(max_udp_channel_size));
	if (NULL == g_pstUDPSCChannelShm)
	{
		TRACESVR(log_mask_system_error, "create s--> share memory failed!\n");
		return fail;
	}

	size_t core_size = CLogicFrame::size();
	g_pstCoreShm = create_shm_memory((key_t)LOGSVRCONFIG.m_iCoreKey, core_size);
	if (NULL == g_pstCoreShm)
	{
		TRACESVR(log_mask_system_error, "create core share memory failed!\n");
		return fail;
	}

	CLogicFrame::set_memory(g_pstCoreShm);

	return success;
}


int32_t check_run_dir()
{
	DIR* pRunDir = NULL;

	pRunDir = opendir("./log");
	if (NULL == pRunDir)
	{
		return fail;
	}
	else
	{
		closedir(pRunDir);
	}

	pRunDir = opendir("./config");
	if (NULL == pRunDir)
	{
		return fail;
	}
	else
	{
		closedir(pRunDir);
	}

	return success;
}
void *thread_main(void *arg);

int32_t main(int argc, char** argv)
{
	int32_t init_flag = 0;
	bool is_daemon = false;

	if (argc > 1)
	{
		if (0 == strcasecmp((const char*)argv[1], "init"))
		{
			init_flag = 1;
		}

		if (0 == strcasecmp((const char*)argv[1], "-d"))
		{
			is_daemon = true;
		}

		if (0 == strcasecmp((const char*)argv[1], "-v"))
		{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)

			printf("Chat Server Debug Version %s %s %s\n", SERVER_VERSION, __DATE__, __TIME__);
#else

			printf("Hall Server Release Version %s %s %s\n", SERVER_VERSION, __DATE__, __TIME__);
#endif

			return success;
		}
	}

	int lock_fd = open((const char *)argv[0], O_RDONLY);
	if(lock_fd < 0)
	{
		exit(1);
	}

	if(flock(lock_fd, LOCK_EX | LOCK_NB) < 0)
	{
		printf("Chat Server was launched!\n");
		exit(1);
	}

	if (success != check_run_dir())
	{
		printf("check run directory failed!\n");
		exit(1);
	}

	if (is_daemon)
	{
		InitDaemon(1, 0);
	}

	if (success != initialize(init_flag))
	{
		printf("Log Server fail to launch: initialize failed!\n");
		exit(1);
	}

	if (success != LOGICFRAME.to_working())
	{
		printf("Log Server fail to launch: LOGICFRAME.to_working failed!\n");
		exit(1);
	}

	printf("Log Server is working... \n");

	//int32_t iThreadCount = max_stat_thread_count + 1;
	int32_t iThreadCount = LOGSVRCONFIG.m_iStatThreadCount + 1;
	Thread *tptr = new Thread[iThreadCount];

	for (int32_t i=0; i< iThreadCount ; ++i)
	{
		pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void*)i);
	}

	for (int32_t i=0; i< iThreadCount ; ++i)
	{
		pthread_join(tptr[i].thread_tid, NULL);
	}

	//LOGICFRAME.working();

	TRACESVR(log_mask_system_error, "[%s] Log server exit normally!\n", __FUNCTION__);

	return success;
}
void *thread_main(void *arg)
{


	int index = (u_int64_t)arg;
	printf("thread [%d] starting...\n", index);

	if ( LOGSVRCONFIG.m_iStatThreadCount > index)
	{
		LOGICFRAME.m_szStateThreads[index].set_index(index);
		TRACESVR("Log Server Stat Thread:  %d started............\n",index);
		LOGICFRAME.m_szStateThreads[index].run(index);
	}
	else
	{
		TRACESVR("Log Server Main thread %d started............\n",index);
		LOGICFRAME.working();
		

	}
	return NULL;

}
