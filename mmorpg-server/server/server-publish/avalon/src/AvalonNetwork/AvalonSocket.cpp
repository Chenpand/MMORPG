#include "AvalonSocket.h"
#include "AvalonStringUtil.h"

namespace Avalon
{
	bool SockAddr::FromString(const std::string& str)
	{
		std::vector<std::string> tokens;
		Avalon::StringUtil::Split(str,tokens,":");
		if(tokens.size() < 2) return false;

		SetIP(tokens[0]);
		SetPort(Avalon::StringUtil::ConvertToValue<UInt32>(tokens[1]));
		return true;
	}

	std::string SockAddr::ToString() const
	{
		char addrbuf[32];
		memset(addrbuf, 0, sizeof(addrbuf));
		avalon_snprintf(addrbuf, 32, "%s:%u", m_IP.c_str(), m_Port);
		return addrbuf;
	}

	void SockAddr::SetAddr(const sockaddr_t& addr)
	{
		char ip[AVALON_IPV4_LEN];
		sockaddr_decode(addr,ip,m_Port);
		m_IP = ip;
	}

	void SockAddr::GetAddr(sockaddr_t &addr) const
	{
		sockaddr_encode(addr,m_IP.c_str(),m_Port);
	}



	bool Socket::Create(int type)
	{
		m_Sock = sock_create(type);
		return m_Sock != AVALON_INVALID_SOCK;
	}

	bool Socket::Bind(const SockAddr& addr)
	{
		sockaddr_t _addr;
		addr.GetAddr(_addr);
		return sock_bind(m_Sock,_addr);
	}

	bool Socket::Connect(const SockAddr& addr)
	{
		sockaddr_t _addr;
		addr.GetAddr(_addr);
		return sock_connect(m_Sock,_addr);
	}

	bool Socket::Listen(int backlog)
	{
		return sock_listen(m_Sock,backlog);
	}

	sock_t Socket::Accept()
	{
		return sock_accept(m_Sock,NULL);
	}

	int Socket::Send(const void* buf, Int32 len, int flags)
	{
		return sock_send(m_Sock,buf,len,flags);
	}

	int Socket::Recv(void* buf, Int32 len, int flags)
	{
		return sock_recv(m_Sock,buf,len,flags);
	}

	int Socket::SendTo(const void* buf,size_t len,const SockAddr& addr,int flags)
	{
		sockaddr_t _addr;
		addr.GetAddr(_addr);
		return sock_sendto(m_Sock,buf,len,_addr,flags);
	}

	int Socket::RecvFrom(void* buf,size_t len,SockAddr& addr,int flags)
	{
		sockaddr_t _addr;
		addr.GetAddr(_addr);
		return sock_recvfrom(m_Sock,buf,len,_addr,flags);
	}

	void Socket::Close()
	{
		if(m_Sock != AVALON_INVALID_SOCK)
		{
			sock_close(m_Sock);
			m_Sock = AVALON_INVALID_SOCK;
		}
	}

	void Socket::Shutdown()
	{
		if(m_Sock != AVALON_INVALID_SOCK)
		{
			sock_shutdown(m_Sock);
			m_Sock = AVALON_INVALID_SOCK;
		}
	}

/*	void Socket::Invalidate()
	{
		m_Sock = AVALON_INVALID_SOCK;
	}
*/
	sock_t Socket::GetHandle() const
	{
		return m_Sock;
	}

	bool Socket::GetSockAddr(SockAddr& addr)
	{
		sockaddr_t _addr;
		if(sock_localname(m_Sock,_addr))
		{
			addr.SetAddr(_addr);
			return true;
		}
		return false;
	}

	bool Socket::GetPeerAddr(SockAddr& addr)
	{
		sockaddr_t _addr;
		if(sock_peername(m_Sock,_addr))
		{
			addr.SetAddr(_addr);
			return true;
		}
		return false;
	}

	bool Socket::SetOpt(int level,int opt,const void* optval,socklen_t optlen)
	{
		return sock_setopt(m_Sock,level,opt,optval,optlen);
	}

	bool Socket::GetOpt(int level,int opt,void* optval,socklen_t* optlen)
	{
		return sock_getopt(m_Sock,level,opt,optval,optlen);
	}

	bool Socket::SetNonblock()
	{
		return sock_setnonblock(m_Sock);
	}

	bool Socket::SetReuseAddr()
	{
		Int32 reuse = 1;
		Int32 len = sizeof(Int32);
		return SetOpt(SOL_SOCKET,SO_REUSEADDR,&reuse,len);
	}

	int Socket::GetError()
	{
		return sock_error();
	}
}
