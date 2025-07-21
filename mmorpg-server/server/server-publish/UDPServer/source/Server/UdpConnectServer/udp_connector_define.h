#ifndef __UDPCONNECTOR_DEFINE_H__
#define __UDPCONNECTOR_DEFINE_H__

#include "gameplatform_types.h"
#include "gameplatform_message.h"

#include "Common/common_base.h"
#include "Connector/udp_connector_head.h"

#include "public/server_message.h"

namespace KingNet { namespace Server { namespace UdpConnectorServer {

	enum enmRunFlag
	{
		run_flag_reload		= 0x00000001,
		run_flag_exit		= 0x00000002,
	};

	enum
	{
		http_head_length = 49,				//httpͷ�Ĺ̶�����
											//httpͷ: POST /KingNet HTTP/1.1\r\nContent-Length:65535\r\n\r\n
	};

	struct stConnectorConfig
	{
		int32_t m_iCoreKey;
		int32_t m_iCSChannelKey;
		int32_t m_iSCChannelKey;

		int32_t m_iLogLevel;
	};

	enum enmSocketType
	{
		enmSocketType_invalid = 0,
		enmSocketType_listen =	1,			//����socket
		enmSocketType_common =  2,			//��ͨsocket
	};

	enum
	{
		max_open_port_count = 8,			//�������򿪵Ķ˿������Ŀ

		max_socket_count = 32,				//�������շ����ݵ�socket�������Ŀ

		max_ip_address_length = 30,			//

		auxiliary_handler_count = 2,		//

		max_package_count_per_sending = 512,//		

		max_file_path = 255,				//

		socket_fd_invalid = -1,				//
	};

	enum
	{
		relay_Header_Length = (3*sizeof(int16_t) + 3*sizeof(int32_t) + 2*sizeof(int8_t)),
		upd_Header_Length = (4*sizeof(int32_t) +7*sizeof(int16_t))
	};

#define CONFIG_FILE    "./config/udpconnectorsvrd.cfg"   

struct stServerConfig 
{
	int32_t m_iOpenedPortCount;
	uint16_t m_unOpenedPort[max_open_port_count];	//�����Ķ˿�

	//some timeout & interval
	int32_t m_iLogStatisticInterval;				//ͳ�Ƽ��

	int32_t m_iSocketBufferSize;				
};

//
struct stSocketInfo 
{
	int32_t m_socket_fd;
	char m_client_address[max_ip_address_length];
	enmSocketType m_socket_type;
	int32_t m_uin;											//��Ӧ�ò������������ID������Ψһ��ʶ

	uint32_t m_source_ip;									//Դip
	uint16_t m_source_port;									//Դport

	uint16_t m_local_port;									//���ĸ������˿������ϵ�
	int32_t m_recv_bytes;									//�յ������ݴ�С
	int32_t m_recv_package;									//�յ����ݰ�����

	time_t m_create_time;									//socket����ʱ��
	time_t m_lastrecv_timestamp;							//���һ�δӶԶ˽��յ����ݵ�timestamp
	char m_package_buffer[max_relay_pkg_size];				//����õĻ�����
};

}}}//namespace KingNet { namespace Server { { namespace UdpConnectorServer

#endif //__UDPCONNECTOR_DEFINE_H__

