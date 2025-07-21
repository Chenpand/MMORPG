
#include "gameplatform_message.h"

#include "Common/common_codequeue.h"
#include "Common/common_codeengine.h"
#include "Common/common_flag.h"
#include "Common/common_config.h"
#include "Common/common_memory.h"
#include "Common/common_logengine.h"

#include "public/server_message.h"

#include "Connector/udp_connector_head.h"

#include "udp_connector_log.h"
#include "udp_connector_frame.h"

namespace KingNet { namespace Server { namespace UdpConnectorServer {

	CShmMemory* CUdpConnFrame::m_pstShmMemory = NULL; 

	//some global variable
	CShmMemory* g_pstCoreShm = NULL;
	CShmMemory* g_pstCSChannelShm = NULL;
	CShmMemory* g_pstSCChannelShm = NULL;
	
#ifdef __MULTI_THREAD__
	CShmMemory *g_apShmPipeAuxOut[auxiliary_handler_count] = {NULL, NULL};
#endif

	stConnectorConfig g_stSvrConfig;
	CFlag g_byRunFlag;
	//

	void CUdpConnFrame::set_meory(CShmMemory* pstShmMemory)
	{
		m_pstShmMemory = pstShmMemory;
	}

	CUdpConnFrame::CUdpConnFrame()
	{
		//nothing
	}
	
	CUdpConnFrame::~CUdpConnFrame()
	{
		//
	}


	void* CUdpConnFrame::operator new( size_t size ) throw()
	{
		if (NULL == m_pstShmMemory)
		{
			return NULL;
		}

		char* pszTmp = NULL;
		pszTmp = (char*)m_pstShmMemory->create_segment(size);

		return (void*)pszTmp;
	}


	void CUdpConnFrame::operator delete(void* ptr)
	{
		//nothing
		return;
	}

	int32_t CUdpConnFrame::size()
	{
		int32_t iTempSize  =  (int32_t)sizeof(CUdpConnFrame);
		return iTempSize;
	}

	CUdpConnFrame* CUdpConnFrame::spawn()
	{
		return new CUdpConnFrame;
	}

	int32_t CUdpConnFrame::initialize()
	{
		m_stSocketSatistics.m_unSocketCount = 0;

		int32_t ret = load_config();
		if( success != ret)
		{
			return ret;
		}

		for(size_t i = 0; i < sizeof(m_stSocketInfo)/sizeof(m_stSocketInfo[0]); ++i)
		{
			memset(&m_stSocketInfo[i], 0x0, sizeof(m_stSocketInfo[i]));
			m_stSocketInfo[i].m_socket_fd  = socket_fd_invalid;
		}

		//epoll event entity array
		m_pevents = NULL;

		//open udp socket	
		for (int32_t i = 0; i < m_stConfig.m_iOpenedPortCount; ++i)
		{
			ret = open_epoll_socket(m_stConfig.m_unOpenedPort[i]);
			if (success != ret)
			{
				TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] open socket on udp port[%d] failed!\n", 
					__FUNCTION__, m_stConfig.m_unOpenedPort[i]);
				return ret;
			}

			TRACESVR(log_mask_system_config, "[CUdpConnFrame::%s] open socket on udp port[%d] ready!\n", __FUNCTION__, m_stConfig.m_unOpenedPort[i]);

			m_stSocketInfo[m_iSocket].m_socket_fd = m_iSocket;
			m_stSocketInfo[m_iSocket].m_socket_type = enmSocketType_listen;
			m_stSocketInfo[m_iSocket].m_local_port = m_stConfig.m_unOpenedPort[i];
		}

		++m_iSocket;

 		ret = make_channel();
		if(success != ret )
		{
			return fail;
		}

		time(&m_tmLastStatisticTime);

		TRACESVR("[%s] UdpConnector Server initialize successfully!\n", __FUNCTION__);

		return success;
	}

	int32_t CUdpConnFrame::make_channel()
	{
		CCodeQueue::set_sharedmemory(g_pstCSChannelShm);

		m_pstCSChannel = new CCodeQueue(max_cs_channel_size);
		if (NULL == m_pstCSChannel)
		{
			TRACESVR("[%s] new m_pstCSChannel failed!\n", __FUNCTION__);
			return fail;
		}

		CCodeQueue::set_sharedmemory(g_pstSCChannelShm);
		m_pstSCChannel = new CCodeQueue(max_cs_channel_size);
		if (NULL == m_pstSCChannel)
		{
			TRACESVR("[%s] new m_pstSCChannel failed!\n", __FUNCTION__);
			return fail;
		}

#ifdef __MULTI_THREAD__
	
		for (int32_t i = 0; i < auxiliary_handler_count; ++i)
		{
			CCodeQueue::set_sharedmemory(g_apShmPipeAuxOut[i]);
			m_pstACChannel[i] = new CCodeQueue(max_cs_channel_size);
			if (NULL == m_pstACChannel[i])
			{
				TRACESVR("[%s] new m_pstACChannel[%d] failed!\n", __FUNCTION__, i);
				return fail;
			}
		}

#endif

		return success;
	}
// 

	int32_t CUdpConnFrame::working()
	{
		TRACESVR("connector server now is working ...\n");
		for(/**/; /**/;)
		{
			check_flag();
			recv_messages();     
			check_and_send_data(); 
		}

		return 0;
	}

	int32_t CUdpConnFrame::reload_config()
	{
		CConfigFile stConfigFile;

		if (0 != stConfigFile.open_file(CONFIG_FILE))
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] open config file:%s failed\n", __FUNCTION__, CONFIG_FILE);
			return fail;
		}

		//
		stConfigFile.get_value_of("udpconnectorsvrd", "statistic_interval", m_stConfig.m_iLogStatisticInterval, 90);

		//
		stConfigFile.get_value_of("udpconnectorsvrd", "log_level", g_stSvrConfig.m_iLogLevel, log_mask_everything);

		stConfigFile.close_file();

		return success;
	}


	int32_t CUdpConnFrame::load_config()
	{
		
		CConfigFile stConfigFile;

		memset(&m_stConfig, 0, sizeof(stServerConfig));

		if (0 != stConfigFile.open_file(CONFIG_FILE))
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] open config file:%s failed\n", __FUNCTION__, CONFIG_FILE);
			return fail;
		}

		//监听端口配置
		stConfigFile.get_value_of("udpconnectorsvrd", "bind_port_count", m_stConfig.m_iOpenedPortCount, 0);
		if (0 >= m_stConfig.m_iOpenedPortCount || m_stConfig.m_iOpenedPortCount > max_open_port_count)
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] config item [bind_port_count] = %d\n", m_stConfig.m_iOpenedPortCount);
			return fail;
		}

		char szPort[16] = {'\0'};
		int iPort = 0;
		for (int32_t i = 0; i < m_stConfig.m_iOpenedPortCount; ++i)
		{
			sprintf(szPort, "bind_port%d", i+1);
			stConfigFile.get_value_of("udpconnectorsvrd", szPort, iPort, 80);
			m_stConfig.m_unOpenedPort[i] = (uint16_t)iPort;
		}

		//log统计数据的时间间隔
		stConfigFile.get_value_of("udpconnectorsvrd", "statistic_interval", m_stConfig.m_iLogStatisticInterval, 90);

		//socket的缓冲区大小
		stConfigFile.get_value_of("udpconnectorsvrd", "socket_buffer", m_stConfig.m_iSocketBufferSize, 1024000); //1000k

		stConfigFile.close_file();

		return success;
	}

	int CUdpConnFrame::recv_messages()
	{
		int32_t timeout = 1;		//单位:ms
		int fd_event_count = 0;

		fd_event_count = epoll_wait(m_epoll_fd, m_pevents, m_maxfd, timeout);
		if (fd_event_count < 0)
		{
			return fail;
		}

		int sock_fd = -1;
		struct epoll_event *pstEvent = m_pevents;

		for(int i = 0; i < fd_event_count; ++i, pstEvent++)
		{
			sock_fd = pstEvent->data.fd;
			if (sock_fd < 0 || sock_fd >= int(sizeof(m_stSocketInfo)/sizeof(m_stSocketInfo[0])))
			{
				continue;
			}

			m_pstCurrentSocket = &m_stSocketInfo[sock_fd];
			
			if (0 != (EPOLLERR & pstEvent->events))
			{
				TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] socket(%d) epoll error so closed! and clear socket info\n",
					__FUNCTION__, sock_fd);

				continue;
			}
			

	
			recv_client_messages(sock_fd);
		
		}//for


		return success;
	}//

	int CUdpConnFrame::check_and_send_data()
	{
		int32_t iTempRet = success;
		
		int32_t i = 0;
		int32_t data_size = 0;
#ifdef __MULTI_THREAD__
		for(i = 0; i < max_package_count_per_sending/4; i++)
#else
		for(i = 0; i < max_package_count_per_sending; ++i)
#endif
		{
			data_size = (int32_t)sizeof(m_szMsgBuf);
			iTempRet = m_pstSCChannel->pop((char * )m_szMsgBuf, data_size);
			if(success != iTempRet)
			{
				break;
			}

			if(data_size <= 0 )
			{
				break;
			}

			TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] get one code from S2C channel, size = %d, content = \n",
				__FUNCTION__, data_size);

			TRACEBIN(log_mask_package, m_szMsgBuf, data_size);

			//处理从service进程收到的包
			process_code_from_service(m_szMsgBuf, data_size);

		}//for(i = 0; i < max_package_count_per_sending; ++i)


#ifdef __MULTI_THREAD__

		for( int32_t j = 0; j < auxiliary_handler_count; ++j)
		{
			for(i = 0; i < max_package_count_per_sending * 3/4; ++i)
			{
				data_size = max_s2c_package_size;
				iTempRet = m_pstACChannel[j]->pop((char * )m_szMsgBuf, data_size);
				if(success != iTempRet)
				{
					break;
				}

				if(data_size <= 0 )
				{
					break;
				}

				TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] get one code from A2C channel[%d], size = %d, content = \n",
					__FUNCTION__, j, data_size);

				TRACEBIN(log_mask_package, m_szMsgBuf, data_size);

				//处理从service进程收到的包
				process_code_from_service(m_szMsgBuf, data_size);
			}
		}

#endif
		return success;
	}

	int32_t CUdpConnFrame::socket_recv(int fd, char* data, int32_t size, struct sockaddr_in *pAddress)
	{
		if (NULL == data || size <= 0 || NULL == pAddress)
		{
			return -1;
		}

		socklen_t nAddrLen = sizeof(struct sockaddr);
		int32_t recved = 0; 

		recved = recvfrom(fd, data, size, 0, (struct sockaddr *)pAddress, &nAddrLen);
		if (0 < recved)
		{
			return recved;
		}

		return -1;
	}

	//int32_t CUdpConnFrame::process_request_get_ip_port(const char* pszCode, const int32_t iCodeSize)
	//{
	//	if (iCodeSize < int32_t(relay_Header_Length) || iCodeSize > max_relay_pkg_size || NULL == pszCode)
	//	{
	//		return 0; 
	//	}

	//	static char szCodeBuffer[max_relay_pkg_size + 100] = {'\0',};

	//	char *pTmp = &szCodeBuffer[0];
	//	int32_t nCodeLen = iCodeSize;

	//	//先拷贝头部数据
	//	memcpy(pTmp, pszCode, iCodeSize);
	//	pTmp += iCodeSize;

	//	//编码后续的ip,port
	//	*(int32_t *)(pTmp) = m_pstCurrentSocket->m_source_ip;
	//	pTmp += sizeof(int32_t);
	//	nCodeLen += sizeof(int32_t);

	//	*(int16_t *)(pTmp) = m_pstCurrentSocket->m_source_port;
	//	pTmp += sizeof(int16_t);
	//	nCodeLen += sizeof(int16_t);

	//	struct sockaddr_in dstAddr;
	//	dstAddr.sin_family = AF_INET;
	//	dstAddr.sin_addr.s_addr = m_pstCurrentSocket->m_source_ip;
	//	dstAddr.sin_port = m_pstCurrentSocket->m_source_port;

	//	int32_t iSendBytes = socket_send(m_pstCurrentSocket->m_socket_fd, szCodeBuffer, nCodeLen, &dstAddr);
	//	if (iSendBytes == nCodeLen)
	//	{
	//		TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] send data to client[uin:%d] sended = %d bytes!\n", 
	//			__FUNCTION__, m_pstCurrentSocket->m_uin, iSendBytes);

	//		return success;
	//	}
	//	else
	//	{
	//		TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] send error to client[uin:%d] package size = %d, but sended = %d byte!\n",__FUNCTION__, 
	//			m_pstCurrentSocket->m_uin, nCodeLen, iSendBytes);

	//		return fail;
	//	}
	//}

	int32_t CUdpConnFrame::transfer_one_code(int32_t iIndex, const char* pszCode, const int32_t iCodeSize)
	{	
		//
		TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] receive code from [socket idx: %d] size: %d code = \n",
			__FUNCTION__, iIndex, iCodeSize);

		TRACEBIN(log_mask_package, (char*)pszCode, iCodeSize);

		static char szCodeBuffer[max_relay_pkg_size + 100] = {'\0',};
		CUdpNetHead stNetHead;
		int32_t coded_length = 0;

		m_pstCurrentSocket->m_uin = get_uid_from_code(pszCode, iCodeSize);

		if (m_pstCurrentSocket->m_uin <= 0)
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] invalid uid (%d), so discard the package\n",
				__FUNCTION__, m_pstCurrentSocket->m_uin);

			return fail;
		}

		//准备NetHead
		stNetHead.m_unSocketIndex = (uint16_t)iIndex;
		stNetHead.m_cControlType = udp_head_type_client_request;

		if (stNetHead.m_cControlType == udp_head_type_client_request)
		{
			stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP = (int32_t)m_pstCurrentSocket->m_source_ip;
			stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort = (int16_t)m_pstCurrentSocket->m_source_port;
		}

		stNetHead.m_iPackageSize = stNetHead.size() + iCodeSize;

		//编码到预留的Buffer区域
		int32_t nTmpLen = 0;

		stNetHead.encode(szCodeBuffer, nTmpLen);
		coded_length += nTmpLen;

		//拷贝数据部分
		memcpy(&szCodeBuffer[coded_length], pszCode, iCodeSize);
		coded_length += iCodeSize;
	
		//append C2S Buffer
		if (success != m_pstCSChannel->append((const char*)szCodeBuffer, coded_length))
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] append player uin (%d) code to C2S channel failed.\n",
				__FUNCTION__, m_pstCurrentSocket->m_uin);

			//此处是否需要关闭当前的链接呢？

			return fail;
		}

		TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] get player's uid = %d\n", __FUNCTION__, m_pstCurrentSocket->m_uin);

		TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] and  transfer to service, size: %d code = \n",
			__FUNCTION__, coded_length);

		TRACEBIN(log_mask_package, szCodeBuffer, coded_length);
		
		return success;
	}

	void CUdpConnFrame::transfer_code(int32_t iIndex)
	{
		int32_t remained_size = m_pstCurrentSocket->m_recv_bytes;


		char* pPackageStart = &m_pstCurrentSocket->m_package_buffer[0]; //包起始位置
		char* pTmp = pPackageStart;

		if (remained_size <= 0)
		{
			return;
		}

		int32_t iPackageSize = 0;

		while (true)
		{
			if (*pPackageStart != 0)//有HTTP头封装
			{
				if (remained_size < int32_t(http_head_length + sizeof(int32_t)))//若已接收到的数据大小不足一个http协议头以及应用层数据包长度字段(2B)，
					//则继续等待数据. 特别注意: 此处应该根据应用层协议的包长度字段，进行不同的处理
				{
					break;
					//return;
				}

				pTmp = pPackageStart + (http_head_length); //指向http头后的元素
				if (pTmp[-1] !='\n' || pTmp[-2] != '\r' || pTmp[-3] != '\n' || pTmp[-4] !='\r')//http头必须以"\r\n\r\n"结尾
				{
					TRACESVR(log_mask_system_error, "[CConnFrame::%s] package has flag of http head, but the " 
						"head was invalid! so close the socket[%d] ip:port= %s:%d, uin= %d\n", __FUNCTION__, iIndex, 
						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_local_port, m_pstCurrentSocket->m_uin);
					TRACESVR(log_mask_system_error, "the invalid package head = \n");
					TRACEBIN(log_mask_system_error, pPackageStart, http_head_length);

					//clear_current_socket();

					return;
				}

				//去掉http协议头(使用memmove效率可能有问题)
				//memmove(m_pstCurrentSocket->m_szMsgBuf, pTmp, (remained_size - http_head_length));
				//remained_size -= http_head_length;


				//pTmp = &m_pstCurrentSocket->m_szMsgBuf[0];

				iPackageSize = ntohl(*(int32_t*)pTmp); //特别注意: 此处应该根据应用层协议的包长度字段，进行不同的处理

				//假定客户端发来的数据包不超过 max_web_client_pkg_size！！
				if (iPackageSize >= max_web_client_pkg_size || iPackageSize < (int32_t)(MIN_CSPackage_Header_Length))
				{
					TRACESVR(log_mask_system_error, "[CConnFrame::%s] package size was invalid," 
						" so close the socket[%d] ip:port= %s:%d, uin =%d, iPackageSize = %d\n", __FUNCTION__, iIndex, 
						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_local_port, m_pstCurrentSocket->m_uin, iPackageSize);

					//clear_current_socket();

					return;
				}

				//若已收到一个完整的应用层数据包，则进行转发
				if (success == transfer_one_code(iIndex, (const char*)pTmp, (const int32_t)iPackageSize))
				{
					//pTmp += iPackageSize; //移动指针指向下一个待转发的数据包
					//remained_size -= iPackageSize;
					pPackageStart += (http_head_length + iPackageSize);
					remained_size -= (http_head_length + iPackageSize);

					TRACESVR(log_mask_detail, "[CConnFrame::%s] transfer one code from client[%s:%d] to service, size = %d\n",
						__FUNCTION__, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_local_port, iPackageSize);

					//TRACEBIN(log_mask_detail, pTmp, iPackageSize);
				}
				else
				{
					//
					TRACESVR(log_mask_system_error, "[CConnFrame::%s] socket(%d, client ip:%s, uin:%d) transfer_one_code failed!\n", 
						__FUNCTION__, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin);
					//clear_current_socket();

					return;
				}


			}//if (*pTmp != 0)//有HTTP头封装
			else//没有HTTP头封装
			{
				if (remained_size < (int32_t)sizeof(int32_t) + 1) //默认使用2B保存包长度 .特别注意: 此处应该根据应用层协议的包长度字段，进行不同的处理
				{
					//return;		//继续等待数据
					break;
				}

				//得到包的实际长度
				//++pTmp;		//去掉1Byte的标识字段
				pTmp = pPackageStart + 1; //去掉1Byte的标识字段
				//--remained_size; //去掉1Byte的标识字段

				iPackageSize = ntohl(*(int32_t*)pTmp); //特别注意: 此处应该根据应用层协议的包长度字段，进行不同的处理

				//假定客户端发来的数据包不超过 max_web_client_pkg_size！
				if (iPackageSize >= max_web_client_pkg_size || iPackageSize < (int32_t)MIN_CSPackage_Header_Length)
				{
					TRACESVR(log_mask_system_error, "[CConnFrame::%s] package size was invalid,iPackageSize = %d" 
						" so close the socket[%d] ip:port= %s:%d, uin:%d, package head = \n", __FUNCTION__,iPackageSize, iIndex, 
						m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_local_port, m_pstCurrentSocket->m_uin);

					TRACEBIN(log_mask_detail, pTmp, MIN_CSPackage_Header_Length);

					//clear_current_socket();

					return;
				}

				if (remained_size - 1 < iPackageSize)//若接收到的数据还不足以组成一个完整的包，则继续等待数据, -1表示除去1字节的标识符
				{
					// 					if (pTmp != m_pstCurrentSocket->m_szMsgBuf)
					// 					{
					// 						//删除已经处理过的数据，把未处理的数据移到接收缓冲区头部
					// 						memmove(m_pstCurrentSocket->m_szMsgBuf, pTmp, remained_size);
					// 						m_pstCurrentSocket->m_iRecvBytes = remained_size; //
					// 						return;
					// 					}
					break;
				}

				//若已收到一个完整的应用层数据包，则进行转发
				if (success == transfer_one_code(iIndex, (const char*)pTmp, (const int32_t)iPackageSize))
				{
					//pszTmp += iPackageSize; //移动指针指向下一个待转发的数据包
					pPackageStart += (iPackageSize + 1);
					remained_size -= (iPackageSize + 1);
				}
				else
				{
					TRACESVR(log_mask_system_error, "[CConnFrame::%s] socket(%d, client ip:%s, uin:%d) transfer_one_code failed!\n", 
						__FUNCTION__, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_client_address, m_pstCurrentSocket->m_uin);
					//
					//clear_current_socket();
					//TRACESVR(log_mask_system_error, "[CConnFrame::%s] transfer_one_code failed!\n", __FUNCTION__);
					return;
				}


			}//else
		}//end while(true)

		m_pstCurrentSocket->m_recv_bytes = remained_size; //

		//删除已经处理过的数据，把未处理的数据移到接收缓冲区头部
		if (pPackageStart != m_pstCurrentSocket->m_package_buffer && m_pstCurrentSocket->m_recv_bytes > 0)
		{
			memmove(m_pstCurrentSocket->m_package_buffer, pPackageStart, remained_size);
			m_pstCurrentSocket->m_recv_bytes = remained_size; //
			//return;
		}
		//char* pPackageStart = &m_pstCurrentSocket->m_package_buffer[0]; //包起始位置

		//int32_t iPackageSize = m_pstCurrentSocket->m_recv_bytes;

		////收到一个完整的应用层数据包，则进行转发
		//if (success != transfer_one_code(iIndex, (const char*)pPackageStart, (const int32_t)iPackageSize))
		//{
		//	TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] socket(%d, client uin:%d) transfer_one_code failed!\n", 
		//		__FUNCTION__, m_pstCurrentSocket->m_socket_fd, m_pstCurrentSocket->m_uin);

		//	return;
		//}

		////增加收到的数据包个数
		//m_pstCurrentSocket->m_recv_package++;

		////重置接收的数据字节数
		//m_pstCurrentSocket->m_recv_bytes = 0;

		return ;
	}

	int32_t CUdpConnFrame::recv_client_messages(int32_t fd)
	{
		int32_t sock_fd = m_pstCurrentSocket->m_socket_fd; 

		struct sockaddr_in srcAddr;
		char* pszRecvBuffer = m_pstCurrentSocket->m_package_buffer;
		int32_t buffer_size = (int32_t)sizeof(m_pstCurrentSocket->m_package_buffer);

		int32_t iReceived = socket_recv(sock_fd, pszRecvBuffer, buffer_size, &srcAddr);
		if( 0 >= iReceived )
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] receive data error, msg = %s\n",
				__FUNCTION__, strerror(errno));

			//clear_current_socket();	

			return fail;
		}

		m_pstCurrentSocket->m_source_ip = srcAddr.sin_addr.s_addr;
		m_pstCurrentSocket->m_source_port = srcAddr.sin_port;
		m_pstCurrentSocket->m_recv_bytes = iReceived;
	
		TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] receive data size(%d) from client(%d:%d).\n", 
			__FUNCTION__, m_pstCurrentSocket->m_recv_bytes, 
			m_pstCurrentSocket->m_source_ip, m_pstCurrentSocket->m_source_port);

		transfer_code(fd);

		check_and_send_data();

		return success;
	}

	//返回已经发送的字节数
	int32_t CUdpConnFrame::socket_send(int fd, char* data, int32_t size, struct sockaddr_in *pAddress)
	{
		if (NULL == data || size <= 0 || NULL == pAddress)
		{
			return -1;
		}
		
		int32_t nAddrLen = sizeof(struct sockaddr);
		
		int32_t sended  = sendto(fd, data, (size_t)size, 0, (struct sockaddr *)pAddress, nAddrLen);
		if (sended <= 0)
		{
			TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] send data(size:%d) error, msg = %s\n",
				__FUNCTION__, size, strerror(errno));
		}

		return sended;
	} 

	int32_t CUdpConnFrame::get_uid_from_code(const char* pszCode, const int32_t iCodeSize)
	{
		if (iCodeSize < int32_t(upd_Header_Length) || NULL == pszCode)
		{
			return 0; 
		}

		const int16_t src_uid_offset_in_udp_head = 4;

		int32_t uid = *(int32_t*)(pszCode + src_uid_offset_in_udp_head);

		return (int32_t)ntohl((uint32_t)uid);
	}


	int32_t CUdpConnFrame::open_epoll_socket(uint16_t port)
	{
		if (epoll_init() < 0)
		{
			return fail;
		}

		m_iSocket = epoll_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_iSocket < 0)
		{
			//destroy epoll
			epoll_destroy();
			return fail;
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		int ret = 0;
		ret = bind(m_iSocket, (struct sockaddr *) &addr, sizeof(addr));
		if (ret < 0)
		{
			TRACESVR(log_mask_system_error, "[CUdpConnFrame::%s] bind on port :%d error, error msg = %s\n",
				__FUNCTION__, port, strerror(errno));

			epoll_close(m_iSocket);
			epoll_destroy();

			return fail;
		}

		int32_t iOptValue = 0;
		socklen_t iOptLen = 0;

		iOptLen = (socklen_t)sizeof(iOptValue);
		iOptValue = m_stConfig.m_iSocketBufferSize;

		if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptValue, iOptLen))
		{
			return fail;
		}

		if (0 != setsockopt(m_iSocket, SOL_SOCKET, SO_RCVBUF, (const void *)&iOptValue, iOptLen))
		{
			return fail;
		}

		if (epoll_new_socket(m_iSocket) < 0)
		{
			epoll_close(m_iSocket);
			epoll_destroy();
			return fail;
		}
	
		return success;
	}

	int32_t CUdpConnFrame::epoll_init(void)
	{
		if (NULL != m_pevents) //保证events只被创建一次
		{
			return 0;
		}

		m_maxfd = (int)max_socket_count;

		memset(&m_epoll_event, 0, sizeof(m_epoll_event));
		m_epoll_event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		m_epoll_event.data.ptr = NULL;
		m_epoll_event.data.fd  = -1;

		m_pevents = (struct epoll_event *) malloc(m_maxfd * sizeof(struct epoll_event));
		if (NULL == m_pevents)
		{
			return -1;
		}

		m_epoll_fd = epoll_create(m_maxfd);
		if (m_epoll_fd < 0)
		{
			return -1;
		}

		return 0;
	}

	int CUdpConnFrame::epoll_socket(int domain, int type, int protocol)
	{
		int fd = socket(domain, type, protocol);
		if (fd < 0)
		{
			return -1;
		}

		int flags = 1;

		if (ioctl(fd, FIONBIO, &flags) && ((flags = fcntl(fd, F_GETFL, 0)) < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0))
		{
			close(fd);
			return -1;
		}

		return fd;
	}

	int CUdpConnFrame::epoll_new_socket(int fd)
	{
		m_epoll_event.data.fd  = fd;
		if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &m_epoll_event) < 0)
		{
			return -1;
		}

		return 0;
	}

	void CUdpConnFrame::epoll_close(int fd)
	{
		close(fd);
		return;
	}

	void CUdpConnFrame::epoll_destroy(void)
	{
		free(m_pevents);
		close(m_epoll_fd);

		return;
	}

	void CUdpConnFrame::check_flag()
	{
		if (g_byRunFlag.is_setted(run_flag_reload))
		{
			g_byRunFlag.unset(run_flag_reload);
			TRACESVR(log_mask_system_config, "[CUdpConnFrame::%s] to reload config from [%s]\n",
				__FUNCTION__, CONFIG_FILE);

			reload_config();

			//重新设置日志等级
			SETTRACELEVEL(g_stSvrConfig.m_iLogLevel);

			TRACESVR("[CUdpConnFrame::%s] udpconnectorsvrd reload config ...\n!", __FUNCTION__);
			return;
		}

		if (g_byRunFlag.is_setted(run_flag_exit))
		{
			g_byRunFlag.unset(run_flag_exit);
			TRACESVR("[CUdpConnFrame::%s] udpconnectorsvrd exit ...\n!", __FUNCTION__);

			exit(0);

			return;
		}
	}

	void CUdpConnFrame::clear_current_socket()
	{
		if (m_pstCurrentSocket->m_socket_fd > 0)
		{
			m_epoll_event.data.fd = m_pstCurrentSocket->m_socket_fd;
			if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_pstCurrentSocket->m_socket_fd, &m_epoll_event) < 0)
			{
				//oh, my god! , but do nothing here
			}

			close(m_pstCurrentSocket->m_socket_fd);
		}


		//reset field value
		m_pstCurrentSocket->m_socket_fd = socket_fd_invalid;
		m_pstCurrentSocket->m_source_ip = 0;
		m_pstCurrentSocket->m_source_port = 0;

		m_pstCurrentSocket->m_local_port = 0;
		m_pstCurrentSocket->m_recv_bytes  = 0;
		m_pstCurrentSocket->m_recv_package = 0;
		m_pstCurrentSocket->m_socket_type = enmSocketType_invalid;
		m_pstCurrentSocket->m_uin = 0;

		m_pstCurrentSocket->m_create_time = 0;
		m_pstCurrentSocket->m_lastrecv_timestamp = 0;

		return;
	}

	int32_t CUdpConnFrame::process_code_from_service( const char* pszInCode, const int32_t iInCodeSize )
	{		
		int32_t  nIndex = 0;

		if(!pszInCode || iInCodeSize <= 0 )
		{
			return fail;
		}

		CUdpNetHead stNetHead;
		int16_t nNetHeadLength = 0;
		if (success != stNetHead.decode(pszInCode, iInCodeSize, &nNetHeadLength))
		{
			TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] decode net head failed, just discard package, socket index[%d].\n",
				__FUNCTION__, nIndex);

			return fail;
		}

		if (stNetHead.m_iPackageSize != iInCodeSize)
		{
			TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] the package size decoded from service"
				" was not equal with received size, socket index[%d].\n",	__FUNCTION__, nIndex);
			return fail;
		}

		struct sockaddr_in dstAddr;
		int32_t iBodySize = stNetHead.m_iPackageSize - nNetHeadLength;
		char* pTmp = (char*)pszInCode + nNetHeadLength;
		

		nIndex = (int32_t)stNetHead.m_unSocketIndex;

		if(1 > nIndex || max_socket_count <= nIndex)
		{
			TRACESVR( log_mask_system_error, "[CUdpConnFrame::%s] Invalid socket Index %d !",__FUNCTION__, nIndex);
			return fail;
		}

		m_pstCurrentSocket = &m_stSocketInfo[nIndex]; /*得到socket指针*/

		switch (stNetHead.m_cControlType)
		{
		case udp_head_type_service_notify:		//servcie要求的中转包
		{
			dstAddr.sin_family = AF_INET;
			dstAddr.sin_addr.s_addr = stNetHead.m_stControlInfo.m_stDestIPInfo.m_iDestIP;
			dstAddr.sin_port = stNetHead.m_stControlInfo.m_stDestIPInfo.m_nDestPort;

			int32_t iSendBytes = socket_send(m_pstCurrentSocket->m_socket_fd, pTmp, iBodySize, &dstAddr);
			if (iSendBytes == iBodySize)
			{
				TRACESVR(log_mask_detail, "[CUdpConnFrame::%s] send data to client[uin:%d][%d:%d] sended = %d bytes!\n", 
					__FUNCTION__, m_pstCurrentSocket->m_uin, dstAddr.sin_addr.s_addr,
					 dstAddr.sin_port, iSendBytes);

				return success;
			}
			else
			{
				TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] send error to client[uin:%d][%d:%d] package size = %d, "
					" but sended = %d byte!\n",__FUNCTION__, m_pstCurrentSocket->m_uin, dstAddr.sin_addr.s_addr,
					dstAddr.sin_port, iBodySize, iSendBytes);
				
		//		//clear_current_socket();

				return fail;
			}			

			break;
		}

		default:
			TRACESVR(log_mask_system_warning, "[CUdpConnFrame::%s] send invalid type =%d package"
				" to client[uin:%d] on Listening Port:%d,"
				" socket index[%d], so ignore(discard) it!\n", __FUNCTION__, stNetHead.m_cControlType,
				m_pstCurrentSocket->m_uin,
				m_pstCurrentSocket->m_local_port, nIndex);

			break;
		}

		return success;
	}

	void CUdpConnFrame::log_statistics()
	{
		//
		time_t tmNow = time(NULL);
		int32_t interval = tmNow - m_tmLastStatisticTime;
		TRACESVR(log_mask_system_performance, "================ statistics ================\n");
		TRACESVR(log_mask_system_performance, "========= connection(%d) in (%d) seconds =========\n",
			m_stSocketSatistics.m_unSocketCount, interval);

		m_stSocketSatistics.m_unSocketCount = 0;
	}
}}}//namespace KingNet { namespace Server  { namespace ConnectorServer

