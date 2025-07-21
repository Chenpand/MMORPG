#include "AvalonAcceptor.h"
#include "AvalonINetReactor.h"
#include "AvalonNetService.h"

namespace Avalon
{
	Acceptor::Acceptor(const Avalon::Socket &sock,NetService* service)
		:m_Sock(sock)
	{
		m_pService = service;
	}

	Acceptor::~Acceptor()
	{
		m_Sock.Shutdown();
	}

	sock_t Acceptor::GetHandle() const
	{
		return m_Sock.GetHandle();
	}

	void Acceptor::HandleInput()
	{
		while(true)
		{
			sock_t sock = m_Sock.Accept();
			if(sock == AVALON_INVALID_SOCK)
			{
				int err = m_Sock.GetError();
				if(err != AVALON_EAGAIN && err !=  AVALON_EWOULDBLOCK)
				{
					NETLOG_ERROR("Accept error = %d",err);
				}
				break;
			}

			Socket mysock(sock);
			m_pService->OnConnected(mysock);
//			mysock.Invalidate();
		}
	}

	void Acceptor::HandleOutput()
	{
	}

	void Acceptor::HandleException()
	{
		NETLOG_ERROR("unexpect exception when accept!");
		GetReactor()->RemoveHandler(this);
	}

	void Acceptor::HandleClose()
	{
		delete this;
	}

	void Acceptor::HandleTick(const Time& now)
	{
	}
}
