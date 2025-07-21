#ifndef __LogServer_LOGICFRAME_H__
#define __LogServer_LOGICFRAME_H__

#include <vector>

#include "gameplatform_types.h"

#include "Common/common_namespace_definition.h"
#include "Common/common_memory.h"
#include "Common/common_singleton.h"
#include "Common/common_objectqueue.h"
#include "Common/common_logengine.h"
#include "Common/common_udpsocket.h"
#include "Common/common_socket.h"
#include "Common/common_object_hashtable.h"
#include "Common/common_datetime.h"


#include "log_common_define.h"
#include "public/server_message.h"
#include "public/server_log_message_body.h"
#include "log_message.h"
#include "log_stat.h"
#include "log_stat_thread.h"
#include "log_socket_pipe.h"
namespace KingNet { namespace Server {

	class CShmMemory;
	class CCodeQueue;
	class CFlag;
}}

namespace KingNet { namespace Server { namespace LogServer {


	class CLogicFrame
	{
	public:
		CLogicFrame();
		~CLogicFrame();

		void*				operator new(size_t size)throw();
		void				operator delete(void* pszMemory);

		int32_t				initialize();
		int32_t				resume();
		int32_t				to_working();
		int32_t				working();

		int32_t				encode_message_to_clientcode(char* pszOut, int32_t& iOutLength, CMessageHead& head, const char* pszBody, const int32_t iBodyLength);
		int32_t				decode_tcp_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength);
		int32_t				decode_udp_clientcode_to_message(const char* pszInCode, const int32_t iInCodeLength);

		int32_t				notify_to_close_connection( int32_t connection_idx,int32_t iUniqueID, int32_t ipAddr, int16_t port);

		int32_t				send_data_to_thread(int32_t uid,const char *buf, int32_t iLength );
	

		int32_t				get_uid_from_code(const char* pszCode, const int32_t iCodeSize);


		stDateTime			get_date_time();

	protected:
		
		int32_t				receive_message();   //返回收到的包个数

		int32_t				check_timers();      //检查超时
		int32_t				check_flag();		 //检查flag

		int32_t				receive_tcp_client_message();  //接收tcp通信包
		int32_t				receive_udp_client_message();  //接收udp通信包


		int32_t 			send_message_to_client(CMessageHead* head, const char* body, const int32_t length);


	public: 
		static CShmMemory*	m_pstMemory;
	public:
		static size_t		size();
		static void			set_memory(CShmMemory* memory);

	
	public:
		std::vector<CStatThread> m_szStateThreads;
		std::vector<CSocketPipe> m_szSocketPipes;
		std::vector<CLogServerLog> m_systemLog;

		//CStatThread			m_szStateThreads[max_stat_thread_count];
		//CSocketPipe			m_szSocketPipes[max_stat_thread_count];
		//CLogServerLog		m_systemLog[max_stat_thread_count];
	protected:
		

		

		CObjectPool*    	m_pstMessageEntryPool;					//是m_stMessage的底层存储实现
		CObjectQueue    	m_stMessage;							//消息头部，与m_pstMessageBody一起构成平台系统的主消息队列
		CCodeQueue*     	m_pstMessageBody;						//消息体

		CCodeQueue*			m_pstC2SChannel;						//C --> S 通信包共享内存管道
		CCodeQueue*			m_pstS2CChannel;						//S --> C 通信包共享内存管道

		CCodeQueue*			m_pstUdpC2SChannel;						//
		CCodeQueue*			m_pstUdpS2CChannel;						//
	protected:

		struct timeval		m_tvLastCheckTime;						//最近一次检测定时器的时间(主循环) 至少100ms的间隔
		time_t				m_tmLastCheckShm;						//最近一次检查共享内存的时间
		int32_t				m_nRequestSequence;						//与后台服务器交互的流水号，一个事务请求对应一个流水号
		stDateTime			m_stDatetime;							//当天日期

	};

	extern CShmMemory*		g_pstCoreShm;
	extern CShmMemory*		g_pstCSChannelShm;
	extern CShmMemory*		g_pstSCChannelShm;
	extern CShmMemory*		g_pstUDPCSChannelShm;
	extern CShmMemory*		g_pstUDPSCChannelShm;
	extern CFlag			g_byRunFlag;


	extern char g_szSSKey[auth_service_key_length];

#define LOGICFRAME Singleton<CLogicFrame>::Instance()

}}}//namespace KingNet { namespace Server { namespace LogServer {


#endif
