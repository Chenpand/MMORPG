#include "GSRouter.h"

#include <CLSysProtocol.h>
#include <CLCenterProtocol.h>
#include <CLGameSessionProtocol.h>
#include "GSInternalNetwork.h"

namespace Router
{
	Avalon::NetConnectionPtr g_AdminConn = NULL;

	Avalon::NetConnectionPtr g_WorldConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_ADMIN)
		{
			g_AdminConn = GSInternalNetwork::Instance()->FindConnection(id);
		}
		else if(servType == ST_WORLD)
		{
			g_WorldConn = GSInternalNetwork::Instance()->FindConnection(id);
		}
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_ADMIN)
		{
			g_AdminConn = NULL;
		}
		else if(servType == ST_WORLD)
		{
			g_WorldConn = NULL;
		}
	}

	void SendToAdmin(Avalon::Protocol& protocol)
	{
		if(g_AdminConn != NULL)
		{
			CL_SEND_PROTOCOL(g_AdminConn,protocol);
		}
	}

	void SendToWorld(Avalon::Protocol& protocol)
	{
		if(g_WorldConn != NULL)
		{
			CL_SEND_PROTOCOL(g_WorldConn,protocol);
		}
	}

	void SendToMatch(Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitRaceProtocolInNode trans;
		trans.nodeId = CL_MAKE_NODEID(ZONE_ID, ST_MATCHSERVER, 0);
		if (protocol.Encode(trans.packet))
		{
			SendToWorld(trans);
		}
	}

	void SendToCenter(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToCenter(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToCenter(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetCenterNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
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
		SendToCenter(req);
		req.Detach();
	}

	void SendToGlobalActivity(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToGlobalActivity(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToGlobalActivity(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetGlobalActivityNodeID(0);
		SendToCenter(transmit);
		transmit.Detach();
	}

	void SendToBattle(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToBattle(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToBattle(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = CL_MAKE_NODEID(0, ST_BATTLE, 0);

		Router::SendToAdmin(transmit);
		transmit.Detach();
	}
}
