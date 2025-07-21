#ifndef __COMMON_TCPCONNECTION_H__
#define __COMMON_TCPCONNECTION_H__

#include "common_socket.h"
#include "common_namespace_definition.h"

SERVER_BEGIN


//some const variables
enum 
{
	recv_buffer_size = 0x200000,
	send_buffer_size = 0x200000,	//该定义为server之间的socket buffer长度，注意必要要保证大于server间通讯的最大包长
};

//connection status
enum enmTCPConnectionStatus
{
	enmTCPConnectionStatus_closed = 0,
	enmTCPConnectionStatus_opened,
	enmTCPConnectionStatus_connected,
	enmTCPConnectionStatus_error
};

enum enmConnErrorCode
{
	enmConnErrorCode_success					= 0,
	enmConnErrorCode_invalid_socket				= -1,
	enmConnErrorCode_recv_notenouth_buffer		= -2,
	enmConnErrorCode_peer_closed				= -3,
	enmConnErrorCode_send_notenouth_buffer		= -4,
	enmConnErrorCode_invalid_param				= -5,
	enmConnErrorCode_unknow						= -6,

};


//socket type
enum enmSocketTypes
{
	enmSocketTypes_listen = 0,
	enmSocketTypes_comm = 1,
};

//对TCP协议的Socket的封装
//主要接口:
//1.接收数据. 提供了在应用层的接收缓存区
//2.发送数据. 提供了在应用层对一次发生未发生完的数据的缓存
//3.建立监听端口(开启服务器端监听)
//4.建立连接(连接到监听的服务器端)
class CTCPSocket
{
public:
	CTCPSocket();
	~CTCPSocket();

	
	void close_socket();
	
	int32_t get_fd();
	enmTCPConnectionStatus get_status();

	//open socket as  a server or client
	int32_t open_as_server(uint16_t port, char* ip = NULL);
	int32_t open_as_client(char* localIP = NULL);

	//
	int32_t connect_to(char* ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);
	//u_long ip : 网络字节序的地址
	int32_t connect_to(u_long ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);

	//
	int32_t accept_fd(int32_t fd);

	//
	int32_t add_fd_set(fd_set* fdset);

	//
	bool is_fd_ready(fd_set* fdset);

	//receive data from socket stream, and store recv buffer, wait for processing
	//
	int32_t recv_data(void);

	//send data to remote peer
	int32_t send_data(char* data, size_t size);

	//get one completed code from receive buffer
	//return value: >0 -- success, <= 0  -- some error
	//size: 应该传入data的大小,传出得到的实际数据大小（目前size没有传入data的大小，仅传出数据的实际大小）
	int32_t get_one_code(char* data, size_t& size);
	
	int32_t get_memcache_one_code(char* data,size_t& size);
	/**
	@brief 接收到的数据包前6个字节表示包长度
	*/
	int32_t get_one_code_ex(char* data, size_t& size);

	//
	void set_4_byte_size(bool flag);

	//
	bool is_4_byte_size(void);

private: 
	//prohibit copy constructor & operator '='
	CTCPSocket(const CTCPSocket& original);
	CTCPSocket& operator =(CTCPSocket& rvalue);

public:

protected:
	enmSocketTypes m_socket_type;
	int32_t m_socket_fd;

	enmTCPConnectionStatus m_socket_status;

	//标识是否用4字节表示长度，true :yes；否则，no
	//即使用int32_t表示长度,实际长度不会超过int16_t的范围
	//具体是使用4个字节还是2个字节表示长度，是由应用协议决定的，这会影响
	//对包实际长度的识别
	bool m_4bytes;

	//receiving buffer
	int32_t m_recv_head;
	int32_t m_recv_tail;
	char m_recv_buffer[recv_buffer_size+1]; //特别注意：不是循环缓冲区

	//sending buffer
	int32_t m_send_head;
	int32_t m_send_tail;
	char m_send_buffer[send_buffer_size+1];//特别注意：不是循环缓冲区

#ifdef __MULTI_THREAD__
	pthread_mutex_t m_mutex;			//the mutual exclusive variable
#endif

private:
};

class CTCPConnection
{
public:
	CTCPConnection();
	~CTCPConnection();
private:
	CTCPConnection(const CTCPConnection& original);
	CTCPConnection& operator = (const CTCPConnection& original);

public:
	//
	int32_t initialize(int32_t entity_id, int16_t entity_type, u_long ip, uint16_t port);
	//
	CTCPSocket& get_socket();
	//
	int32_t get_entity_id();
	//
	int16_t get_entity_type();
	//
	u_long get_ip();
	//
	uint16_t get_port();
	//
	void get_entity_info(int32_t& id, int16_t& type, u_long& ip, uint16_t& port);

	//
	int32_t connect_to(char* localIP = NULL, bool nonblock = true, int32_t msecond = 100);

protected:
	CTCPSocket m_socket;
	int16_t m_entity_type;
	int16_t m_entity_id;

	u_long m_ip;
	uint16_t m_port;

private:
};

SERVER_END



#endif //__COMMON_TCPCONNECTION_H__

