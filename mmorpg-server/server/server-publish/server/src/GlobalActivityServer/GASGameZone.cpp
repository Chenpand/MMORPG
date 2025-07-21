#include "GASGameZone.h"

#include <CLSysProtocol.h>
#include <CLGateProtocol.h>
#include "GASInternalNetwork.h"

GASGameZone::GASGameZone()
	:m_Id(0)
{
}

GASGameZone::~GASGameZone()
{
}

bool GASGameZone::Init(UInt32 id)
{
	m_Conn = GASInternalNetwork::Instance()->FindConnection(id);
	if(m_Conn == NULL) return false;
	m_Id = id;
	return true;
}

void GASGameZone::SendPacket(Avalon::Packet* packet)
{
	m_Conn->SendPacket(packet);
}

void GASGameZone::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GASGameZone::TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Packet* packet)
{
	CLProtocol::GateTransmitPacket trans(packet);
	trans.id = DecodeRoleIdFromGASPlayerUId(uid);
	BroadcastToGate(adminNode, trans);
	trans.Detach();
}

void GASGameZone::TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		TransmitPlayerMsg(uid, adminNode, packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GASGameZone::TransmitToAdmin(UInt32 adminNode, Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = adminNode;
	if (protocol.Encode(trans.packet))
	{
		SendProtocol(trans);
	}
}

void GASGameZone::BroadcastToGate(UInt32 adminNode, Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitFromGlobalToNode trans;
	trans.destServer = ST_GATE;
	if (protocol.Encode(trans.packet))
	{
		TransmitToAdmin(adminNode, trans);
	}
}

void GASGameZone::BroadcastToGate(Avalon::Protocol& protocol)
{
	CLProtocol::GateBroadcastAll broadcastall;
	if (protocol.Encode(broadcastall.packet))
	{
		CLProtocol::SysTransmitFromGlobalToNode trans;
		trans.destServer = ST_GATE;
		if (broadcastall.Encode(trans.packet))
		{
			SendProtocol(trans);
		}
	}
}

void GASGameZone::BroadcastToWorld(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitFromGlobalToNode trans;
	trans.destServer = ST_WORLD;
	if (protocol.Encode(trans.packet))
	{
		SendProtocol(trans);
	}
}

void GASGameZone::TransmitToWorld(UInt32 adminNode, Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitFromGlobalToNode trans;
	trans.destServer = ST_WORLD;
	if (protocol.Encode(trans.packet))
	{
		TransmitToAdmin(adminNode, trans);
	}
}
