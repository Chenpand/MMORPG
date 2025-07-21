#include "AvalonNetworkAPI.h"

namespace Avalon
{
	void sockaddr_encode(sockaddr_t& addr,const char* ip,UInt16 port)
	{
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);
	}

	void sockaddr_decode(const sockaddr_t& addr,char* ip,UInt16& port)
	{
		ip[0] = 0;
#if defined(AVALON_OS_WINDOWS)
		char* pIp = inet_ntoa(addr.sin_addr);
		if(pIp)
		{
			avalon_strncpy(ip,pIp,AVALON_IPV4_LEN-1);
		}
#else
		inet_ntop(AF_INET, &addr.sin_addr, ip, AVALON_IPV4_LEN);
#endif
		port = ntohs(addr.sin_port);
	}

	sock_t sock_create(int type)
	{
		return socket(PF_INET,type,0);
	}

	bool sock_bind(sock_t sock,const sockaddr_t &addr)
	{
		 return bind(sock,(const sockaddr*)&addr,sizeof(sockaddr_t)) != AVALON_SOCK_ERROR;
	}

	bool sock_connect(sock_t sock,const sockaddr_t &addr)
	{
		int ret =  connect(sock,(const sockaddr*)&addr,sizeof(sockaddr_t));
		return ret != AVALON_SOCK_ERROR;
	}

	bool sock_listen(sock_t sock,int backlog)
	{
		return listen(sock,backlog) != AVALON_SOCK_ERROR;
	}

	sock_t sock_accept(sock_t sock,sockaddr_t* addr)
	{
		sock_t sockret = AVALON_INVALID_SOCK;
		socklen_t socklen = sizeof(sockaddr_t);

		while(true)
		{
			sockret = accept(sock,(sockaddr*)addr,addr != NULL ? &socklen : NULL);
			if(AVALON_INVALID_SOCK == sockret && AVALON_EINTR == sock_error())
			{
				continue;
			}
			return sockret;
		}
	}

	int sock_send(sock_t sock,const void* buf,size_t len,int flags)
	{
		int lenret = AVALON_SOCK_ERROR;

		while(true)
		{
			lenret = send(sock,(const char*)buf,len,flags);
			if(AVALON_SOCK_ERROR == lenret && AVALON_EINTR == sock_error())
			{
				continue;
			}
			return lenret;
		}
	}

	int sock_sendto(sock_t sock,const void* buf,size_t len,const sockaddr_t& addr,int flags)
	{
		int lenret = AVALON_SOCK_ERROR;

		while(true)
		{
			lenret = sendto(sock,(const char*)buf,len,flags,(const sockaddr*)&addr,sizeof(sockaddr_t));
			if(AVALON_SOCK_ERROR == lenret && AVALON_EINTR == sock_error())
			{
				continue;
			}
			return lenret;
		}
	}

	int sock_recv(sock_t sock,void* buf,size_t len, int flags)
	{
		int lenret = AVALON_SOCK_ERROR;

		while(true)
		{
			lenret = recv(sock,(char*)buf,len,flags);
			if(AVALON_SOCK_ERROR == lenret && AVALON_EINTR == sock_error())
			{
				continue;
			}
			return lenret;
		}
	}

	int sock_recvfrom(sock_t sock,void* buf,size_t len,const sockaddr_t& addr,int flags)
	{
		int lenret = AVALON_SOCK_ERROR;
		socklen_t addrlen = sizeof(sockaddr_t);

		while(true)
		{
			lenret = recvfrom(sock,(char*)buf,len,flags,(sockaddr*)&addr,&addrlen);
			if(AVALON_SOCK_ERROR == lenret && AVALON_EINTR == sock_error())
			{
				continue;
			}
			return lenret;
		}
	}

	bool sock_close(sock_t sock)
	{
		int ret = AVALON_SOCK_ERROR;
		
		while(true)
		{
#ifndef AVALON_OS_WINDOWS
			ret = close(sock);
#else
			ret = closesocket(sock);
#endif
			ret = closesocket(sock);
			if(AVALON_SOCK_ERROR == ret && AVALON_EINTR == sock_error())
			{
#ifndef AVALON_OS_WINDOWS
				ret = close(sock);
#else
				ret = closesocket(sock);
#endif
			}
			return ret != AVALON_SOCK_ERROR;
		}
	}

	void sock_shutdown(sock_t sock)
	{
#if defined(AVALON_OS_WINDOWS)
		shutdown(sock, 2);
#else
		shutdown(sock, SHUT_RDWR);
#endif
	}

	bool sock_localname(sock_t sock,sockaddr_t& addr)
	{
		socklen_t socklen = sizeof(sockaddr_t);

		return getsockname(sock,(sockaddr*)&addr,&socklen) != AVALON_SOCK_ERROR;
	}

	bool sock_peername(sock_t sock,sockaddr_t& addr)
	{
		socklen_t socklen = sizeof(sockaddr_t);

		return getpeername(sock,(sockaddr*)&addr,&socklen) != AVALON_SOCK_ERROR;
	}

	bool sock_setopt(sock_t sock,int level,int opt,const void* optval,socklen_t optlen)
	{
		return setsockopt(sock,level,opt,(const char*)optval,optlen) != AVALON_SOCK_ERROR;
	}

	bool sock_getopt(sock_t sock,int level,int opt,void* optval,socklen_t* optlen)
	{
		return getsockopt(sock,level,opt,(char*)optval,optlen) != AVALON_SOCK_ERROR;
	}

	bool sock_setnonblock(sock_t sock)
	{
#if defined(AVALON_OS_WINDOWS)
		u_long val = 1;
		return ioctlsocket(sock,FIONBIO,&val) != AVALON_SOCK_ERROR;
#else
		int opts = fcntl(sock,F_GETFL);
		if(AVALON_SOCK_ERROR == opts) return false;
	
		opts = opts|O_NONBLOCK;
		return fcntl(sock, F_SETFL,opts) != AVALON_SOCK_ERROR;
#endif
	}

	int sock_error()
	{
#if defined(AVALON_OS_WINDOWS)
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	static Logger* gNetLogger = NULL;

	void net_setlogger(Logger* logger){ gNetLogger = logger; }

	Logger* net_getlogger(){ return gNetLogger; }
}
