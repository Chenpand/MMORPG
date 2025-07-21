#include "CLInternalNetwork.h"
#include <AvalonThread.h>
#include <AvalonProcessUnit.h>
#include <AvalonStringUtil.h>
#include "CLSysProtocol.h"
#include "CLErrorCode.h"

#include "CLNetworkPerformance.h"

CLInternalConnection::CLInternalConnection(CLNetwork* network):CLNetConnection(network)
{
}

CLInternalConnection::~CLInternalConnection()
{
}

bool CLInternalConnection::PreHandlePacket(Avalon::Packet *packet)
{
	if(CLNetConnection::PreHandlePacket(packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::SYS_LOGIN_REQ:
		{
			if(IsVerified()) return true;

			CLProtocol::SysLoginReq req;
			req.Decode(packet);

			UInt32 zoneId = CL_GET_ZONEID_FROM_NODEID(req.nodeid);
			UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(req.nodeid);
			UInt32 servId = CL_GET_SERVERID_FROM_NODEID(req.nodeid);
			if(IsAdminServer(SERVER_TYPE) && !IsSelfConfigAddress(servType))
			{
				zoneId = ZONE_ID;
			}
			UInt32 nodeid = CL_MAKE_NODEID(zoneId,servType,servId);

			CLProtocol::SysLoginRet ret;
			ret.nodeid = NODE_ID;
			ret.result = ErrorCode::SUCCESS;
			SendProtocol(ret);

			InfoLogStream << "recv login req service(" << zoneId << ", " << servType << "," << servId << ") from " << this->GetPeerAddr().ToString() << LogStream::eos;

			if(!GetNetwork()->AddVerifiedConnection(nodeid,this))
			{
				ErrorLogStream << "internal connection(" << zoneId << ")(" <<
					servType << ")(" << servId << ") is already existed!" << LogStream::eos;
				Close();
				return true;
			}

		}
		return true;
	case CLProtocol::SYS_LOGIN_RET:
		{
			if(IsVerified()) return true;

			CLProtocol::SysLoginRet ret;
			ret.Decode(packet);

			UInt32 zoneId = CL_GET_ZONEID_FROM_NODEID(ret.nodeid);
			UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(ret.nodeid);
			UInt32 servId = CL_GET_SERVERID_FROM_NODEID(ret.nodeid);

			if(ret.result == ErrorCode::SYS_REPEAT_LOGIN)
			{
				ErrorLogStream << "repeat login to node(" << zoneId << ")("
					<< servType << ")(" << servId << ")." << LogStream::eos;
				Close();
				return true;
			}

			InfoLogStream << "recv SYS_LOGIN_RET from (" << ret.nodeid << "), and to call AddVerifiedConnection" << LogStream::eos;

			if(!GetNetwork()->AddVerifiedConnection(ret.nodeid,this))
			{
				ErrorLogStream << "internal connection(" << zoneId << ")("
					<< servType << ")(" << servId << ") is already existed!" << LogStream::eos;
				Close();
				return true;
			}

			if(IsAdminServer(servType) && !IsSelfConfigAddress(SERVER_TYPE))
			{
				ZONE_ID = zoneId;				//这样就从Admin Server获得了服务区ID

				//请求网络地址列表
				CLProtocol::SysNetAddressReq req;
				if(req.Encode(packet))
				{
					SendPacket(packet);
				}
			}
		}
		return true;
	}

	return false;
}

void CLInternalConnection::ProcessSendPacket(Avalon::Packet *&packet)
{
	CLNetworkPerfMgr::Instance()->OnSendPacket(GetID(), packet->GetID(), packet->GetSize());
}

CLInternalNetwork::CLInternalNetwork()
{
	m_pAddress = NULL;
	m_bGotInfo = false;
	m_bWaitShutdown = false;

	m_ShutdownTimer.SetInterval(1000);
}

CLInternalNetwork::~CLInternalNetwork()
{
}

bool CLInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if(properties == NULL || addresses == NULL) return false;
	m_pAddress = addresses;

	if(!CLNetwork::Startup(properties))
	{
		ErrorLogStream << "startup network failed!" << LogStream::eos;
		return false;
	}
	
	UInt32 nodeId = 0;
	//如果是自配置
	if(IsSelfConfigAddress(SERVER_TYPE))
	{
		nodeId = NODE_ID;
		Avalon::SockAddr addr;
		if(addr.FromString(properties->GetProperty(GetName(),"bind_address").GetString()))
		{
			m_pAddress->AddInternalAddress(nodeId, addr);
		}
	}
	else
	{
		if(!m_AdminAddr.FromString(properties->GetProperty(GetName(),"adminserver_address").GetString())) return false;

		if(!GetInfoFromAdminServer())
		{
			ErrorLogStream << "can not get servers' info!" << LogStream::eos;
			CLNetwork::Shutdown(0);
			return false;
		}

		nodeId = NODE_ID;
	}

	Avalon::SockAddr addr;
	if(m_pAddress->FindInternalAddress(nodeId, addr))
	{
		if(!Bind(addr))
		{
			CLNetwork::Shutdown(0);
			return false;
		}
	}

	// 格式:auto_connect=7|8|9|11  具体是服务器类型
	std::string autoconnect = properties->GetProperty(GetName(),"auto_connect").GetString();
	std::vector<std::string> typestr;
	Avalon::StringUtil::Split(autoconnect,typestr,"|");

	for(std::vector<std::string>::iterator iter = typestr.begin();iter != typestr.end();++iter)
	{
		UInt32 type = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
		if (!m_pAddress->GetInternalAddresses(type, m_ConnectAddrs)) //根据"auto_connect"配置 得到需要主动去连接的服务器地址
		{
			ErrorLogStream << "in config file, autoconnect to server(type:" << type << "),but can't find its address" << LogStream::eos;
			return false;
		}
		
	}

	ConnectToServers();

	int times = CONNECT_TIMEWAIT;
	while(!IsAllConnected() && times-- > 0) Avalon::Thread::Sleep(1000);

	if(times <= 0)
	{
		ErrorLogStream << "can not connect to all servers!" << LogStream::eos;
		CLNetwork::Shutdown(0);
		return false;
	}

	return true;
}

void CLInternalNetwork::Shutdown(UInt32 inputTimeout)
{
	CLNetwork::Shutdown(inputTimeout);
}

void CLInternalNetwork::OnTick(const Avalon::Time& now)
{
	for(ReconnectTimerVec::iterator iter = m_ReconnectTimers.begin();
		iter != m_ReconnectTimers.end();)
	{
		Avalon::NetConnectionPtr conn = FindConnection(iter->nodeId);
		if(conn != NULL)
		{
			iter = m_ReconnectTimers.erase(iter);
		}
		else
		{ 
			if(iter->timer.IsTimeout(now))
			{
				SockAddrMap::iterator sockIter = m_ConnectAddrs.find(iter->nodeId);
				if(sockIter != m_ConnectAddrs.end())
				{
					InfoLogStream << "start to reconnect to server " << sockIter->second.ToString() << LogStream::eos;
					Avalon::NetConnectionPtr conn = Connect(sockIter->second);
					if(conn != NULL)
					{
						CLProtocol::SysLoginReq req;
						req.nodeid = NODE_ID;
						CL_SEND_PROTOCOL(conn,req);
					}
				}
			}
			++iter;
		}
	}

	if(m_bWaitShutdown)
	{
		CheckShutdownService();
	}

	CLNetwork::OnTick(now);
}

bool CLInternalNetwork::PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet)
{
	if(CLNetwork::PreHandlePacket(conn,packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::SYS_NETADDRESS_REQ:
		{
			if(m_pAddress == NULL) return true;

			CLProtocol::SysNetAddressReq req;
			req.Decode(packet);

			CLProtocol::SysNetAddressRet ret(m_pAddress);
			CL_SEND_PROTOCOL(conn, ret);
		}
		return true;
	case CLProtocol::SYS_NETADDRESS_RET:
		{
			if(m_pAddress == NULL) return true;
			
			CLProtocol::SysNetAddressRet ret(m_pAddress);  //从返回的协议中解码得到地址列表
			ret.Decode(packet);
			m_bGotInfo = true;
		}
		return true;
	}

	return false;
}

Avalon::NetConnection* CLInternalNetwork::CreateConnection()
{
	return new CLInternalConnection(this);
}

class BroadcastVisitor : public Avalon::NetConnectionVisitor
{
public:
	BroadcastVisitor(ServerType type,Avalon::Packet* packet)
	{
		m_Type = type;
		m_pCopyPacket = Avalon::Packet::Create();
		m_pPacket = packet;
	}
	~BroadcastVisitor()
	{
		Avalon::Packet::Destroy(m_pCopyPacket);
	}

	bool operator()(Avalon::NetConnection* conn)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(conn->GetID());
		if(servType == m_Type)
		{
			m_pPacket->CopyTo(m_pCopyPacket);
			conn->SendPacket(m_pCopyPacket);
		}
		return true;
	}

private:
	UInt32 m_Type;
	Avalon::Packet* m_pCopyPacket;
	Avalon::Packet* m_pPacket;
};

void CLInternalNetwork::Broadcast(ServerType type,Avalon::Packet* packet)
{
	BroadcastVisitor visitor(type,packet);
	VisitConnections(visitor);
}

void CLInternalNetwork::CheckShutdownService()
{
	m_bWaitShutdown = true;

	class CheckVisitor : public Avalon::NetConnectionVisitor
	{
	public:
		CheckVisitor(bool bNotify):m_bShutdown(true),m_bNotify(bNotify){}
		~CheckVisitor(){}

		bool operator()(Avalon::NetConnection* conn)
		{
			if(conn->GetType() == Avalon::NCT_PASSIVE)
			{
				m_bShutdown = false;
				if(m_bNotify)
				{
					CLProtocol::SysNotifyShutdown protocol;
					CL_SEND_PROTOCOL(conn, protocol);
				}
			}
			return true;
		}

		bool NeedShutdown() const { return m_bShutdown; }

	private:
		//是否需要停止服务
		bool m_bShutdown;
		//是否通知
		bool m_bNotify;
	};

	CheckVisitor visitor(m_ShutdownTimer.IsTimeout(Avalon::Time::CurrentTime()));
	VisitConnections(visitor);

	if(visitor.NeedShutdown())
	{
		OnShutdownService();
	}
}

bool CLInternalNetwork::GetInfoFromAdminServer()
{
	if(SERVER_TYPE == ST_ADMIN) return false; 
	Avalon::NetConnectionPtr conn = Connect(m_AdminAddr);
	if(conn == NULL) return false;

	CLProtocol::SysLoginReq req;
	req.nodeid = NODE_ID;

	CL_SEND_PROTOCOL(conn,req);

	UInt32 times = 0;
	while(!m_bGotInfo && ++times < 300)
	{
		Avalon::ProcessUnit::Sleep(10);
	}

	return m_bGotInfo;
}

void CLInternalNetwork::ConnectToServers()
{
	for (SockAddrMap::iterator iter = m_ConnectAddrs.begin(); iter != m_ConnectAddrs.end(); ++iter)
	{
		InfoLogStream << "nodeid:" << iter->first << " IP=" << iter->second.GetIP().c_str() << ":" << iter->second.GetPort() << LogStream::eos;
	}

	for(SockAddrMap::iterator iter = m_ConnectAddrs.begin();iter != m_ConnectAddrs.end();++iter)
	{
		const DisconnectHandleCfg* cfg = GetDisconnectHandleCfg(ServerType(CL_GET_SERVERTYPE_FROM_NODEID(iter->first)));
		if(cfg != NULL && cfg->handleType == DHT_RECONNECT && cfg->servType != ST_CENTER)
		{
			ReconnectTimer timer;
			timer.nodeId = iter->first;
			timer.timer.SetInterval(cfg->retry_inter);
			m_ReconnectTimers.push_back(timer);
		}
		else
		{
			InfoLogStream << "to connect nodeid:" << iter->first << " IP=" << iter->second.GetIP().c_str() << ":" << iter->second.GetPort() << LogStream::eos;
			Avalon::NetConnectionPtr conn = Connect(iter->second);
			if(conn != NULL)
			{
				CLProtocol::SysLoginReq req;
				req.nodeid = NODE_ID;

				CL_SEND_PROTOCOL(conn,req);
			}
		}
	}
	Avalon::Thread::Sleep(200);
}

bool CLInternalNetwork::IsAllConnected()
{
	bool ret = true;
	for(SockAddrMap::iterator iter = m_ConnectAddrs.begin();iter != m_ConnectAddrs.end();++iter)
	{
		InfoLogStream << "in [CLInternalNetwork::IsAllConnected]to check nodeid:" << iter->first << " IP=" << iter->second.GetIP().c_str() << ":" << iter->second.GetPort() << LogStream::eos;
		const DisconnectHandleCfg* cfg = GetDisconnectHandleCfg(ServerType(CL_GET_SERVERTYPE_FROM_NODEID(iter->first)));
		if(cfg != NULL && cfg->handleType == DHT_RECONNECT && cfg->servType != ST_CENTER) continue;

		Avalon::NetConnectionPtr conn = FindConnection(iter->first);
		if(conn == NULL)
		{
			InfoLogStream << "can't find connect(" << iter->first << "), and connect to (" << iter->second.GetIP().c_str() << ":" << iter->second.GetPort() << ")" << LogStream::eos;
			conn = Connect(iter->second);
			if(conn != NULL)
			{
				CLProtocol::SysLoginReq req;
				req.nodeid = NODE_ID;

				CL_SEND_PROTOCOL(conn,req);
			}
			ret = false;
		}
	}
	return ret;
}

bool CLInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if(packet->GetID() == CLProtocol::SYS_NOTIFY_DISCONNECTED)
	{
		CLProtocol::SysNotifyDisconnected protocol;
		protocol.Decode(packet);

		if (protocol.type == Avalon::NCT_PASSIVE)
		{
			OnDisconnected(protocol.id);
			return true;
		}

		if (CL_GET_SERVERTYPE_FROM_NODEID(protocol.id) == SERVER_TYPE)
		{
			// 自己跟自己断开了
			ErrorLogStream << "self service(" << protocol.id << ") disconnected." << LogStream::eos;
			OnDisconnected(protocol.id);
			return true;
		}

		const DisconnectHandleCfg* cfg = GetDisconnectHandleCfg(ServerType(CL_GET_SERVERTYPE_FROM_NODEID(protocol.id)));
		if(cfg == NULL)
		{
			OnDisconnected(protocol.id);
			return true;
		}

		switch(cfg->handleType)
		{
		case DHT_RECONNECT:
			{
				InfoLogStream << "conn " << protocol.id << " disconnected, start to reconnect." << LogStream::eos;

				for(ReconnectTimerVec::iterator iter = m_ReconnectTimers.begin();
					iter != m_ReconnectTimers.end(); ++iter)
				{
					if(iter->nodeId == protocol.id) return true;
				}
				ReconnectTimer timer;
				timer.nodeId = protocol.id;
				timer.timer.SetInterval(cfg->retry_inter);
				m_ReconnectTimers.push_back(timer);
				OnDisconnected(protocol.id);
			}
			break;
		case DHT_SHUTDOWN:
			{
				ErrorLogStream << "key connection " << protocol.id << " is closed!" << LogStream::eos;
				CheckShutdownService();
			}
			break;
		case DHT_NOTIFY:
			{
				OnDisconnected(protocol.id);
			}
			break;
		}

		return true;
	}
	else if(packet->GetID() == CLProtocol::SYS_NOTIFY_SHUTDOWN)
	{
		DebugLogStream << "service(" << packet->GetConnID() << ") is shutdown." << LogStream::eos;
		InfoLogStream << "[server_shutdown] recved shutdown service notify from connection "
			<< packet->GetConnID() << "!" << LogStream::eos;
		OnShutdownService();
		return false;
	}
	else
	{
		return CLNetwork::ExecutePacket(packet);
	}
}
