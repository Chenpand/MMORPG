#ifndef __LogServer_COMMON_DEFINE_H__
#define __LogServer_COMMON_DEFINE_H__

#include "gameplatform_types.h"
#include "gameplatform_message.h"
#include "Connector/connector_head.h"
#include "public/server_message.h"
#include "public/server_log_message_body.h"
#include "CSMessages_i.h"
#include <iostream>
#include <deque>
#include "Common/common_socket.h"
#include "public/server_public_define.h"
using namespace std;
namespace KingNet { namespace Server { namespace LogServer {


enum enmMessageType
{
	message_type_request	= MSG_TYPE_REQUEST,
	message_type_response	= MSG_TYPE_RESPONSE, 
	message_type_notify		= MSG_TYPE_NOTIFY,
	message_type_other		= MSG_TYPE_OTHER,
};

enum enmObjectPoolType //LogServer中的实体对象管理器类型
{
	pool_type_handler		= 0x01,
	pool_type_messageentry	= 0x02,
};
enum enmRunFlag
{
	run_flag_reload			= 0x00000001,
	run_flag_exit			= 0x00000002,

};

#define NET_BUFLEN	0x400000        //网络缓冲区的大小
#define WAIT_THREAD_MESSAGE_TIME 100  //等待时间

enum
{
	max_table_count					  = 100,
	max_handler_timeout               = 120,
	check_share_memory_interval		  = 120,
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)

	//max_handler_count_per_server      = 2000,

	//max_timer_count_per_server       = 8000,
	//max_message_entry_count_in_queue = 10000,
	//max_message_body_queue_size      = 0x4000000, //64M

	//max_handler_heartbeat_count       = 100,
	//check_handler_alive_interval		 = 60,
	//max_stat_thread_count			=  1,
#else

	//max_handler_count_per_server      = 100000,
	//max_timer_count_per_server       = 500000,
	//max_message_entry_count_in_queue = 160000,
	//max_message_body_queue_size      = 0x10000000,	//256M 

	//max_handler_heartbeat_count       = 8,			//玩家超时的最大心跳次数
	//check_handler_alive_interval		 = 120,
	//max_stat_thread_count			  = 8,
#endif


};


typedef char TName[32];
typedef char TFileName[255];

#define SERVER_VERSION "CB1-BUILD001"

}}}

#endif
