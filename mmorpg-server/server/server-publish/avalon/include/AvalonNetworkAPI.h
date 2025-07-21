/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	����api����
 */
#ifndef _AVALON_NETWORKAPI_H_
#define _AVALON_NETWORKAPI_H_

#include "AvalonDefine.h"
#include "AvalonLogger.h"

#include <cstdarg>

#if defined(AVALON_OS_WINDOWS)
#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif
#	include <Winsock2.h>
#	include <windows.h>
#	pragma comment(lib,"Ws2_32.lib") 

#else
#ifndef socklen_t
#define socklen_t __socklen_t
#endif
#	include <unistd.h>
#	include <fcntl.h>
#	include <errno.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <sys/epoll.h>
#	include <sys/select.h>
#	include <sys/time.h>
#	include <netinet/tcp.h>
#endif

namespace Avalon
{
	typedef struct sockaddr_in sockaddr_t;

#if defined(AVALON_OS_WINDOWS)
	typedef SOCKET sock_t;
	typedef int socklen_t;
#	define AVALON_INVALID_SOCK INVALID_SOCKET
#	define AVALON_SOCK_ERROR SOCKET_ERROR

#	define AVALON_ECONNABORTED		WSAECONNABORTED
#	define AVALON_EWOULDBLOCK		WSAEWOULDBLOCK
#	define AVALON_ECONNRESET		WSAECONNRESET
#	define AVALON_EINTR				WSAEINTR
#	define AVALON_ENOBUFS			WSAENOBUFS
#	define AVALON_EAGAIN			-1

#else
	typedef int sock_t;
#	define AVALON_INVALID_SOCK -1
#	define AVALON_SOCK_ERROR -1

#	define closesocket close

#	define AVALON_ECONNABORTED		ECONNABORTED
#	define AVALON_EWOULDBLOCK		EWOULDBLOCK
#	define AVALON_ECONNRESET		ECONNRESET
#	define AVALON_EINTR				EINTR
#	define AVALON_ENOBUFS			ENOBUFS
#	define AVALON_EAGAIN			EAGAIN
#endif

#define AVALON_IPV4_LEN 32

	/**
	 *@brief ��ʼ���׽��ֵ�ַ
	 *@param ip ip��ַ
	 *@param port �˿�
	 */
	void sockaddr_encode(sockaddr_t& addr,const char* ip,UInt16 port);

	/**
	 *@brief ��ȡip�Ͷ˿�
	 */
	void sockaddr_decode(const sockaddr_t& addr,char* ip,UInt16& port);


	/**
	 *@brief ����һ���׽���
	 *@param type �׽�������
	 */
	sock_t sock_create(int type);

	/**
	 *@brief �󶨵�ַ
	 */
	bool sock_bind(sock_t sock,const sockaddr_t& addr);

	/**
	 *@brief ��������
	 */
	bool sock_connect(sock_t sock,const sockaddr_t& addr);

	/**
	 *@brief listen
	 */
	bool sock_listen(sock_t sock,int backlog);

	/**
	 *@brief accept
	 */
	sock_t sock_accept(sock_t sock,sockaddr_t* addr);

	/**
	 *@brief ��������
	 */
	int sock_send(sock_t sock,const void* buf,size_t len,int flags = 0);

	/**
	 *@brief ��������
	 */
	int sock_sendto(sock_t sock,const void* buf,size_t len,const sockaddr_t& addr,int flags = 0);

	/**
	 *@brief ��������
	 */
	int sock_recv(sock_t sock,void* buf,size_t len,int flags = 0);

	/**
	 *@brief ��������
	 */
	int sock_recvfrom(sock_t sock,void* buf,size_t len,const sockaddr_t& addr,int flags = 0);

	/**
	 *@brief �ر��׽���
	 */
	bool sock_close(sock_t sock);
	void sock_shutdown(sock_t sock);

	/**
	 *@brief ����׽��ֱ��ε�ַ
	 */
	bool sock_localname(sock_t sock,sockaddr_t& addr);

	/**
	 *@brief ��öԶ˵�ַ
	 */
	bool sock_peername(sock_t sock,sockaddr_t& addr);

	/**
	 *@brief �����׽���ѡ��
	 */
	bool sock_setopt(sock_t sock,int level,int opt,const void* optval,socklen_t optlen);

	/**
	 *@brief ��ȡ�׽���ѡ��
	 */
	bool sock_getopt(sock_t sock,int level,int opt,void* optval,socklen_t* optlen);

	/**
	 *@brief ����Ϊ������
	 */
	bool sock_setnonblock(sock_t sock);

	/**
	 *@brief ��ȡ������
	 */
	int sock_error();

	
	/**
	 *@brief ����������־
	 */
	void net_setlogger(Logger* logger);

	/**
	 *@brief ��ȡ������־
	 */
	Logger* net_getlogger();

#define NETLOG_ERROR(Format, ...) \
	do{ \
		if(net_getlogger() != NULL) \
			net_getlogger()->Error(Format, ##__VA_ARGS__); \
	}while(false);

#define NETLOG_INFO(Format, ...) \
	do{ \
		if(net_getlogger() != NULL) \
			net_getlogger()->Info(Format, ##__VA_ARGS__); \
	}while(false);

}

#endif
