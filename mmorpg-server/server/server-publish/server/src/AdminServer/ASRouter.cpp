#include "ASRouter.h"
#include "ASInternalNetwork.h"

#include <CLCenterProtocol.h>

namespace Router
{
	Avalon::NetConnectionPtr g_LoginConn = NULL;

	Avalon::NetConnectionPtr g_WorldConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_LOGIN)
		{
			g_LoginConn = ASInternalNetwork::Instance()->FindConnection(id);
		}
		else if(servType == ST_WORLD)
		{
			g_WorldConn = ASInternalNetwork::Instance()->FindConnection(id);
		}
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_LOGIN)
		{
			g_LoginConn = NULL;
		}
		else if(servType == ST_WORLD)
		{
			g_WorldConn = NULL;
		}
	}

	bool IsWorldConnected()
	{
		return g_WorldConn != NULL;
	}

	void SendToLogin(Avalon::Protocol& protocol)
	{
		if(g_LoginConn != NULL)
		{
			CL_SEND_PROTOCOL(g_LoginConn,protocol);
		}
	}

	void SendToWorld(Avalon::Packet* packet)
	{
		if(g_WorldConn != NULL)
		{
			g_WorldConn->SendPacket(packet);
		}
	}

	void SendToWorld(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToWorld(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToGate(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			ASInternalNetwork::Instance()->Broadcast(ST_GATE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToScene(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			ASInternalNetwork::Instance()->Broadcast(ST_SCENE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToZone(UInt32 zoneId, Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToZone(zoneId, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToZone(UInt32 zoneId, Avalon::Packet* packet)
	{
		CLProtocol::CenterTransmitToZone req(packet);
		req.zoneId = zoneId;
		Avalon::NetConnectionPtr conn = ASInternalNetwork::Instance()->FindConnection(GetCenterNodeID(0));
		if(conn != NULL)
		{
			CL_SEND_PROTOCOL(conn, req);
		}
		req.Detach();
	}

	void BroadcastToZones(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			BroadcastToZones(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToZones(Avalon::Packet* packet)
	{
		CLProtocol::CenterBroadcastToZones req(packet);
		Avalon::NetConnectionPtr conn = ASInternalNetwork::Instance()->FindConnection(GetCenterNodeID(0));
		if(conn != NULL)
		{
			CL_SEND_PROTOCOL(conn, req);
		}
		req.Detach();
	}
}
