#include "AvalonNetService.h"
#include <assert.h>
#include "AvalonNetThread.h"
#include "AvalonNetConnection.h"
#include "AvalonAcceptor.h"

namespace Avalon
{

#if defined(AVALON_OS_WINDOWS)
		class NetInitHelper
		{
		public:
			NetInitHelper()
			{
				WORD wVersionRequested = MAKEWORD( 2, 0);
				WSADATA wsaData;
 
				WSAStartup( wVersionRequested, &wsaData );
			}
			~NetInitHelper()
			{
				WSACleanup();
			}
		};
		static NetInitHelper __helper;
#endif


	NetService::NetService()
	{
		m_pInitThread = NULL;
		m_pIOThreads = NULL;

		m_bRun = false;
	}

	NetService::~NetService()
	{
		Shutdown();
	}

	void NetService::CheckProperties(NetProperties& properties)
	{
		if(properties.num_of_iothread < 1) properties.num_of_iothread = 1;
		if(properties.size_of_socket_buffer < 256) properties.size_of_socket_buffer = 256;
		if(properties.verify_timeout < 5) properties.verify_timeout = 5;
		if(properties.heartbeat_timeout < 10) properties.heartbeat_timeout = 10;
		if(properties.maxnum_of_connection < 1) properties.maxnum_of_connection = 1;
	}

	bool NetService::Startup(NetProperties& properties)
	{
		if(m_bRun) return true;

		CheckProperties(properties);

		m_Properties = properties;
		m_pInitThread = new NetThread(m_Properties.maxnum_of_connection + 100);
		m_pInitThread->SetInterval(Avalon::Time(2));
		m_pIOThreads = new NetThreadGroup(m_Properties.num_of_iothread
			,m_Properties.maxnum_of_connection / m_Properties.num_of_iothread + 10);

		if(!m_pIOThreads->Activate()
			||!m_pInitThread->Activate())
		{
			Shutdown();
			return false;
		}
		
		m_bRun = true;
		return true; 
	}

	void NetService::Shutdown(UInt32 inputTimeout)
	{
		if(!m_bRun) return;

		m_bRun = false;

		m_pInitThread->Stop(inputTimeout);
		m_pInitThread->Join();

		m_pIOThreads->Stop(inputTimeout);
		m_pIOThreads->Join();

		AVALON_DELETE(m_pInitThread);
		AVALON_DELETE(m_pIOThreads);
	}

	bool NetService::Bind(const SockAddr& addr)
	{
		if(!m_bRun) return false;

		Socket socket;

		if(!socket.Create(SOCK_STREAM))
		{
			NETLOG_ERROR("can not create socket (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			return false;
		}

		socket.SetReuseAddr();

		if(!socket.Bind(addr))
		{
			NETLOG_ERROR("can not bind (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			socket.Close();
			return false;
		}

		if(!socket.SetNonblock())
		{
			NETLOG_ERROR("can not setnonblock (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			socket.Close();
			return false;
		}

		if(!socket.Listen(128))
		{
			NETLOG_ERROR("can not listen (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			socket.Close();
			return false;
		}

		Acceptor* acceptor = new Acceptor(socket,this);  //noted by aprilliu ,创建一个接收器，用于处理监听端口到来的连接请求

		m_pInitThread->RegisterHandler(acceptor,NETEVENT_IN | NETEVENT_EXCEPTION);

		NETLOG_INFO("bind netaddress (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());

		return true;
	}

	NetConnectionPtr NetService::Connect(const SockAddr& addr)
	{
		if(!m_bRun) return NULL;

		Socket socket;

		if(!socket.Create(SOCK_STREAM))
		{
			NETLOG_ERROR("can not create socket (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			return NULL;
		}

		if(!socket.Connect(addr))
		{
			socket.Close();
			return NULL;
		}

		if(!InitSocket(socket))
		{
			socket.Close();
			return NULL;
		}

		NetConnection* conn = CreateConnection();
		conn->IncRef();                             //noted by aprilliu, 创建连接，不要忘记增加引用计数
		if(!conn->Open(socket,NCT_ACTIVE,this))
		{
			socket.Close();
			conn->DecRef();   //noted by aprilliu, 关闭连接要减少引用计数
			return NULL;
		}

		NetConnectionPtr ptr = conn;

		NetThread* thread = m_pIOThreads->GetMinHandlerThread();
		thread->RegisterHandler(conn,NETEVENT_IN | NETEVENT_EXCEPTION | NETEVENT_TICK); //by aprilliu, 对于IO线程其相关的Event Handler就是继承于NetConnection的类，比如：CLNetConnection

		NETLOG_INFO("[Connect1]connect to netaddress(%s,%u)!",addr.GetIP().c_str(),addr.GetPort());
		
		return ptr;
	}

	NetConnectionPtr NetService::Connect(UInt32 id,const SockAddr& addr)
	{
		if(!m_bRun) return NULL;

		Socket socket;

		if(!socket.Create(SOCK_STREAM))
		{
			NETLOG_ERROR("can not create socket (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
			return NULL;
		}

		if(!socket.Connect(addr))
		{
			socket.Close();
			return NULL;
		}

		if(!InitSocket(socket))
		{
			socket.Close();
			return NULL;
		}

		NetConnection* conn = CreateConnection();
		conn->IncRef();
		if(!conn->Open(socket,NCT_ACTIVE,this))
		{
			socket.Close();
			conn->DecRef();
			return NULL;
		}
//		socket.Invalidate();

		NetConnectionPtr ptr = conn;
		if(!AddVerifiedConnection(id,conn))
		{
			conn->Close();
			conn->DecRef();
			return NULL;
		}

		NetThread* thread = m_pIOThreads->GetMinHandlerThread();
		thread->RegisterHandler(conn,NETEVENT_IN | NETEVENT_EXCEPTION | NETEVENT_TICK);

		NETLOG_INFO("[Connect2]connect to netaddress(%s,%u)!",addr.GetIP().c_str(),addr.GetPort());
		
		return ptr;
	}

	void NetService::OnConnected(Socket& socket)
	{
		if(!InitSocket(socket))
		{
			socket.Close();
			return;
		}

		Avalon::SockAddr addr;
		socket.GetPeerAddr(addr);

		NetConnection* conn = CreateConnection();
		conn->IncRef();
		if(!conn->Open(socket,NCT_PASSIVE,this))
		{
			socket.Close();
			conn->DecRef();
			return;
		}
//		socket.Invalidate();

		NetThread* thread = m_pIOThreads->GetMinHandlerThread();  //added by aprilliu 2016.01.08 得到负载连接最少的thread
		thread->RegisterHandler(conn,NETEVENT_IN | NETEVENT_EXCEPTION | NETEVENT_TICK);

		NETLOG_INFO("recved new connection (%s:%u)!",addr.GetIP().c_str(),addr.GetPort());
	}

	bool NetService::AddVerifiedConnection(UInt32 id,NetConnection* conn)
	{
		if(conn->GetStatus() == NCS_CLOSED)
		{
			return false;
		}

		m_Mutex.Acquire();

		if(!m_Connections.insert(std::make_pair(id,conn)).second)
		{
			m_Mutex.Release();
			return false;
		}

		conn->SetID(id);
		conn->SetVerified();
		m_Mutex.Release();

		conn->OnAsynConnected();
		OnAsynConnected(id, conn->GetType());

		NETLOG_INFO("connection(%u) is verified[%s]!",conn->GetID(), conn->GetPeerAddr().ToString().c_str());
		return true;
	}

	void NetService::RemoveConnection(NetConnection* conn)
	{
		if(conn == NULL)
		{
			return;
		}

		bool bFind = false;
		m_Mutex.Acquire();
		ConnectionMap::iterator iter = m_Connections.find(conn->GetID());
		if(iter != m_Connections.end())
		{
			bFind = true;;
		}
		m_Mutex.Release();

		if(bFind)
		{
			OnAsynDisconnected(conn->GetID(), conn->GetType());
			conn->OnAsynDisconnected();   //子类没有更多的处理逻辑
			
			m_Mutex.Acquire();
			m_Connections.erase(iter);
			m_Mutex.Release();
		}
	}

	NetConnectionPtr NetService::FindConnection(Avalon::UInt32 id)
	{
		AVALON_AUTOMUTEX(m_Mutex);
		ConnectionMap::iterator iter = m_Connections.find(id);
		if(iter != m_Connections.end())
		{
			return NetConnectionPtr(iter->second);
		}

		return NULL;
	}

	void NetService::VisitConnections(NetConnectionVisitor& visitor)
	{
		m_Mutex.Acquire();
		for(ConnectionMap::iterator iter = m_Connections.begin();
			iter != m_Connections.end();++iter)
		{
			if(!visitor(iter->second))
			{
				break;
			}
		}
		m_Mutex.Release();
	}

	bool NetService::InitSocket(Socket& socket)
	{
		SockAddr addr;
		socket.GetPeerAddr(addr);

		if(!socket.SetNonblock())
		{
			NETLOG_ERROR("can not setnonblock (%s:%u),errno=%d!",addr.GetIP().c_str(),addr.GetPort(),Avalon::Socket::GetError());
			return false;
		}

		int bufsize = m_Properties.size_of_socket_buffer;
		socket.SetOpt(SOL_SOCKET,SO_SNDBUF,&bufsize,sizeof(int));
		socket.SetOpt(SOL_SOCKET,SO_RCVBUF,&bufsize,sizeof(int));

		{
			int realSize = 0;
			socklen_t optlen = 4;
			socket.GetOpt(SOL_SOCKET, SO_SNDBUF, &realSize, &optlen);
			NETLOG_INFO("socket (%s:%u), bufsize=%d, sendSize=%d", addr.GetIP().c_str(), addr.GetPort(), bufsize, realSize);
		}

		{
			int realSize = 0;
			socklen_t optlen = 4;
			socket.GetOpt(SOL_SOCKET, SO_RCVBUF, &realSize, &optlen);
			NETLOG_INFO("socket (%s:%u), bufsize=%d, recvSize=%d", addr.GetIP().c_str(), addr.GetPort(), bufsize, realSize);
		}


		//Nagle算法设置
		int tcp_nodelay = m_Properties.tcp_nodelay;
		socket.SetOpt(IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(int));

		return true;
	}

}
