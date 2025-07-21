#ifndef __COMMON_SOCKET_H__
#define __COMMON_SOCKET_H__

#include <stdio.h>
#include "Common/common_base.h"
#include "common_namespace_definition.h"

SERVER_BEGIN

//block flag
enum enmSocketBlockFlag
{
	enmSocketBlockFlag_lock = 0,
	enmSocketBlockFlag_nonblock = 1,
};


#ifdef __cplusplus
extern "C"
{
#endif

int32_t socket_address_to_string(struct sockaddr_in& stAddress, char* pszIPPort, size_t size);

int32_t set_non_block(int32_t fd);

//设置socket的接收&发送缓冲区大小.特别注意的是设置socket缓冲区大小的时机
//对于client来说:必须在调用connect函数前进行对接收缓冲区的设置
//对于server来说:必须在调用listen函数前进行对接收缓冲区的设置
int32_t set_socket_buffer_size(int32_t fd, size_t size);

int32_t create_udp_socket(uint16_t port, enmSocketBlockFlag flag = enmSocketBlockFlag_lock, char* pszIP = NULL);

//简单的接收&发送
//ip_size : pszSrcIP的长度
int32_t recv_data_from(int32_t fd, char* pszBuffer, size_t size, char* pszSrcIP, size_t ip_size, uint16_t& unPort);
int32_t send_data_to(int32_t fd, char* pszBuffer, size_t size, char* pszDstIP, uint16_t unPort);

int32_t get_nic_addrs(struct sockaddr_in* pstNicAddrs, int32_t& count);

int32_t ip_string_to_addr(const char* string_ip, int32_t& ip);
int32_t ip_addr_to_string(int32_t ip, char* string_ip, int16_t buflen);

//msecond的单位: ms
int32_t connect_nonblock(int fd, struct sockaddr_in* serv_addr, socklen_t addrlen, int32_t msecond);

#ifdef __cplusplus
}
#endif

SERVER_END

#endif




