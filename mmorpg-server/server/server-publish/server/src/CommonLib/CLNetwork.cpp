#include "CLNetwork.h"
#include "CLSysProtocol.h"
#include "CLPacketExecutor.h"

#include "CLNetworkPerformance.h"

bool CL_SEND_PROTOCOL(Avalon::NetConnectionPtr Conn, Avalon::Protocol& Req)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (!Req.Encode(packet))
	{
		Avalon::Packet::Destroy(packet);
		return false;
	}
	Conn->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
	return true;
}

CLNetConnection::CLNetConnection(CLNetwork *network)
{
	m_pNetwork = network;
}

CLNetConnection::~CLNetConnection()
{
}

void CLNetConnection::OnDataReached()
{
	Avalon::Packet* packet = RecvPacket();
	while(packet != NULL)
	{
		if(m_pNetwork->PreHandlePacket(this,packet))
		{
			Avalon::Packet::Destroy(packet);
		}
		else
		{
			m_pNetwork->DispatchPacket(packet);
		}
		packet = RecvPacket();  //noted by aprilliu  此处循环接收协议包 是因为可能已经收到多个完整协议包，所以此处的逻辑是尽可能处理完所有已收到的包
	}
}

bool CLNetConnection::SendProtocol(Avalon::Protocol& protocol)
{
	bool ret = false;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		ret = SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
	return ret;
}

void CLNetConnection::OnTick(const Avalon::Time& now)
{
}



CLNetwork::CLNetwork()
{
}

CLNetwork::~CLNetwork()
{
	while(!m_Queue.empty())
	{
		Avalon::Packet* packet = m_Queue.pop();
		Avalon::Packet::Destroy(packet);
	}
}

const std::string CLNetwork::GetName() const
{
	return GetNetworkNameByType(GetType());
}

Avalon::NetConnectionPtr CLNetwork::FindConnectionByType(ServerType type)
{
	class FindConnVisitor : public Avalon::NetConnectionVisitor
	{
	public:
		FindConnVisitor(ServerType type):m_Type(type),m_pConn(NULL){}

		bool operator()(Avalon::NetConnection* conn)
		{
			if(m_Type == int(CL_GET_SERVERTYPE_FROM_NODEID(conn->GetID())))
			{
				m_pConn = conn;
				return false;
			}
			return true;
		}

		Avalon::NetConnection* GetConnection() const
		{
			return m_pConn;
		}

	private:
		ServerType m_Type;
		Avalon::NetConnection* m_pConn;
	};
	FindConnVisitor visitor(type);
	VisitConnections(visitor);
	return visitor.GetConnection();
}

void CLNetwork::FindConnectionByType(ServerType type, std::vector<Avalon::NetConnectionPtr>& connVec)
{
	class FindConnVisitor : public Avalon::NetConnectionVisitor
	{
	public:
		FindConnVisitor(ServerType type, std::vector<Avalon::NetConnectionPtr>& vec) :m_Type(type), 
			connVec(vec) {}

		bool operator()(Avalon::NetConnection* conn)
		{
			if (m_Type == int(CL_GET_SERVERTYPE_FROM_NODEID(conn->GetID())))
			{
				connVec.push_back(conn);
				return true;
			}

			return true;
		}
	
	private:
		ServerType m_Type;
		std::vector<Avalon::NetConnectionPtr>& connVec;
	};

	FindConnVisitor visitor(type, connVec);
	VisitConnections(visitor);
}

bool CLNetwork::Startup(Avalon::PropertySet* properties)
{
	std::string name = GetName();
	
	RegisterProtocols();

	Avalon::NetProperties param;
	param.num_of_iothread = properties->GetProperty(name,"num_of_iothread");
	param.size_of_socket_buffer = properties->GetProperty(name,"size_of_socket_buffer");
	param.send_wait = properties->GetProperty(name,"send_wait");
	param.verify_timeout = 15;
	param.heartbeat_timeout = properties->GetProperty(name,"heartbeat_interval");
	param.maxnum_of_connection = properties->GetProperty(name,"max_num_of_connections");
	param.checkBodySize = properties->GetProperty(name, "check_body_size");

	InfoLogStream << "[NetworkParam] " << name << " checkBodySize(" << (param.checkBodySize ? 1 : 0) << ")." << LogStream::eos;

	return NetService::Startup(param);
}

void CLNetwork::Shutdown(UInt32 inputTimeout)
{
	NetService::Shutdown(inputTimeout);
	CLNetwork::OnTick(CURRENT_TIME);
}

bool CLNetwork::PreHandlePacket(CLNetConnection* conn,Avalon::Packet *packet)
{
	return false; //notedy by aprilliu, 特别注意返回值是false
}

Avalon::NetConnection* CLNetwork::CreateConnection()
{
	return new CLNetConnection(this);
}

void CLNetwork::OnAsynConnected(UInt32 id,  Avalon::NetConnectionType type)
{
	CLProtocol::SysNotifyConnected protocol;
	protocol.id = id;
	protocol.type = type;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		DispatchPacket(packet);
	}
	else
	{
		Avalon::Packet::Destroy(packet);
	}
}

void CLNetwork::OnAsynDisconnected(UInt32 id,  Avalon::NetConnectionType type)
{
	CLProtocol::SysNotifyDisconnected protocol;
	protocol.id = id;
	protocol.type = type;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		DispatchPacket(packet);
	}
	else
	{
		Avalon::Packet::Destroy(packet);
	}
}

void CLNetwork::DispatchPacket(Avalon::Packet *packet)
{
	m_Queue.push(packet);
}

void CLNetwork::OnTick(const Avalon::Time& now)
{
	Avalon::Time startTime = now;

	UInt32 packetNum = 0;
	bool bContinue = true;
	while(bContinue && !m_Queue.empty())
	{
		Avalon::Packet* packet = m_Queue.pop();

		// 收包统计
		CLNetworkPerfMgr::Instance()->OnRecvPacket(packet->GetConnID(), packet->GetID(), packet->GetSize());

		if (!ExecutePacket(packet))
		{
			FatalLogStream << "[Network] ExecutePacket(" << packet->GetID() << ") failed." << LogStream::eos;
		}
		
		if (!bContinue)
		{
			ErrorLogStream << "can't find handler, protocol id is" << packet->GetID() << LogStream::eos;
			bContinue = true;
		}
		Avalon::Packet::Destroy(packet);

		// 一次Tick不能超过10ms
		if ((SERVER_TYPE == ST_WSCENE || SERVER_TYPE == ST_SCENE) && ++packetNum >= MAX_PACKET_PERTICK)
		{
			WarnLogStream << "network tick too long, over " << packetNum << " packet." << LogStream::eos;
			break;
		}
	}

	CLNetworkPerfMgr::Instance()->OnTick(now);
}

bool CLNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if(packet->GetID() == CLProtocol::SYS_NOTIFY_CONNECTED)
	{
		CLProtocol::SysNotifyConnected protocol;
		protocol.Decode(packet);
		OnConnected(protocol.id);
	}
	else if(packet->GetID() == CLProtocol::SYS_NOTIFY_DISCONNECTED)
	{
		CLProtocol::SysNotifyDisconnected protocol;
		protocol.Decode(packet);
		OnDisconnected(protocol.id);
	}
	else
	{
		return m_Executor.ExecutePacket(packet);
	}
	
	return true;
}

