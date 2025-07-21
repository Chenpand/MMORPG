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

#include "tinyxml/tinyxml.h"

#include "log_logicframe.h"
#include "log_log.h"
#include "log_common_define.h"
#include "log_message.h"
#include "public/server_log_message_body.h"
#include "log_config.h"
#include "log_stat_thread.h"
#include "log_stat.h"

namespace KingNet { namespace Server { namespace LogServer {


	CStatThread::CStatThread()
	{
		m_iIndex = 0;
		m_iMaxSystemLogFileSize  = 0x40000000;
		m_iMaxSystemLogFileCount = 100;
	}

	CStatThread::~CStatThread()
	{
		
	}

	void CStatThread::set_index(int32_t index)
	{
		m_iIndex = index;
		m_statLog.SetProcessID(m_iIndex);
	}
	int32_t CStatThread::initialize(int32_t iFd){

		int32_t ret = m_statLog.InitLog(m_iMaxSystemLogFileSize,m_iMaxSystemLogFileCount, "./stat/");

		if( success != ret )
		{
			exit(-1);
		}
		
		m_SocketPipe.SetUpStream(iFd);
		
		return success;
	}


	//int32_t CStatThread::send_data(const char *buf,int32_t iLength ){
	//	int32_t iTempRet = 0;

	//	char  abyCode[10240];

	//	if( NULL == buf || 0 >= iLength || m_SocketPipe.GetSocketFD() <= 0 )
	//	{
	//		g_Syslog.WriteLog(log_mask_system_error,"[CStatThread::%s]Send stat string to write failed,log=%p,m_SocketPipe.GetSocketFD=%d\n",__FUNCTION__,buf,m_SocketPipe.GetSocketFD());
	//		return fail;
	//	}

	//	memcpy((void *)&abyCode[sizeof(int32_t)], (const void *)buf, iLength);
	//	iLength += sizeof(int32_t);
	//	*(int32_t *)abyCode = htonl(iLength);
	//	iTempRet = m_SocketPipe.SendOneCode(iLength, abyCode);
	//	if( iTempRet != success )
	//	{
	//		g_Syslog.WriteLog(log_mask_system_error,"[CProcessor::%s]Send stat string to write failed,log=%s,iTempRet=%d\n",__FUNCTION__,buf,iTempRet);
	//		return fail;
	//	}
	//	else
	//	{
	//		g_Syslog.WriteLog(log_mask_detail,"[CProcessor::%s]Successfully send log \"%s\" to write.\n",__FUNCTION__, buf);
	//	}
	//	receive_data();
	//	return success;
	//}

	int32_t CStatThread::decode_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength)
	{
		if (NULL == pszInCode || 0 >= iInCodeLength )
		{
			return fail;
		}

		char* pszTmp = (char*) pszInCode;
		int32_t iTmpSize = iInCodeLength;
		
		
		//int32_t head_length = 0;
		int16_t shCSHeadLen = 0;
		if (success != stCSHead.decode((const char*)pszTmp, iTmpSize,&shCSHeadLen))
		{
			
			LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_error, "[CStatThread::%s] CCSHead decode failed\n", __FUNCTION__);
			return fail;
		}

		//解包出来的协议包长度和传入的长度不等
		if (iTmpSize < stCSHead.m_stHead.nPackageLength)
		{
			LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_error, "[CStatThread::%s] CCSHead.nPackageLength invalid, stCSHead.m_stHead.nPackageLength= %d, in_size = %D\n", 
				__FUNCTION__, stCSHead.m_stHead.nPackageLength, iTmpSize);
			return fail;
		}

		pszTmp += shCSHeadLen;
		iTmpSize -= shCSHeadLen;

		LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_detail, "in [CStatThread::%s], receive message id:0x%X, type:%d, iSequence:%d\n", 
			__FUNCTION__, stCSHead.m_stHead.shMessageID, stCSHead.m_stHead.shMessageType,stCSHead.m_stHead.nSequence);

		if( stCSHead.m_stHead.shMessageID == CS_MSG_NOTIFY_WRITE_LOG && stCSHead.m_stHead.shMessageType == message_type_notify)
		{
			
			m_stNotifyBody.initialize();
			if( success != m_stNotifyBody.decode(pszTmp,iTmpSize) )
			{
				LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_error,"[CStatThread::%s] Process[%d] decode notify write log message error\n",__FUNCTION__,m_iIndex);
				return fail;
			}
			//LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_detail,"[CStatThread::%s]Process[%d] recv main thread message:%s\n",__FUNCTION__,m_iIndex,m_stNotifyBody.m_szContentStr);
			LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex, log_mask_detail, "[CStatThread::%s]Process[%d] recv main thread uid:%d type:%d message:%s\n", __FUNCTION__, m_iIndex, m_stNotifyBody.m_iUid, m_stNotifyBody.m_shTableType, m_stNotifyBody.m_szContentStr);
			TFileName tableName;
			if( fail == LOGSVRCONFIG.get_table_name(m_stNotifyBody.m_shTableType,tableName,sizeof(tableName)))
			{
				LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_warning,"[CStatThread::%s]Process[%d] get table name failed type[%d] !\n",__FUNCTION__,m_iIndex, m_stNotifyBody.m_shTableType);
				return fail;
			}
			m_statLog.WriteLog(m_stNotifyBody.m_szContentStr,tableName);
			//m_statLog.WriteServerLog("%s\n",m_stNotifyBody.m_szContentStr);
		}
		else
		{
			return fail;
		}
		return success;
	}


	int32_t CStatThread::receive_data()
	{
		fd_set fds_read;
		struct timeval stMonTime;
		int32_t iOpenFDNum = 0;
		int32_t iCmdNumber = 0;
		int32_t nTempCodeLength;
		char  abyCodeBuf[max_cs_package_size];
		char  *pMsg = NULL;


		if( m_SocketPipe.GetSocketFD()<= 0 )
		{
			LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_error,"[CStatThread::%s] m_SocketPipe[%d].GetSocketFD() return invalid value\n",__FUNCTION__,m_iIndex);
			return -1;
		}

		FD_ZERO( &fds_read );
		stMonTime.tv_sec = 0;
		stMonTime.tv_usec = WAIT_THREAD_MESSAGE_TIME;

		FD_SET( m_SocketPipe.GetSocketFD() , &fds_read );
		iOpenFDNum = select(m_SocketPipe.GetSocketFD()+1, &fds_read, NULL, NULL, &stMonTime);
		if( 0 >= iOpenFDNum )
		{
			return success;
		}

		m_SocketPipe.RecvData();

		for(;;)
		{
			//获得一个完整的Execer传过来的完整业务包,得到的包里包含有详细的错误信息之外,暂无它用.
			if( m_SocketPipe.GetOneCode(nTempCodeLength, abyCodeBuf) <= 0 )
			{
				break;//没有一个完整业务包,则跳出循环..
			}
			pMsg = (char *)&abyCodeBuf[sizeof(int32_t)];
			if( NULL == pMsg)
			{
				LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_warning,"[CStatThread::%s]Process[%d] recv main thread failed:%p\n",__FUNCTION__,m_iIndex,pMsg);
			}	
			
			
			if (success != decode_clientcode_to_message((const char*)pMsg, (const int32_t) nTempCodeLength))
			{
				LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,log_mask_system_warning, "in [CStatThread::%s], decode_udp_clientcode_to_message failed!\n", __FUNCTION__);
				continue;
			}
			
			iCmdNumber++;
		}
		return iCmdNumber;
	}
	void CStatThread::run(int32_t index)
	{
		LOGICFRAME.m_systemLog[m_iIndex].WriteThreadLog(m_iIndex,"CStatThread::%s process %d start run!\n",__FUNCTION__,index);
		while(true)
		{
			receive_data();
		}
	}

}}}
