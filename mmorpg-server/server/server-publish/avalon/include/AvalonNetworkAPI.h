/**
 *@author	 
 *@email	 
 *@date		2011-03-03	
 *@brief	网络api定义
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
	 *@brief 初始化套接字地址
	 *@param ip ip地址
	 *@param port 端口
	 */
	void sockaddr_encode(sockaddr_t& addr,const char* ip,UInt16 port);

	/**
	 *@brief 获取ip和端口
	 */
	void sockaddr_decode(const sockaddr_t& addr,char* ip,UInt16& port);


	/**
	 *@brief 创建一个套接字
	 *@param type 套接字类型
	 */
	sock_t sock_create(int type);

	/**
	 *@brief 绑定地址
	 */
	bool sock_bind(sock_t sock,const sockaddr_t& addr);

	/**
	 *@brief 建立连接
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
	 *@brief 发送数据
	 */
	int sock_send(sock_t sock,const void* buf,size_t len,int flags = 0);

	/**
	 *@brief 发送数据
	 */
	int sock_sendto(sock_t sock,const void* buf,size_t len,const sockaddr_t& addr,int flags = 0);

	/**
	 *@brief 接受数据
	 */
	int sock_recv(sock_t sock,void* buf,size_t len,int flags = 0);

	/**
	 *@brief 接受数据
	 */
	int sock_recvfrom(sock_t sock,void* buf,size_t len,const sockaddr_t& addr,int flags = 0);

	/**
	 *@brief 关闭套接字
	 */
	bool sock_close(sock_t sock);
	void sock_shutdown(sock_t sock);

	/**
	 *@brief 获得套接字本段地址
	 */
	bool sock_localname(sock_t sock,sockaddr_t& addr);

	/**
	 *@brief 获得对端地址
	 */
	bool sock_peername(sock_t sock,sockaddr_t& addr);

	/**
	 *@brief 设置套接字选项
	 */
	bool sock_setopt(sock_t sock,int level,int opt,const void* optval,socklen_t optlen);

	/**
	 *@brief 获取套接字选项
	 */
	bool sock_getopt(sock_t sock,int level,int opt,void* optval,socklen_t* optlen);

	/**
	 *@brief 设置为非阻塞
	 */
	bool sock_setnonblock(sock_t sock);

	/**
	 *@brief 获取错误码
	 */
	int sock_error();

	
	/**
	 *@brief 设置网络日志
	 */
	void net_setlogger(Logger* logger);

	/**
	 *@brief 获取网络日志
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
