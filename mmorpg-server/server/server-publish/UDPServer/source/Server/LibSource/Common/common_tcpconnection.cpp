
#include "Common/common_tcpconnection.h"
#include <pthread.h>
#include <errno.h>
#include <string.h>

SERVER_BEGIN

CTCPSocket::CTCPSocket()
{
	m_socket_fd = -1;
	m_socket_type = enmSocketTypes_listen; //
	m_socket_status = enmTCPConnectionStatus_closed;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	m_4bytes = false;
	

#ifdef __MULTI_THREAD__
	pthread_mutex_init(&m_mutex, NULL);
#endif
	
}

//
CTCPSocket::~CTCPSocket()
{
	if (0 < m_socket_fd && enmTCPConnectionStatus_opened == m_socket_status)
	{
		close_socket();
	}
}

//close the tcp socket
void CTCPSocket::close_socket()
{
	if (m_socket_fd > 0)
	{
		close(m_socket_fd);
	}

	m_socket_fd = -1;
	m_socket_status = enmTCPConnectionStatus_closed;
	return;
}

int32_t CTCPSocket::get_fd()
{
	int32_t fd = -1;

//#ifdef __MULTI_THREAD__
//	pthread_mutex_lock(&m_mutex);
//#endif

	fd = m_socket_fd;


//#ifdef __MULTI_THREAD__
//	pthread_mutex_unlock(&m_mutex);
//#endif

	return fd;
}

enmTCPConnectionStatus CTCPSocket::get_status()
{
	enmTCPConnectionStatus status = enmTCPConnectionStatus_closed;

//#ifdef __MULTI_THREAD__
//	pthread_mutex_lock(&m_mutex);
//#endif

	status = m_socket_status;
	
//#ifdef __MULTI_THREAD__
//	pthread_mutex_unlock(&m_mutex);
//#endif

	return status;
}

int32_t CTCPSocket::open_as_server( uint16_t port, char* ip /*= NULL*/ )
{
	//firstly, close the socket
	if (enmTCPConnectionStatus_closed != m_socket_status && m_socket_fd > 0)
	{
		close_socket();
	}

	//open socket
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		m_socket_fd = -1;
		m_socket_status = enmTCPConnectionStatus_closed;
		return -1;
	}

	//
	int32_t flag = 1;

	if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, (int)sizeof(flag)))
	{
		close_socket();
		return -1;
	}

	//
	struct sockaddr_in stSocketAddr;
	memset(&stSocketAddr, 0x0, sizeof(stSocketAddr));
	stSocketAddr.sin_family = AF_INET;
	if (NULL != ip)
	{
		stSocketAddr.sin_addr.s_addr = inet_addr(ip);
	}
	else
	{
		stSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	}

	stSocketAddr.sin_port = (u_short)htons(port);
	socklen_t addrSize = socklen_t(sizeof(stSocketAddr));
	if (0 != bind(m_socket_fd, (const sockaddr*) &stSocketAddr, addrSize))
	{
		close_socket();
		return -1;
	}

	if (0 != listen(m_socket_fd, 15))
	{
		close_socket();
		return -1;
	}

	//设置为非阻塞
	set_non_block(m_socket_fd);

	m_socket_type = enmSocketTypes_listen; //监听socket
	m_socket_status = enmTCPConnectionStatus_opened;

	return 0;
}

int32_t CTCPSocket::open_as_client( char* localIP /*= NULL*/ )
{
	//firstly, close the socket
	if (enmTCPConnectionStatus_closed != m_socket_status && m_socket_fd > 0)
	{
		close_socket();
	}

	//open socket
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		m_socket_status = enmTCPConnectionStatus_closed;
		m_socket_fd = -1;
		return -1;
	}

	if (NULL != localIP)
	{
		struct sockaddr_in stLocalAddress;
		memset(&stLocalAddress, 0x0, sizeof(stLocalAddress));

		stLocalAddress.sin_family = AF_INET;
		stLocalAddress.sin_addr.s_addr = inet_addr(localIP);
		socklen_t addressSize = (socklen_t)sizeof(stLocalAddress);
		if (0 != bind(m_socket_fd, (const struct sockaddr*) &stLocalAddress, addressSize))
		{
			close_socket();
			return -1;
		}

	}

	m_socket_type = enmSocketTypes_comm;
	m_socket_status = enmTCPConnectionStatus_opened;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	return 0;
}

int32_t CTCPSocket::connect_to( char* ip, uint16_t port, bool nonblock /*= true*/ , int32_t msecond /* = 100*/)
{
	if (NULL == ip)
	{
		return -1;
	}

	//check socket type (common or listening)
	if (enmSocketTypes_comm != m_socket_type)
	{
		return -1;
	}

	if (enmTCPConnectionStatus_opened != m_socket_status || 0 > m_socket_fd)
	{
		return -1;
	}

	struct sockaddr_in stDstAddress;
	memset(&stDstAddress, 0x0, sizeof(stDstAddress));
	stDstAddress.sin_family = AF_INET;
	stDstAddress.sin_addr.s_addr = inet_addr(ip);
	stDstAddress.sin_port = htons(port);
	socklen_t sockSize = (socklen_t)sizeof(stDstAddress);

	//if (0 != connect(m_socket_fd, (const sockaddr*) &stDstAddress, sockSize))
	if (0 != connect_nonblock(m_socket_fd, &stDstAddress, sockSize, msecond))
	{
		close_socket();
		return -1;
	}

	if (nonblock)
	{
		set_non_block(m_socket_fd);
	}

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	//set socket status to connected !!
	m_socket_status = enmTCPConnectionStatus_connected;
	return 0;

}

int32_t CTCPSocket::connect_to( u_long ip, uint16_t port, bool nonblock /*= true*/ , int32_t msecond /* = 100*/)
{
	//check socket type (common or listening)
	if (enmSocketTypes_comm != m_socket_type)
	{
		return -1;
	}

	if (enmTCPConnectionStatus_opened != m_socket_status || 0 > m_socket_fd)
	{
		return -1;
	}

	struct sockaddr_in stDstAddress;
	memset(&stDstAddress, 0x0, sizeof(stDstAddress));
	stDstAddress.sin_family = AF_INET;
	stDstAddress.sin_addr.s_addr = ip;
	stDstAddress.sin_port = htons(port);
	socklen_t sockSize = (socklen_t)sizeof(stDstAddress);

	//if (0 != connect(m_socket_fd, (const sockaddr*) &stDstAddress, sockSize))
	if (0 != connect_nonblock(m_socket_fd, &stDstAddress, sockSize, msecond))
	{
		close_socket();
		return -1;
	}

	if (nonblock)
	{
		set_non_block(m_socket_fd);
	}

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	m_socket_status = enmTCPConnectionStatus_connected;
	return 0;
}

int32_t CTCPSocket::accept_fd( int32_t fd )
{
	if (fd < 0)
	{
		return -1;
	}

#ifdef __MULTI_THREAD__
	pthread_mutex_lock(&m_mutex);
#endif

	if (enmTCPConnectionStatus_connected == m_socket_status && m_socket_fd > 0)
	{
		close_socket();
	}

	m_socket_fd = fd;
	m_socket_status = enmTCPConnectionStatus_connected;
	m_socket_type = enmSocketTypes_comm;

	m_recv_head = 0;
	m_recv_tail = 0;

	m_send_head = 0;
	m_send_tail = 0;

	//set non block
	set_non_block(m_socket_fd);

#ifdef __MULTI_THREAD__
	pthread_mutex_unlock(&m_mutex);
#endif

	return 0;

}

int32_t CTCPSocket::add_fd_set( fd_set* fdset )
{
	if (NULL == fdset)
	{
		return -1;
	}

//#ifdef __MULTI_THREAD__
//	pthread_mutex_lock(&m_mutex);
//#endif

	if (enmTCPConnectionStatus_connected == m_socket_status &&  m_socket_fd > 0)
	{
		FD_SET(m_socket_fd, fdset);
	}
	else if (m_socket_fd > 0)
	{
		close_socket();
		return -1;
	}


//#ifdef __MULTI_THREAD__
//	pthread_mutex_unlock(&m_mutex);
//#endif
	
	return 0;
}

bool CTCPSocket::is_fd_ready( fd_set* fdset )
{
	if (NULL == fdset)
	{
		return false;
	}

	bool flag = false;

//#ifdef __MULTI_THREAD__
//	pthread_mutex_lock(&m_mutex);
//#endif

	if (enmTCPConnectionStatus_connected == m_socket_status && m_socket_fd > 0)
	{
		flag = FD_ISSET(m_socket_fd, fdset);
	}


//#ifdef __MULTI_THREAD__
//	pthread_mutex_unlock(&m_mutex);
//#endif

	return flag;
}


int32_t CTCPSocket::recv_data( void )
{
	int32_t ret = 0;

//#ifdef __MULTI_THREAD__
//	pthread_mutex_lock(&m_mutex);
//#endif

	if (enmTCPConnectionStatus_connected != m_socket_status || 0 > m_socket_fd)
	{
//#ifdef __MULTI_THREAD__
//		pthread_mutex_unlock(&m_mutex);
//#endif
		return (int32_t)enmConnErrorCode_invalid_socket;
	}

	//
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	//
	int32_t received_byte = 0;
	
	do //循环从网络连接上接收数据
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			//接收缓冲区的头指针不在
			if (m_recv_head > 0)
			{
				//注意: 要使用memmove而不是memcpy
				memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(m_recv_tail - m_recv_head));
				m_recv_tail -= m_recv_head;
				m_recv_head = 0;
			}
			else
			{
				//recv buffer was full, just stop receiving data
				ret = (int32_t)enmConnErrorCode_recv_notenouth_buffer;
				break;
			}
		}//if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))

		//
		received_byte = recv(m_socket_fd, &m_recv_buffer[m_recv_tail], size_t(sizeof(m_recv_buffer) - m_recv_tail), 0);
		if (received_byte > 0)
		{
			m_recv_tail += received_byte;
		}
		else if (0 == received_byte)
		{	
			//peer was closed, so just break receiving
			close_socket();
			ret = enmConnErrorCode_peer_closed;
			break;
		}
		else if (EAGAIN != errno) //received_byte < 0 && EAGAIN != errno
		{
			//must be some error!
			close_socket();
			ret = enmConnErrorCode_unknow;
			break;
		}

	} while (received_byte > 0); //


//#ifdef __MULTI_THREAD__
//	pthread_mutex_unlock(&m_mutex);
//#endif

	return ret;
}

int32_t CTCPSocket::send_data( char* data, size_t size )
{
	int32_t ret = 0;
	if (NULL == data || 0 == size)
	{
		return (int32_t)enmConnErrorCode_invalid_param;
	}

#ifdef __MULTI_THREAD__
	pthread_mutex_lock(&m_mutex);
#endif

	if (enmTCPConnectionStatus_connected != m_socket_status || m_socket_fd < 0)
	{
#ifdef __MULTI_THREAD__
		pthread_mutex_unlock(&m_mutex);
#endif
		return (int32_t)enmConnErrorCode_invalid_socket;
	}

	int32_t data_remaind_byte = 0;
	data_remaind_byte = m_send_tail - m_send_head;

	//上次发生的剩余数据
	char* pszDataPtr = &m_send_buffer[m_send_head];

	int32_t sended_byte = 0;
	while (data_remaind_byte > 0)
	{
		sended_byte = send(m_socket_fd, pszDataPtr, (size_t)data_remaind_byte, 0);
		if (sended_byte > 0)
		{
			m_send_head += sended_byte;
			pszDataPtr += sended_byte;
			data_remaind_byte -= sended_byte;
		}
		if (sended_byte < 0 && EAGAIN != errno)
		{
			m_socket_status = enmTCPConnectionStatus_error;
			ret = (int32_t)enmConnErrorCode_unknow;
			break;
		}
		if (sended_byte < 0 )
		{
			if( send_buffer_size - m_send_tail >= (int32_t)size )
			{
				memcpy(&m_send_buffer[m_send_tail], pszDataPtr, (size_t)size);
				m_send_tail += size;
				ret = 0;  //暂且认为已经发生成功
			}
			else if( send_buffer_size - m_send_tail + m_send_head >= (int32_t)size)
			{
				memmove(&m_send_buffer[0], &m_send_buffer[m_send_head], (size_t)data_remaind_byte);
				m_send_head = 0;
				m_send_tail = data_remaind_byte;
				memcpy(&m_send_buffer[m_send_tail], pszDataPtr, (size_t)size);
				m_send_tail += size;
				ret = 0;  //暂且认为已经发生成功
			}
			else
			{
				ret = (int32_t)enmConnErrorCode_send_notenouth_buffer;
			}
			break;
		}


	}//while (data_remaind_byte > 0)

	if (0 == data_remaind_byte)
	{
		m_send_head = 0;
		m_send_tail = 0;
	}
	else
	{	
#ifdef __MULTI_THREAD__
		pthread_mutex_unlock(&m_mutex);
#endif

		return ret;
	}
	
	//
	data_remaind_byte = (int32_t)size;
	pszDataPtr = data;

	while (data_remaind_byte > 0)
	{
		sended_byte = send(m_socket_fd, pszDataPtr, (size_t)data_remaind_byte, 0);
		if (sended_byte > 0)
		{
			pszDataPtr += sended_byte;
			data_remaind_byte -= sended_byte;
		}
		else if (sended_byte < 0 && EAGAIN != errno)
		{
			m_socket_status = enmTCPConnectionStatus_error;
			ret = (int32_t)enmConnErrorCode_unknow;
			break;
		}
		else if (sended_byte < 0)
		{
			//cache the remained data
			memcpy(&m_send_buffer[m_send_tail], pszDataPtr, (size_t)data_remaind_byte);
			m_send_tail += data_remaind_byte;
			ret = 0;  //暂且认为已经发生成功
			break;
		}

	}//while (data_remaind_byte > 0)


#ifdef __MULTI_THREAD__
	pthread_mutex_unlock(&m_mutex);
#endif

	return ret;
}

//
void CTCPSocket::set_4_byte_size( bool flag )
{
	m_4bytes = flag;
}

bool CTCPSocket::is_4_byte_size( void )
{
	return m_4bytes;
}

//
int32_t CTCPSocket::get_one_code( char* data, size_t& size )
{
	if (NULL == data)
	{
		return -1;
	}

	int32_t buffer_data_size = 0;
	buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size <= 0)
	{
		return 0;
	}

	//根据表示长度的字节数，检查数据的合法性
	if ((is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int32_t)) 
		||(!is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int16_t)))
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;

		}

		return 0;
	}

	//长度字段占用4byte
	int32_t code_size = 0;
	if (is_4_byte_size())
	{
		code_size = (int32_t)ntohl((u_long) (*(int32_t*)&m_recv_buffer[m_recv_head]));
	}
	else
	{
		code_size = (int32_t)ntohs((uint16_t) (*(int16_t*)&m_recv_buffer[m_recv_head]));
	}

	if (code_size <= 0)
	{
		return 0;
	}

	//若接收缓冲区内的数据不足一个code
	if (buffer_data_size < code_size)
	{
		//并且数据已经存放到缓冲区尾了, 则移动数据到接收缓冲区头部
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;
		}

		return 0;
	}

	//
	//最好判断一下传入参数:接收buffer的大小,目前的实现是没有判断，这就要求传入的buffer长度是足够的
	size = (size_t)code_size;
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += code_size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return 1;
}
int32_t CTCPSocket::get_memcache_one_code( char* data, size_t& size )
{
	if (NULL == data)
	{
		return -1;
	}

	int32_t buffer_data_size = 0;
	buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size <= 0)
	{
		return 0;
	}

	//根据表示长度的字节数，检查数据的合法性
	if ((is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int32_t)) 
		||(!is_4_byte_size() && buffer_data_size < (int32_t)sizeof(int16_t)))
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;

		}

		return 0;
	}

	//长度字段占用4byte
	if( buffer_data_size <= 8)
	{
		return 0;
	}
	int32_t code_size = (int32_t)ntohl((u_long) (*(int32_t*)(&m_recv_buffer[m_recv_head+8]))) + 24;

	if (code_size <= 0 )
	{
		return 0;
	}

	//若接收缓冲区内的数据不足一个code
	if (buffer_data_size < code_size)
	{
		//并且数据已经存放到缓冲区尾了, 则移动数据到接收缓冲区头部
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;
		}

		return 0;
	}

	//
	//最好判断一下传入参数:接收buffer的大小,目前的实现是没有判断，这就要求传入的buffer长度是足够的
	size = (size_t)code_size;
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += code_size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return 1;
}

int32_t CTCPSocket::get_one_code_ex( char* data, size_t& size )
{
	if (NULL == data)
	{
		return -1;
	}

	int32_t buffer_data_size = 0;
	buffer_data_size = m_recv_tail - m_recv_head;

	//判断接收缓冲区内的数据大小
	if (buffer_data_size <= 0)
	{
		return 0;
	}

	//根据表示长度的字节数，检查数据的合法性
	if (buffer_data_size < 6)
	{
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;

		}

		return 0;
	}

	//长度字段占用6byte,取得code的长度
 	int32_t code_size = 0;
	char pkgLenBuf[6];
	memset(pkgLenBuf, 0, sizeof(pkgLenBuf));
	memcpy(pkgLenBuf, &m_recv_buffer[m_recv_head], sizeof(pkgLenBuf));
	sscanf(pkgLenBuf, "%6x", &code_size);

	//若接收缓冲区内的数据不足一个code
	if (buffer_data_size < code_size)
	{
		//移动数据到接收缓冲区头部
		if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
		{
			memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
			m_recv_head = 0;
			m_recv_tail = buffer_data_size;
		}

		return 0;
	}

	//
	//最好判断一下传入参数:接收buffer的大小.
	size_t t = (size_t)(code_size + sizeof(pkgLenBuf));
	if( size < t)
	{
		return -2;	
	}
	size = t;//这意味着前6Byte中保存的长度并不包括6Byte自身
	memcpy(data, &m_recv_buffer[m_recv_head], size);

	//
	m_recv_head += size;
	if (m_recv_tail == m_recv_head)
	{
		m_recv_head = 0;
		m_recv_tail = 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// CTCPConnection Imp
//////////////////////////////////////////////////////////////////////////
CTCPConnection::CTCPConnection()
{
	m_ip = 0;
	m_port = 0;
	m_entity_id = -1;
	m_entity_type = -1;
}

CTCPConnection::~CTCPConnection()
{
	//nothing
}

int32_t CTCPConnection::initialize( int32_t entity_id, int16_t entity_type, u_long ip, uint16_t port )
{
	m_entity_id = entity_id;
	m_entity_type = entity_type;
	m_ip = ip;
	m_port = port;
	
	return 0;
}

CTCPSocket& CTCPConnection::get_socket()
{
	return m_socket;
}

int32_t CTCPConnection::get_entity_id()
{
	return m_entity_id;
}

int16_t CTCPConnection::get_entity_type()
{
	return m_entity_type;
}

u_long CTCPConnection::get_ip()
{
	return m_ip;
}

uint16_t CTCPConnection::get_port()
{
	return m_port;
}

void CTCPConnection::get_entity_info( int32_t& id, int16_t& type, u_long& ip, uint16_t& port )
{
	id = m_entity_id;
	type = m_entity_type;
	ip = m_ip;
	port = m_port;
}

int32_t CTCPConnection::connect_to( char* localIP, bool nonblock /*= true*/, int32_t msecond /* = 100*/ )
{
	if (m_entity_id < 0)
	{
		return -1;
	}

	int32_t ret = m_socket.open_as_client(localIP);
	if (0 != ret)
	{
		return -1;
	}

	ret = m_socket.connect_to(m_ip, m_port, nonblock, msecond);
	if (0 != ret)
	{
		return -1;
	}

	return ret;
}


SERVER_END
