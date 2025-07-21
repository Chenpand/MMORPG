#include "OBSRouter.h"
#include <CLCenterProtocol.h>
#include <CLSysProtocol.h>
#include <CLGateProtocol.h>

#include "OBSInternalNetwork.h"
#include "RelaySessionMgr.h"

void OBSRouter::OnConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = OBSInternalNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		ErrorLogStream << "can't find connection(" << id << ")." << LogStream::eos;
		return;
	}

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_WORLD)
	{
        InfoLogStream << "add world connection(" << id << ")." << LogStream::eos;
		m_worldConnections[id] = conn;
	}

	if (servType == ST_UNION)
	{
		m_UnionConnections = conn;
	}
}

void OBSRouter::OnDisconnected(UInt32 id)
{
	InfoLogStream << "connection(" << id << ") disconnect." << LogStream::eos;
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_WORLD)
	{
		InfoLogStream << "del world connection(" << id << ")." << LogStream::eos;
		m_worldConnections.erase(id);
	}

	if (servType == ST_UNION)
	{
		InfoLogStream << "del union connection(" << id << ")." << LogStream::eos;
		m_UnionConnections = nullptr;
	}
}


bool OBSRouter::SendToUnion(Avalon::Packet* packet)
{
	if (m_UnionConnections == NULL)
	{
		ErrorLogStream << "can't find Union connection" << LogStream::eos;
		return false;
	}

	return m_UnionConnections->SendPacket(packet);
}

bool OBSRouter::SendToUnion(Avalon::Protocol& protocol)
{
	if (m_UnionConnections == NULL)
	{
		ErrorLogStream << "can't find Union connection." << LogStream::eos;
		return false;
	}

	bool ret = false;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		ret = m_UnionConnections->SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);

	return ret;
}

bool OBSRouter::SendToWorld(UInt32 id, Avalon::Packet* packet)
{
	Avalon::NetConnectionPtr conn = FindConnection(id);
	if (conn == NULL)
	{
		ErrorLogStream << "can't find world connection(" << id << ")." << LogStream::eos;

		conn = OBSInternalNetwork::Instance()->FindConnection(id);
		if (conn == NULL)
		{
			ErrorLogStream << "can't find connection(" << id << ")." << LogStream::eos;
			return false;
		}
	}

	return conn->SendPacket(packet);
}

bool OBSRouter::SendToWorld(UInt32 id, Avalon::Protocol& protocol)
{
	Avalon::NetConnectionPtr conn = FindConnection(id);
	if (conn == NULL)
	{
		ErrorLogStream << "can't find world connection(" << id << ")." << LogStream::eos;

		conn = OBSInternalNetwork::Instance()->FindConnection(id);
		if (conn == NULL)
		{
			ErrorLogStream << "can't find connection(" << id << ")." << LogStream::eos;
			return false;
		}
	}

	bool ret = false;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		ret = conn->SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);

	return ret;
}

bool OBSRouter::SendToPlayer(UInt32 wsNodeId, UInt64 playerId, Avalon::Protocol& protocol)
{
    CLProtocol::SysTransmitToPlayer req;

    req.playerId = playerId;
    if (!protocol.Encode(req.packet))
    {
        return false;
    }
    
    return SendToWorld(wsNodeId, req);
}

bool OBSRouter::SendToPlayer(UInt32 wsNodeId, UInt64 playerId, Avalon::Packet* packet)
{
    CLProtocol::SysTransmitToPlayer req(packet);
    req.playerId = playerId;
    bool ret = SendToWorld(wsNodeId, req);
    req.Detach();

    return ret;
}

bool OBSRouter::SendToWorldPlayer(UInt32 wsNodeId, UInt64 playerId, Avalon::Protocol& protocol)
{
    CLProtocol::GateTransmitPacket req;
    req.id = playerId;
    if (!protocol.Encode(req.packet))
    {
        return false;
    }

    return SendToWorld(wsNodeId, req);
}

bool OBSRouter::SendToWorldPlayer(UInt32 wsNodeId, UInt64 playerId, Avalon::Packet* packet)
{
    CLProtocol::GateTransmitPacket req(packet);
    req.id = playerId;
    bool ret = SendToWorld(wsNodeId, req);
    req.Detach();

    return ret;
}

void OBSRouter::BroadcastToWorld(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (!protocol.Encode(packet))
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	for (auto itr : m_worldConnections)
	{
		Avalon::NetConnectionPtr conn = itr.second;
		if (conn != NULL)
		{
			conn->SendPacket(packet);
		}
	}

	Avalon::Packet::Destroy(packet);
}

void OBSRouter::BroadcastToRelay(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (!protocol.Encode(packet))
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	auto relaySessiones = RelaySessionMgr::Instance()->GetRelaySessiones();
	for (auto session : relaySessiones)
	{
		session.SendPacket(packet);
	}

	Avalon::Packet::Destroy(packet);
}

bool OBSRouter::SendToRelay(UInt32 nodeid, Avalon::Protocol& protocol)
{
	auto relaySessiones = RelaySessionMgr::Instance()->FindSessionByNodeId(nodeid);
	if (!relaySessiones)
	{
		return false;
	}

	relaySessiones->SendProtocol(protocol);
	return true;
}

Avalon::NetConnectionPtr OBSRouter::FindConnection(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_WORLD)
	{
		MapConnection::iterator itr = m_worldConnections.find(id);
		return itr == m_worldConnections.end() ? NULL : itr->second;
	}

	return NULL;
}