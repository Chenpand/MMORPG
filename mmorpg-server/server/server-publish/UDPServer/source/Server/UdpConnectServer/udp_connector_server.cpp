#include <sys/resource.h>
#include "Common/common_flag.h"
#include "Common/common_base.h"
#include "Common/common_memory.h"
#include "Common/common_codequeue.h"
#include "Common/common_config.h"

#include "udp_connector_define.h"
#include "udp_connector_frame.h"
#include "udp_connector_log.h"

using namespace KingNet;
using namespace KingNet::Server;
using namespace KingNet::Server::UdpConnectorServer;

int32_t initialize(int32_t mode = 1);

void sigusr1_handle( int32_t iSigVal );
void sigusr2_handle( int32_t iSigVal );

CUdpConnFrame *g_pFrame;

#define SERVER_VERSION  "CB8"

void sigusr1_handle( int32_t iSigVal )
{
	g_byRunFlag.set(run_flag_reload );
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle( int32_t iSigVal )
{
	g_byRunFlag.set(run_flag_exit);
	signal(SIGUSR2, sigusr2_handle);
}

int32_t load_config()
{
	CConfigFile stConfigFile;

	if (0 != stConfigFile.open_file(CONFIG_FILE))
	{
		fprintf(stdout, "open config file :%s failed\n", CONFIG_FILE);
		return fail;
	}

	memset(&g_stSvrConfig, 0, sizeof(g_stSvrConfig));

	stConfigFile.get_value_of("udpconnectorsvrd", "core_key", g_stSvrConfig.m_iCoreKey, 512301);
	stConfigFile.get_value_of("udpconnectorsvrd", "cs_channel_key", g_stSvrConfig.m_iCSChannelKey, 512402);
	stConfigFile.get_value_of("udpconnectorsvrd", "sc_channel_key", g_stSvrConfig.m_iSCChannelKey, 512403);

	stConfigFile.get_value_of("udpconnectorsvrd", "log_level", g_stSvrConfig.m_iLogLevel, log_mask_everything);
	
	stConfigFile.close_file();

	return success;
}

int32_t set_openfile_limit(int32_t filecount)
{
	struct rlimit rlim;

	rlim.rlim_cur = rlim.rlim_max = filecount;
	if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		TRACESVR(log_mask_system_error, "[%s] setrlimit failed!, error = %s\n",
			__FUNCTION__, strerror(errno));
		return fail;
	}

	rlim.rlim_cur = rlim.rlim_max = 0;
	if (getrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		TRACESVR(log_mask_system_error, "[%s] getrlimit failed!, error = %s\n",
			__FUNCTION__, strerror(errno));
		return fail;
	}

	TRACESVR(log_mask_detail, "[%s] setlimit successfully! now max open file = %d\n",
		__FUNCTION__, rlim.rlim_cur);

	return success;
}

int32_t initialize(int32_t mode)
{
	int32_t ret = success;

	//安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);

	g_byRunFlag.initialize(0);

	//load config for key
	if (success != load_config())
	{
		//TRACESVR("[%s] load_config failed!\n", __FUNCTION__);
		fprintf(stdout, "[%s] load_config failed!\n", __FUNCTION__);
		return fail;
	}

	SETTRACELEVEL(g_stSvrConfig.m_iLogLevel);//设置日志等级/掩码

#ifdef __OPEN_LIMIT__

	if (success != set_openfile_limit((max_socket_count + 200)))
	{
		return fail;
	}
#endif

	g_pstCSChannelShm = create_shm_memory((key_t)g_stSvrConfig.m_iCSChannelKey, CCodeQueue::size(max_udp_channel_size), mode);
	if (NULL == g_pstCSChannelShm)
	{
		TRACESVR("[%s] create_shm_memory for key=%d failed\n", __FUNCTION__, g_stSvrConfig.m_iCSChannelKey);
		return fail;
	}

	g_pstSCChannelShm = create_shm_memory((key_t)g_stSvrConfig.m_iSCChannelKey, CCodeQueue::size(max_udp_channel_size), mode);
	if (NULL == g_pstSCChannelShm)
	{
		TRACESVR("[%s] create_shm_memory for key=%d failed\n", __FUNCTION__, g_stSvrConfig.m_iSCChannelKey);
		return fail;
	}

	int32_t iCoreSize = CUdpConnFrame::size();
	g_pstCoreShm = create_shm_memory((key_t)g_stSvrConfig.m_iCoreKey, iCoreSize, mode);
	if (NULL == g_pstCoreShm)
	{
		TRACESVR("[%s] create_shm_memory for key=%d, size = %d failed\n", __FUNCTION__, g_stSvrConfig.m_iCoreKey, iCoreSize);
		return fail;
	}

	CUdpConnFrame::set_meory(g_pstCoreShm);

	g_pFrame = new CUdpConnFrame;
	if (NULL == g_pFrame)
	{
		destroy_shm_memory((key_t)g_stSvrConfig.m_iCoreKey);
		exit(-1);
	}


	return ret;
}


int main( int argc, char **argv )
{
	bool is_daemon = false;

	if(1 < argc && !strcasecmp(argv[1], "-d" ) )
	{
		is_daemon = true;
	}
	else
	{
		is_daemon = false;
	}

	if (argc > 1 && !strcasecmp(argv[1], "-v"))
	{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || (__DEBUG__)
		fprintf(stdout, "udpconnector server debug  version %s %s %s\n", SERVER_VERSION, __DATE__, __TIME__);
#else
		fprintf(stdout, "udpconnector server release  version %s %s %s\n", SERVER_VERSION, __DATE__, __TIME__);
#endif
		return 0;
	}

	int lock_fd = open((const char *)argv[0], O_RDONLY);
	if(lock_fd < 0 )
	{
		exit(1);
	}

	if( flock(lock_fd, LOCK_EX | LOCK_NB) < 0 )
	{
		printf("UdpConnector Server was lauched!\n");
		exit(1);
	}


	if (is_daemon)
	{
		InitDaemon(1,0);
	}

	int32_t ret = 0;
	int32_t mode = 0;

	ret = initialize(mode);
	if(success != ret)
	{
		printf("UdpConnector Server fail to launch!\n");
		exit(0);
	}

	fprintf(stdout, "UdpConnector is going to launch ...\n");
	ret = g_pFrame->initialize();
	if( success != ret )
	{
		fprintf(stdout, "UdpConnector launch failed!\n");
		exit(0);
	}

	fprintf(stdout, "UdpConnector is working ...\n");
	TRACESVR(log_mask_system_config, "UdpConnector is working ...\n");
	g_pFrame->working();

	TRACESVR(" UdpConnector server exit now ...\n");

	return 0;
}

