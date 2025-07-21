#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "gameplatform_message.h"

#include "Common/common_base.h"
#include "Common/common_codequeue.h"
#include "Common/common_string.h"
#include "Common/common_socket.h"
#include "Common/common_flag.h"
#include "Common/Crypt.h"

#include "public/server_public_define.h"

#include "Connector/web_connector_head.h"
#include "Connector/udp_connector_head.h"
#include "tinyxml/tinyxml.h"

#include "log_logicframe.h"
#include "log_log.h"
#include "log_common_define.h"
#include "log_message.h"
#include "public/server_log_message_body.h"
#include "log_config.h"



namespace KingNet{ namespace Server { namespace LogServer {

	CFlag		g_byRunFlag;
	CShmMemory* g_pstCoreShm      = NULL;
	CShmMemory* g_pstCSChannelShm = NULL;
	CShmMemory* g_pstSCChannelShm = NULL;
	CShmMemory* g_pstUDPCSChannelShm = NULL;
	CShmMemory* g_pstUDPSCChannelShm = NULL;
	CShmMemory* CLogicFrame::m_pstMemory = NULL;


	char		g_szSSKey[auth_service_key_length] = {'B', 'e', 's', 't', 'G', 'a', 'm', 'e', '@', 'K', 'i', 'n', 'g', 'N', 'e', 't'};

	size_t CLogicFrame::size()
	{
		size_t total = 0;

		//CLogicFrame
		total += sizeof(CLogicFrame);
		TRACESVR(log_mask_detail, "[CLogicFrame::%s] CLogicFrame size is %d.\n", __FUNCTION__, total);

		//message entry
		total += CObjectPool::size(LOGSVRCONFIG.m_iMaxMessageEntryCount, sizeof(CMessageEntry));
		TRACESVR(log_mask_detail, "[CLogicFrame::%s] CLogicFrame + CHandler + CTimer + CMessageEntry size is %d.\n", __FUNCTION__, total);

		//message body queue
		total += CCodeQueue::size(LOGSVRCONFIG.m_iMaxMessageBodyQueueSize);
		TRACESVR(log_mask_detail, "[CLogicFrame::%s] CLogicFrame's total size is %d\n", __FUNCTION__, total);

		return total;
	}

	void CLogicFrame::set_memory(CShmMemory* memory)
	{
		m_pstMemory = memory;
	}

	void* CLogicFrame::operator new(size_t size)throw()
	{
		if (NULL == m_pstMemory)
		{
			return NULL;
		}

		char* ptmp = (char*)m_pstMemory->create_segment(size);

		return (void*)ptmp;
	}

	void CLogicFrame::operator delete(void *pszMemory)
	{
		//do nothing
		return;
	}

	CLogicFrame::CLogicFrame()
	{
		if (NULL == m_pstMemory)
		{
			return;
		}

		int32_t max_stat_thread_count = LOGSVRCONFIG.m_iStatThreadCount;
		m_szStateThreads.reserve(max_stat_thread_count);
		m_szSocketPipes.reserve(max_stat_thread_count);
		m_systemLog.reserve(max_stat_thread_count);

		CObjectPool::set_memory(m_pstMemory);
	
		//消息体对象池
		m_pstMessageEntryPool = new CObjectPool(LOGSVRCONFIG.m_iMaxMessageEntryCount, sizeof(CMessageEntry), CMessageEntry::create_one_object);
		if(NULL == m_pstMessageEntryPool)
		{
			TRACESVR(log_mask_system_error,"[CLogicFrame::Initialize] create message entry pool fail!\n");
		}
		//主消息队列对应的消息体队列
		CCodeQueue::set_sharedmemory(m_pstMemory);
		m_pstMessageBody = new CCodeQueue(LOGSVRCONFIG.m_iMaxMessageBodyQueueSize);

		CCodeQueue::set_sharedmemory(g_pstCSChannelShm);
		m_pstC2SChannel = new CCodeQueue(max_cs_channel_size);

		CCodeQueue::set_sharedmemory(g_pstSCChannelShm);
		m_pstS2CChannel = new CCodeQueue(max_cs_channel_size);

		CCodeQueue::set_sharedmemory(g_pstUDPCSChannelShm);
		m_pstUdpC2SChannel = new CCodeQueue(max_cs_channel_size);

		CCodeQueue::set_sharedmemory(g_pstUDPSCChannelShm);
		m_pstUdpS2CChannel = new CCodeQueue(max_cs_channel_size);

		if (CShmMemory::shm_mode_init == m_pstMemory->get_mode())
		{
			initialize();
		}
		else
		{
			resume();
		}

		m_tmLastCheckShm	 = time(NULL);

		//生成交互管道...
		for(int32_t i=0;i<max_stat_thread_count;++i)
		{
			int32_t aiSocketPair[2];
			if( socketpair(AF_LOCAL, SOCK_STREAM, 0, aiSocketPair) )
			{
				TRACESVR(log_mask_system_error,"[CLogicFrame::Initialize] create local pair failed of %d, %s.\n", errno, strerror(errno));
				return;
			}
			set_socket_buffer_size(aiSocketPair[0], NET_BUFLEN);
			set_socket_buffer_size(aiSocketPair[1], NET_BUFLEN);
			m_szSocketPipes[i].SetUpStream(aiSocketPair[0]);

			int ret = 0;
			ret = m_szStateThreads[i].initialize(aiSocketPair[1]);
			if( success != ret )
			{
				TRACESVR("[CLogicFrame::%s] CLogicFrame initialize failed \n",__FUNCTION__);
				return;
			}
		}
		for(int32_t i=0;i<max_stat_thread_count;++i)
		{
			m_systemLog[i].SetLogLevel(LOGSVRCONFIG.m_iLogMask);
		}

	}

	CLogicFrame::~CLogicFrame()
	{
		//do nothing
	}
	
	
	int32_t CLogicFrame::initialize()
	{
		//initialize the core message queue
		m_stMessage.initialize(m_pstMessageEntryPool, 0);

		m_nRequestSequence = 0;

		return success;
	}
	
	int32_t CLogicFrame::send_data_to_thread(int32_t uid,const char *buf, int32_t iLength )
	{
		int32_t iTempRet = 0;

		char  abyCode[10240];
		size_t nProcess = (size_t)uid & (LOGSVRCONFIG.m_iStatThreadCount - 1);

		if( NULL == buf || 0 >= iLength || m_szSocketPipes[nProcess].GetSocketFD() <= 0 )
		{
			TRACESVR(log_mask_system_error,"[CLogicFrame::%s]get socket fd failed,log=%p,m_SocketPipe[%d].GetSocketFD=%d\n",__FUNCTION__,buf,nProcess,m_szSocketPipes[nProcess].GetSocketFD());
			return fail;
		}

		memcpy((void *)&abyCode[sizeof(int32_t)], (const void *)buf, iLength);
		iLength += sizeof(int32_t);
		*(int32_t *)abyCode = htonl(iLength);
		iTempRet = m_szSocketPipes[nProcess].SendOneCode(iLength, abyCode);
		if( iTempRet != success )
		{
			TRACESVR(log_mask_system_error,"[CLogicFrame::%s]Send one code failed,log=%s,iTempRet=%d\n",__FUNCTION__,buf,iTempRet);
			return fail;
		}
		else
		{
			TRACESVR(log_mask_detail,"[CLogicFrame::%s]Successfully send log to write.\n",__FUNCTION__);
		}
		//test 发送以后直接接收数据,测试是否能够接收
		//m_StatThread.receive_data();
		return success;
	}
	int32_t CLogicFrame::resume()
	{
		m_stMessage.resume(m_pstMessageEntryPool);
		return success;
	}


	int32_t CLogicFrame::to_working()
	{
		
		if (CShmMemory::shm_mode_init != m_pstMemory->get_mode())
		{
			TRACESVR(log_mask_detail, "[CLogicFrame::%s] share memory mode(%d).\n", __FUNCTION__, m_pstMemory->get_mode());
			return success;
		}

		TRACESVR(log_mask_detail, "[CLogicFrame::%s] success.\n", __FUNCTION__);

		return success;
	}


	int32_t CLogicFrame::working()
	{
		TRACESVR(log_mask_detail, "[CLogicFrame::%s] Chat Server is working... \n", __FUNCTION__);

		while(true)
		{
			receive_message();
			check_timers();
			//dispatch_message();

			if (check_flag() != success)
			{
				return fail;
			}
		}

		return success;
	}

	int32_t CLogicFrame::encode_message_to_clientcode(char* pszOut, int32_t& iOutLength, CMessageHead& head, 
		const char* pszBody, const int32_t iBodyLength)
	{
		// 打包后的数据: NetHead + CSHead + Body

		if (NULL == pszOut || (NULL != pszBody && iBodyLength <= 0))
		{
			return fail;
		}

		if (head.m_bDestEntityType != entity_type_client && head.m_bDestEntityType != entity_type_connector) //如果不是发往客户端的消息，则错误
		{
			return fail;
		}

		//目前,必然为head_type_force_close_connection
		if (entity_type_connector == head.m_bDestEntityType) 
		{
			TRACESVR(log_mask_system_performance, "[CLogicFrame::%s] notify client closed [IP:%d,PORT:%d,INDEX:%d]\n", __FUNCTION__,
				head.m_iMessageSequence,
				head.m_iMessageID,
				head.m_iDestID);

			CWebNetHead stNetHead;
			stNetHead.m_cControlType        = head_type_force_close_connection;
			stNetHead.m_unConnectionIndex   = (uint16_t)head.m_iDestID;
			stNetHead.m_iPackageSize        = stNetHead.size();					//记录网络头的长度
			stNetHead.m_iUniqueID			= head.m_iSourceID;
			stNetHead.m_stControlInfo.m_stDisconnectInfo.m_cDisconnectType = enmForceCloseType_Delay;
			//此处CMessageHead::m_iMessageSequence 保存的是ipAddr
			stNetHead.m_stControlInfo.m_stDisconnectInfo.m_iSourceIP = head.m_iMessageSequence;   //
			//此处CMessageHead::m_iMessageID保存的是port
			stNetHead.m_stControlInfo.m_stDisconnectInfo.m_nSourcePort = head.m_iMessageID;

			//编码控制块(网络头)，没有协议体
			if (success != stNetHead.encode(pszOut, iOutLength))
			{
				return fail;
			}

			return success;
		}
		return fail;
	}



	int32_t CLogicFrame::decode_udp_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength)
	{
		if (NULL == pszInCode || 0 >= iInCodeLength )
		{
			return fail;
		}

		char* pszTmp = (char*) pszInCode;
		int32_t iTmpSize = iInCodeLength;


		CUdpNetHead stNetHead;
		int16_t nethead_length = 0;

		if (success != stNetHead.decode((const char*) pszTmp, (const int32_t) iTmpSize, &nethead_length) )
		{
			TRACESVR(log_mask_detail, "[CLogicFrame:%s] CUDPNetHead::decode failed!\n", __FUNCTION__);
			return fail;
		}

		pszTmp += nethead_length;
		iTmpSize -= nethead_length;
		int32_t uid = get_uid_from_code((const char*)pszTmp, (const int32_t)iTmpSize);
		int32_t result = send_data_to_thread(uid,(const char*)pszTmp, iTmpSize);
		if (success != result)
		{
				TRACESVR(log_mask_system_error, "in [CLogicFrame::%s] post_message failed!\n", __FUNCTION__);
		}
		return success;
	}

	int32_t CLogicFrame::decode_tcp_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength)
	{
		if (NULL == pszInCode || 0 >= iInCodeLength  )
		{
			return fail;
		}

		char* pszTmp = (char*) pszInCode;
		int32_t iTmpSize = iInCodeLength;

		CWebNetHead stNetHead;
		int16_t nethead_length = 0;


		if (success != stNetHead.decode((const char*) pszTmp, (const int32_t) iTmpSize, &nethead_length) )
		{
			TRACESVR(log_mask_detail, "[CLogicFrame:%s] CWebNetHead::decode failed!\n", __FUNCTION__);
			notify_to_close_connection(stNetHead.m_unConnectionIndex,stNetHead.m_iUniqueID, stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP, stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
			return fail;
		}

		if (stNetHead.m_cControlType == head_type_disconnect_notify) //client断线通知消息
		{
			TRACESVR(log_mask_detail, "[CLogicFrame::%s] receive msg that client[%s] was closed\n",
				__FUNCTION__, inet_ntoa(*(struct in_addr*)&stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP));
			return success;
		}

		pszTmp += nethead_length;
		iTmpSize -= nethead_length;

		//CCSHead stCSHead;
		////int32_t head_length = 0;
		//int16_t shCSHeadLen = 0;
		//if (success != stCSHead.decode((const char*)pszTmp, iTmpSize,&shCSHeadLen))
		//{
		//	TRACESVR(log_mask_detail, "[CLogicFrame::%s] CCSHead decode failed\n", __FUNCTION__);
		//	notify_to_close_connection(stNetHead.m_unConnectionIndex,stNetHead.m_iUniqueID, stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP, stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
		//	return fail;
		//}

		////解包出来的协议包长度和传入的长度不等
		//if (iTmpSize < stCSHead.m_stHead.nPackageLength)
		//{
		//	TRACESVR(log_mask_detail, "[CLogicFrame::%s] CCSHead.nPackageLength invalid, stCSHead.m_stHead.nPackageLength= %d, in_size = %D\n", 
		//		__FUNCTION__, stCSHead.m_stHead.nPackageLength, iTmpSize);
		//	return fail;
		//}

		//pszTmp += shCSHeadLen;
		//iTmpSize -= shCSHeadLen;


		int32_t uid = get_uid_from_code((const char*)pszTmp, (const int32_t)iTmpSize);
		int32_t result = send_data_to_thread(uid,(const char*)pszTmp, iTmpSize);
		if (success != result)
		{
			TRACESVR(log_mask_system_error, "in [CLogicFrame::%s] post_message failed!\n", __FUNCTION__);
		}
		notify_to_close_connection(stNetHead.m_unConnectionIndex,stNetHead.m_iUniqueID,stNetHead.m_stControlInfo.m_stSourceIPInfo.m_iSourceIP, stNetHead.m_stControlInfo.m_stSourceIPInfo.m_nSourcePort);
		return success;
	}



	int32_t CLogicFrame::send_message_to_client(CMessageHead* head, const char* body, const int32_t length)
	{
		static char szPackage[max_cs_package_total_size] = {0};
		int32_t iPackageSize = 0;
		
		iPackageSize = (int32_t)sizeof(szPackage);

		if (success != encode_message_to_clientcode(szPackage, iPackageSize, *head, body, length))
		{
			TRACESVR(log_mask_system_warning, "in [%s] encode_message_to_clientcode of message id:%04d, type:%02d, iSequence:%04d, body size:%04d failed, so discard!\n",
				__FUNCTION__,head->m_iMessageID, head->m_nMessageType, head->m_iMessageSequence, length);

			return fail;
		}

		if (success != m_pstS2CChannel->append((const char*)&szPackage[0], iPackageSize))
		{
			TRACESVR(log_mask_system_error,"in [%s] append package of message id:0x%x, type:%02d, iSequence:%04d, package size:%04d failed, so discard!\n",
				__FUNCTION__, head->m_iMessageID, head->m_nMessageType, head->m_iMessageSequence, iPackageSize);

			TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] S --> C channel is : %d.\n", __FUNCTION__, m_pstS2CChannel->full());

			return fail;
		}

		TRACESVR(log_mask_detail, "in [%s] send package of message id:0x%x, type:%02d, iSequence:%04d, package size:%04d success. and content = \n", 
			__FUNCTION__, head->m_iMessageID, head->m_nMessageType, head->m_iMessageSequence, iPackageSize);

		TRACEBIN(log_mask_detail, szPackage, iPackageSize);

		return success;
	}

	int32_t CLogicFrame::receive_message()
	{
		int32_t iPackageCount = 0;

		iPackageCount += receive_tcp_client_message();
		iPackageCount += receive_udp_client_message();
		//iPackageCount += receive_server_message();

		return iPackageCount;

	}


	int32_t CLogicFrame::check_timers()
	{
		struct timeval tvNowTime;
		gettimeofday(&tvNowTime, NULL);

		int32_t iInterval = (tvNowTime.tv_sec - m_tvLastCheckTime.tv_sec)*1000 + (tvNowTime.tv_usec - m_tvLastCheckTime.tv_usec)/1000;

		//系统时钟有问题
		if (iInterval < 0) 
		{
			iInterval = 0x7FFFFFFF;
		}

		if (iInterval < 100) //至少100ms的检测间隔
		{
			return success;
		}

		//获取当天的日期
		get_current_struct_datetime(m_stDatetime);

		//更新最近一次的检测时间
		m_tvLastCheckTime.tv_sec  = tvNowTime.tv_sec;
		m_tvLastCheckTime.tv_usec = tvNowTime.tv_usec;


		//更新共享内存的时间戳
		time_t tmNow = time(NULL);
		if ((tmNow - m_tmLastCheckShm) >= check_share_memory_interval)
		{
			m_tmLastCheckShm = tmNow;
			g_pstCoreShm->set_time_stamp();
			g_pstSCChannelShm->set_time_stamp();
			g_pstCSChannelShm->set_time_stamp();
			g_pstUDPSCChannelShm->set_time_stamp();
			g_pstUDPCSChannelShm->set_time_stamp();
		}

		struct timeval stTimeValue;
		stTimeValue.tv_sec  = 0;
		stTimeValue.tv_usec = 100000;
		select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &stTimeValue);
		return success;
	}

	int32_t CLogicFrame::check_flag()
	{
		if (g_byRunFlag.is_setted(run_flag_reload))
		{
			g_byRunFlag.unset(run_flag_reload);

			TRACESVR(log_mask_detail, "[CLogicFrame::%s] receive reload signal, reload...\n", __FUNCTION__);

			int32_t result = fail;

			//重新加载服务器配置
			result = LOGSVRCONFIG.reload_config();
			if (result != success)
			{
				TRACESVR(log_mask_system_error, "[CLogicFrame::%s] reload global config file failed!\n", __FUNCTION__);
				return fail;
			}
			for(int32_t i=0;i<LOGSVRCONFIG.m_iStatThreadCount;++i)
			{
				m_systemLog[i].SetLogLevel(LOGSVRCONFIG.m_iLogMask);
				m_systemLog[i].WriteThreadLog(i,"Reload config success!\n");
			}
			TRACESVR(log_mask_detail, "[CLogicFrame::%s] reload config success.\n", __FUNCTION__);
		}

		if (g_byRunFlag.is_setted(run_flag_exit))
		{
			TRACESVR(log_mask_system_error, "[CLogicFrame::%s] receive exit signal, so exit ...\n", __FUNCTION__);
			exit(0);
		}

		return success;
	}
	int32_t	CLogicFrame::get_uid_from_code(const char* pszCode, const int32_t iCodeSize)
	{
		if (iCodeSize <= int32_t(MIN_CSPackage_Header_Length))
		{
			return 0; //
		}

		const int16_t uid_offset_in_cs_head = 4;

		int32_t uid = *(int32_t*)(pszCode + uid_offset_in_cs_head);

		return (int32_t)ntohl((uint32_t)uid);
	}

	int32_t CLogicFrame::receive_tcp_client_message()
	{
		int32_t iPackageCount = 0;

		if (NULL == m_pstC2SChannel)
		{
			return 0;
		}

		if (m_pstC2SChannel->full())
		{
			TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] C --> S channel is full!\n.", __FUNCTION__);
		}

		char szClientCode[max_client_pkg_size];
		int32_t iCodeSize = 0;

		CMessageHead stMessageHead;

		//char szMessageBody[max_client_pkg_size];
		//int32_t iBodySize = 0;
		//char* pszBody = NULL;

		do 
		{
			if (m_pstMessageEntryPool->get_free_count() <= 128)
			{ 
				//主控消息队列已满
				TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] main message queue is full!\n", __FUNCTION__);
				break;
			}
			iCodeSize = (int32_t) sizeof(szClientCode);
			if (success != m_pstC2SChannel->pop(szClientCode, iCodeSize))
			{
				break;
			}

			if (iCodeSize <= 0)
			{
				break;
			}

			//iBodySize = (int32_t)sizeof(szMessageBody);
			//pszBody = &szMessageBody[0];

			TRACEBIN(log_mask_detail, szClientCode, iCodeSize);
		
			memset(&stMessageHead,0,sizeof(stMessageHead));
			if (success != decode_tcp_clientcode_to_message((const char*)&szClientCode[0], (const int32_t) iCodeSize))
			{
				TRACESVR(log_mask_system_warning, "in [CLogicFrame::%s], decode_tcp_clientcode_to_message failed!\n", __FUNCTION__);
				continue;
			}


			TRACESVR(log_mask_detail, "in [CLogicFrame::%s], receive message id:0x%X, type:%d, iSequence:%d\n", 
				__FUNCTION__, stMessageHead.m_iMessageID, stMessageHead.m_nMessageType, stMessageHead.m_iMessageSequence);
			//if( stMessageHead.m_iMessageID == CS_MSG_NOTIFY_WRITE_LOG&& stMessageHead.m_nMessageType == message_type_notify)
			//{
			//	result = send_data_to_thread(uid,(const char*)&szMessageBody[0], iBodySize);

			//	if (success != result)
			//	{
			//		TRACESVR(log_mask_system_error, "in [CLogicFrame::%s] post_message failed!\n", __FUNCTION__);
			//	}
			//	
			//}
			

			iPackageCount++;

		} while (iCodeSize > 0);

		//TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] package count : %d.\n.", __FUNCTION__, iPackageCount);

		return iPackageCount;
	}

	int32_t CLogicFrame::receive_udp_client_message()
	{
		int32_t iPackageCount = 0;

		if (NULL == m_pstUdpC2SChannel)
		{
			return 0;
		}

		if (m_pstUdpC2SChannel->full())
		{
			TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] C --> S channel is full!\n.", __FUNCTION__);
		}

		char szClientCode[max_client_pkg_size];
		int32_t iCodeSize = 0;

		CMessageHead stMessageHead;

		//char szMessageBody[max_client_pkg_size];
		//int32_t iBodySize = 0;
		//char* pszBody = NULL;

		do 
		{
			if (m_pstMessageEntryPool->get_free_count() <= 128)
			{ 
				//主控消息队列已满
				TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] main message queue is full!\n", __FUNCTION__);
				break;
			}
			iCodeSize = (int32_t) sizeof(szClientCode);
			if (success != m_pstUdpC2SChannel->pop(szClientCode, iCodeSize))
			{
				break;
			}

			if (iCodeSize <= 0)
			{
				break;
			}

			//iBodySize = (int32_t)sizeof(szMessageBody);
			//pszBody = &szMessageBody[0];

			TRACEBIN(log_mask_detail, szClientCode, iCodeSize);
			//int32_t uid = get_uid_from_code((const char*)&szClientCode[0], (const int32_t) iCodeSize);
			//int32_t result = send_data_to_thread(uid,(const char*)&szClientCode[0], iCodeSize);
			//if (success != result)
			//{
			//		TRACESVR(log_mask_system_error, "in [CLogicFrame::%s] post_message failed!\n", __FUNCTION__);
			//}
			//int32_t uid=0;
			if (success != decode_udp_clientcode_to_message((const char*)&szClientCode[0], (const int32_t) iCodeSize))
			{
				TRACESVR(log_mask_system_warning, "in [CLogicFrame::%s], decode_udp_clientcode_to_message failed!\n", __FUNCTION__);
				continue;
			}


			TRACESVR(log_mask_detail, "in [CLogicFrame::%s], receive message id:0x%X, type:%d, iSequence:%d\n", 
				__FUNCTION__, stMessageHead.m_iMessageID, stMessageHead.m_nMessageType, stMessageHead.m_iMessageSequence);

			//if( stMessageHead.m_iMessageID == CS_MSG_NOTIFY_WRITE_LOG && stMessageHead.m_nMessageType == message_type_notify)
			//{
			//	result = send_data_to_thread(uid,(const char*)&szMessageBody[0], iBodySize);

			//	if (success != result)
			//	{
			//		TRACESVR(log_mask_system_error, "in [CLogicFrame::%s] post_message failed!\n", __FUNCTION__);
			//	}
			//}

			iPackageCount++;

		} while (iCodeSize > 0);

		//TRACESVR(log_mask_system_warning, "[CLogicFrame::%s] package count : %d.\n.", __FUNCTION__, iPackageCount);

		return iPackageCount;
	}

	int32_t CLogicFrame::notify_to_close_connection( int32_t connection_idx, int32_t iUniqueID, int32_t ipAddr, int16_t port)
	{
		CMessageHead stHead;
		stHead.m_bDestEntityType    = entity_type_connector;
		stHead.m_iDestID            = connection_idx;
		stHead.m_bSourceEntityType  = entity_type_log;
		stHead.m_iSourceID          = iUniqueID;

		stHead.m_nMessageType = MSG_TYPE_OTHER;
		stHead.m_iMessageID   = port;				//借用m_iMessageID转递port地址信息
		stHead.m_iMessageSequence = ipAddr;		//借用m_iMessageSequence传递ipAddr信息

		return send_message_to_client(&stHead, NULL, 0);
	}


	stDateTime CLogicFrame::get_date_time()
	{
		return m_stDatetime;
	}

}}}//namespace KingNet{ namespace Server { namespace LogServer {

