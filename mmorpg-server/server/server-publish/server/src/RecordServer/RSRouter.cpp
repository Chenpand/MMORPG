#include "RSRouter.h"

#include <CLSysProtocol.h>
#include <CLRecordProtocol.h>
#include "RSNetwork.h"
#include "RSApplication.h"

namespace Router
{
	Avalon::NetConnectionPtr g_DBProxyConn = NULL;
	Avalon::NetConnectionPtr g_RecordMasterConn = NULL;
	Avalon::NetConnectionPtr g_RecordSlaverConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_DBPROXY)
		{
			g_DBProxyConn = RSNetwork::Instance()->FindConnection(id);
		}
		else if (servType == ST_RECORD)
		{
			g_RecordMasterConn = RSNetwork::Instance()->FindConnection(id);
		}
		else if (servType == ST_RECORD_SLAVER)
		{
			g_RecordSlaverConn = RSNetwork::Instance()->FindConnection(id);
		}
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if(servType == ST_DBPROXY)
		{
			g_DBProxyConn = NULL;
		}
		else if (servType == ST_RECORD)
		{
			g_RecordMasterConn = NULL;
		}
		else if (servType == ST_RECORD_SLAVER)
		{
			g_RecordSlaverConn = NULL;
		}
	}

	bool HasSlaverRecord()
	{
		if (!RSApplication::Instance()->OpenTransRequest())
		{
			return false;
		}

		return g_RecordSlaverConn != NULL;
	}

	void SendToDBProxy(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			SendToDBProxy(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToDBProxy(Avalon::Packet* packet)
	{
		if(g_DBProxyConn != NULL)
		{
			g_DBProxyConn->SendPacket(packet);
		}
	}

	void TransToRecord(UInt32 zoneId, Avalon::Protocol& protocol)
	{
		ErrorLogStream << "trans to " << zoneId << LogStream::eos;

		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			TransToRecord(zoneId, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void TransToRecord(UInt32 zoneId, Avalon::Packet* packet)
	{
		ErrorLogStream << "trans to " << zoneId << LogStream::eos;
		CLProtocol::SysTransmitInNodes protocol(packet);
		protocol.destnode = GetRecordNodeID(zoneId);
		SendToDBProxy(protocol);
		protocol.Detach();
	}

	void TransRequest(UInt32 destZoneId, UInt32 nodeId, Avalon::Packet* packet)
	{
		ErrorLogStream << "trans to " << destZoneId << LogStream::eos;
		CLProtocol::RecordTransmitRequest req(packet);
		req.zoneId = ZONE_ID;
		req.nodeId = nodeId;
		TransToRecord(destZoneId, req);
		req.Detach();
	}

	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Protocol& protocol)
	{
		ErrorLogStream << "trans to " << destZoneId << LogStream::eos;
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			TransResponse(destZoneId, destNodeId, packet);
		}
		Avalon::Packet::Destroy(packet);	
	}

	void TransResponse(UInt32 destZoneId, UInt32 destNodeId, Avalon::Packet* packet)
	{
		ErrorLogStream << "trans to " << destZoneId << LogStream::eos;

		CLProtocol::RecordTransmitResponse req(packet);
		req.nodeId = destNodeId;
		TransToRecord(destZoneId, req);
		req.Detach();
	}

	void BroadcastToScene(Avalon::Protocol& protocol)
	{	
		Avalon::Packet* packet = Avalon::Packet::Create();
		if(protocol.Encode(packet))
		{
			RSNetwork::Instance()->Broadcast(ST_SCENE,packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToMasterRecord(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToMasterRecord(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToMasterRecord(Avalon::Packet* packet)
	{
		if (g_RecordMasterConn != NULL)
		{
			g_RecordMasterConn->SendPacket(packet);
		}
	}

	void SendToSlaverRecord(Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			SendToSlaverRecord(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void SendToSlaverRecord(Avalon::Packet* packet)
	{
		if (g_RecordSlaverConn != NULL)
		{
			g_RecordSlaverConn->SendPacket(packet);
		}
	}

	void TransRequestToSlaver(UInt32 nodeId, Avalon::Packet* packet)
	{
		CLProtocol::RecordTransmitRequest req(packet);
		req.zoneId = ZONE_ID;
		req.nodeId = nodeId;
		SendToSlaverRecord(req);
		req.Detach();
	}

	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Protocol& protocol)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		if (protocol.Encode(packet))
		{
			TransResponseFromSlaver(destNodeId, packet);
		}
		Avalon::Packet::Destroy(packet);
	}

	void TransResponseFromSlaver(UInt32 destNodeId, Avalon::Packet* packet)
	{
		CLProtocol::RecordTransmitResponse req(packet);
		req.nodeId = destNodeId;
		SendToMasterRecord(req);
		req.Detach();
	}
}
