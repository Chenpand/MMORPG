#include "WSRouter.h"

#include <CLGateProtocol.h>
#include <CLSysProtocol.h>
#include <CLGameSessionProtocol.h>
#include "WSNetwork.h"

namespace Router
{
	Avalon::NetConnectionPtr g_AdminConn = NULL;

    Avalon::NetConnectionPtr g_MatchConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_ADMIN)
		{
			g_AdminConn = WSNetwork::Instance()->FindConnection(id);
		}
        else if (servType == ST_MATCHSERVER)
        {
            g_MatchConn = WSNetwork::Instance()->FindConnection(id);
        }
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_ADMIN)
		{
			g_AdminConn = NULL;
		}
        else if (servType == ST_MATCHSERVER)
        {
            g_MatchConn = NULL;
        }
	}

	void SendToAdmin(Avalon::Protocol& protocol)
	{
		if(g_AdminConn != NULL)
		{
			CL_SEND_PROTOCOL(g_AdminConn,protocol);
		}
	}

	void SendToAdmin(Avalon::Packet* packet)
	{
		if(g_AdminConn != NULL)
		{
			g_AdminConn->SendPacket(packet);
		}
	}

    void SendToMatch(Avalon::Protocol& protocol)
    {
        if (g_MatchConn != NULL)
        {
            CL_SEND_PROTOCOL(g_MatchConn, protocol);
        }
    }

    void SendToMatch(Avalon::Packet* packet)
    {
        if (g_MatchConn != NULL)
        {
            g_MatchConn->SendPacket(packet);
        }
    }

	void SendToSocial(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToSocial(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToSocial(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetSocialNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
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

	void SendToCross(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToCross(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToCross(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetCrossNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
	}

	void SendToSortList(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToSortList(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToSortList(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetSortListNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
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

	void SendToRelay(UInt32 id, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitRaceProtocolInNode trans;
		trans.nodeId = id;
		if (protocol.Encode(trans.packet))
		{
			SendToMatch(trans);
		}
	}

	void TransmitPlayerProtocolToSortList(ObjID_t playerId, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitPlayerProtocolInNode req;
		req.nodeId = GetSortListNodeID(0);
		req.playerId = playerId;
		if(protocol.Encode(req.packet))
		{
			SendToSortList(req);
		}
	}

	void BroadcastToGate(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			WSNetwork::Instance()->Broadcast(ST_GATE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToScene(Avalon::Protocol& protocol)
	{	
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			WSNetwork::Instance()->Broadcast(ST_SCENE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToPlayer(Avalon::Protocol& protocol, UInt8 bZone)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			Router::BroadcastToPlayer(packet, bZone);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToPlayer(Avalon::Packet* packet, UInt8 bZone)
	{
		if(packet == NULL) return;

		CLProtocol::GateBroadcastAll req(packet);
		req.bZone = bZone;
		BroadcastToGate(req);
		req.Detach();
	}

	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Protocol& protocol, UInt8 bZone)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			Router::BroadcastToPlayerByLvl(level, packet, bZone);
		}
		Avalon::Packet::Destroy(packet);
	}

	void BroadcastToPlayerByLvl(UInt16 level, Avalon::Packet* packet, UInt8 bZone)
	{
		if(packet == NULL) return;

		CLProtocol::GateBroadcastAll req(packet);
		req.level = level;
		req.bZone = bZone;
		BroadcastToGate(req);
		req.Detach();
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
		transmit.destnode = GetBattleNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
	}

	void SendToTeamCopy(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToTeamCopy(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToTeamCopy(Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = GetTeamCopyNodeID(0);
		SendToAdmin(transmit);
		transmit.Detach();
	}

	void SendToTransfer(TransferHeader& tHeader, Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToTransfer(tHeader, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToTransfer(TransferHeader& tHeader, Avalon::Packet* packet)
	{
		CLProtocol::SysTransferProtocol transmit(packet);
		transmit.tHeader = tHeader;
		SendToAdmin(transmit);
		transmit.Detach();
		return;
	}

	void SendToUnion(UnionServerFunctionType type, Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToUnion(type, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToUnion(UnionServerFunctionType type, Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitToUnion transmit(packet);
		transmit.funType = static_cast<UInt32>(type);
		SendToAdmin(transmit);
		transmit.Detach();
		return;
	}

}
