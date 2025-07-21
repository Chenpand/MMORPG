#include "SSRouter.h"

#include <CLSysProtocol.h>
#include <CLGateProtocol.h>
#include <CLCenterProtocol.h>
#include <CLGameSessionProtocol.h>
#include "SSNetwork.h"
#include "PlayerMgr.h"

namespace Router
{
	Avalon::NetConnectionPtr g_AdminConn = NULL;

	Avalon::NetConnectionPtr g_WorldConn = NULL;

	Avalon::NetConnectionPtr g_CrossConn = NULL;

	Avalon::NetConnectionPtr g_ReplayConn = NULL;

	Avalon::NetConnectionPtr g_DungeonConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_ADMIN)
		{
			g_AdminConn = SSNetwork::Instance()->FindConnection(id);
		}
		else if(servType == ST_WORLD)
		{
			g_WorldConn = SSNetwork::Instance()->FindConnection(id);
		}
		else if(servType == ST_CROSS)
		{
			g_CrossConn = SSNetwork::Instance()->FindConnection(id);
		}
		else if (servType == ST_REPLAY)
		{
			g_ReplayConn = SSNetwork::Instance()->FindConnection(id);
		}
		else if (servType == ST_DUNGEON_SVR)
		{
			g_DungeonConn = SSNetwork::Instance()->FindConnection(id);
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
		else if(servType == ST_CROSS)
		{
			g_CrossConn = NULL;
		}
		else if (servType == ST_REPLAY)
		{
			g_ReplayConn = NULL;
		}
		else if (servType == ST_DUNGEON_SVR)
		{
			g_DungeonConn = NULL;
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

	void SendToWorld(Avalon::Protocol& protocol)
	{
		if(g_WorldConn != NULL)
		{
			CL_SEND_PROTOCOL(g_WorldConn,protocol);
		}
	}
	void SendToWorld(Avalon::Packet* packet)
	{
		if(g_WorldConn != NULL)
		{
			g_WorldConn->SendPacket(packet);
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
		SendToCenter(req);
		req.Detach();
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
		if(SERVER_TYPE != ST_WSCENE)
		{
			CLProtocol::SysTransmitInNodes transmit(packet);
			transmit.destnode = GetCrossNodeID(0);
			SendToAdmin(transmit);
			transmit.Detach();
		}
		else
		{
			if (g_CrossConn != NULL) g_CrossConn->SendPacket(packet);
		}
	}

	void SendToReplay(Avalon::Protocol& protocol)
	{
		if (g_ReplayConn != NULL)
		{
			CL_SEND_PROTOCOL(g_ReplayConn, protocol);
		}
	}

	void SendToReplay(Avalon::Packet* packet)
	{
		if (g_ReplayConn != NULL)
		{
			g_ReplayConn->SendPacket(packet);
		}
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

	void BroadcastToGate(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SSNetwork::Instance()->Broadcast(ST_GATE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToScene(UInt32 nodeid, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitInNodes transmit;
		transmit.destnode = nodeid;
		if(protocol.Encode(transmit.packet))
		{		
			SendToWorld(transmit);
		}
	}

	void SendToScene(UInt32 nodeid, Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitInNodes transmit(packet);
		transmit.destnode = nodeid;
		SendToWorld(transmit);
		transmit.Detach();
	}

	void BroadcastToScene(Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitInNodesType transmit;
		transmit.destnodetype = ST_SCENE;
		if(protocol.Encode(transmit.packet))
		{
			SendToWorld(transmit);
		}
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

	void TransmitToPlayer(ObjID_t id, Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			Router::TransmitToPlayer(id, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void TransmitToPlayer(ObjID_t id, Avalon::Packet* packet)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(id);
		if(player != NULL)
		{
			player->SendPacket(packet);
		}
		else
		{
			CLProtocol::SysTransmitToPlayer req(packet);
			req.playerId = id;
			SendToWorld(req);
			req.Detach();
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

    void SendToRelay(UInt32 nodeId, Avalon::Protocol& protocol)
    {
        CLProtocol::SysTransmitRaceProtocolInNode trans;
        trans.nodeId = nodeId;
        if (protocol.Encode(trans.packet))
        {
            SendToWorld(trans);
        }
    }

	void SendToWScene(UInt32 nodeId, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitInNodes trans;
		trans.destnode = nodeId;
		if (protocol.Encode(trans.packet))
		{
			SendToAdmin(trans);
		}
	}

	void SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitInNodes trans;
		trans.destnode = nodeId;
		if (protocol.Encode(trans.packet))
		{
			SendToBattle(trans);
		}
	}

	void SendToSelfScene(UInt32 nodeId, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitInNodes trans;
		trans.destnode = nodeId;
		if (protocol.Encode(trans.packet))
		{
			SendToCross(trans);
		}
	}

	void SendToSelfScenePlayer(UInt32 nodeId, ObjID_t id, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitPlayerProtocolInNode trans;
		trans.nodeId = nodeId;
		trans.playerId = id;
		if (protocol.Encode(trans.packet))
		{
			SendToCross(trans);
		}
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
		return;
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
		return;
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

	UInt32 GetWorldNodeID()
	{
		if (g_WorldConn != NULL)
		{
			return g_WorldConn->GetID();
		}

		return 0;
	}

	void SendToDungeon(Avalon::Protocol& protocol)
	{
		if (g_DungeonConn != NULL)
		{
			CL_SEND_PROTOCOL(g_DungeonConn, protocol);
		}
	}

	void SendToDungeon(Avalon::Packet* packet)
	{
		if (g_DungeonConn != NULL)
		{
			g_DungeonConn->SendPacket(packet);
		}
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
}
