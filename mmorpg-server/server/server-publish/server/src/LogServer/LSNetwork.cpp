#include "LSNetwork.h"

#include <sstream>
#include <AvalonMutex.h>
#include <AvalonMd5.h>
#include <AvalonPropertySet.h>
#include <CLSysProtocol.h>

#include "LSApplication.h"

LogConnection::LogConnection(CLNetwork* network)
	:CLNetConnection(network)
{
}

LogConnection::~LogConnection()
{
}

void LogConnection::OnDataReached()
{
	do
	{
		Avalon::Packet* packet = RecvPacket();
		if(packet == NULL) break;

		GetNetwork()->DispatchPacket(packet);
	}
	while(true);
}

void LogConnection::OnTick(const Avalon::Time& now)
{
	CLNetConnection::OnTick(now);
}

bool LogConnection::PreHandlePacket(Avalon::Packet* packet)
{
	if(CLNetConnection::PreHandlePacket(packet)) return true;

	if(!IsVerified())
	{
		UInt32 id = GenID();
		if(!GetNetwork()->AddVerifiedConnection(id, this))
		{
			ErrorLogStream << "repeat log connection id:" << id << "!" << LogStream::eos;
			Close();
			return true;
		}

		packet->SetConnID(GetID());
	}

	return false;
}

	
UInt32 g_LogIdSeed = 0;
Avalon::Mutex g_LogMutex;

UInt32 LogConnection::GenID()
{
	UInt32 ret = 0;
	g_LogMutex.Acquire();
	if(++g_LogIdSeed == 0) g_LogIdSeed = 1;
	ret = g_LogIdSeed;
	g_LogMutex.Release();
	return ret;
}


LogNetwork::LogNetwork()
{
}

LogNetwork::~LogNetwork()
{
}

Avalon::NetConnection* LogNetwork::CreateConnection()
{
	return new LogConnection(this);
}

bool LogNetwork::Startup(Avalon::PropertySet* properties)
{
	if(!CLNetwork::Startup(properties))
	{
		return false;
	}

	std::string addrstr = properties->GetProperty(GetName(), "bind_address").GetString();
	Avalon::SockAddr addr;
	if(!addr.FromString(addrstr)) return false;

	if(!Bind(addr))
	{
		ErrorLogStream << "bind addr "  << addrstr << " failed!" << LogStream::eos;  
		return false;
	}

	if(!m_UdpSock.Create(SOCK_DGRAM)) return false;
	m_UdpSock.SetReuseAddr();

	addrstr = properties->GetProperty("LogNetwork", "bind_address").GetString();
	Avalon::SockAddr udpaddr;
	if(!udpaddr.FromString(addrstr)) return false;

	if(!m_UdpSock.Bind(udpaddr))
	{
		ErrorLogStream << "bind addr " << addrstr << " failed!" << LogStream::eos;
		return false;
	}

	m_UdpSock.SetNonblock();

	Int32 bufsize = properties->GetProperty("LogNetwork", "size_of_socket_buffer");
	if(bufsize < 10240) bufsize = 10240;
	m_UdpSock.SetOpt(SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	m_UdpSock.SetOpt(SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));

	return true;
}

void LogNetwork::Shutdown(UInt32 inputTimeout)
{
	m_UdpSock.Close();

	CLNetwork::Shutdown(inputTimeout);
}

void LogNetwork::OnTick(const Avalon::Time& now)
{
	CLNetwork::OnTick(now);

	Avalon::Packet* packet = Avalon::Packet::Create();
	int ret = 0;
	Avalon::SockAddr addr;
	while((ret = m_UdpSock.RecvFrom(packet->GetBuffer() - Avalon::Packet::HEAD_SIZE
		, packet->GetMaxSize() + Avalon::Packet::HEAD_SIZE, addr)) > 0)
	{
		if(ret < Avalon::Packet::HEAD_SIZE) continue;

		Avalon::UInt8* header = packet->GetBuffer() - Avalon::Packet::HEAD_SIZE;
		
		packet->SetSize(*(UInt16*)header);
		packet->SetID(*(UInt32*)(header + sizeof(UInt16)));

		if(ret != packet->GetSize() + Avalon::Packet::HEAD_SIZE) continue;

		if(packet->GetID() == CLProtocol::SYS_NET_LOGGER)
		{
			ProcessDataLogPacket(packet, addr);
		}
	}
	Avalon::Packet::Destroy(packet);
}

void LogNetwork::OnConnected(UInt32 id)
{
}

void LogNetwork::OnDisconnected(UInt32 id)
{
}

void LogNetwork::ProcessDataLogPacket(Avalon::Packet* packet, const Avalon::SockAddr& addr)
{
	CLProtocol::SysNetLogger protocol;
	if(!protocol.Decode(packet))
	{
		ErrorLogStream << "decode log protocol failed! " << addr.ToString() << LogStream::eos;
		return;
	}

	std::string logName;
	logName = "datalog_" + protocol.name;
	Avalon::Logger* pLogger = Avalon::Logger::getInstance(logName.c_str());
	if(pLogger == NULL) return;

	Avalon::LogEvent logEvent;
	logEvent.time = Avalon::Time(protocol.now);
	logEvent.priority = Avalon::LogPriority(protocol.priority);
	avalon_strncpy(logEvent.content, protocol.content.c_str(), Avalon::LogEvent::MAX_LOGGER_LEN);
	pLogger->Log(logEvent.priority, logEvent);
}
