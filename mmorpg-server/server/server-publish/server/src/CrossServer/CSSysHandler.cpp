#include "CSSysHandler.h"
#include "CSNetwork.h"
#include "GameZoneMgr.h"

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	// 除了要发到wscene，那么需要转发到adminserver 
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.destnode);
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.destnode);
	if (serverType == ST_WSCENE)
	{
		Avalon::NetConnectionPtr node = CSNetwork::Instance()->FindConnection(protocol.destnode);
		if (node == NULL)
		{
			return;
		}

		node->SendPacket(protocol.packet);
		return;
	}

	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol)
{
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.nodeId);
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.nodeId);
	if (serverType == ST_WSCENE)
	{
		Avalon::NetConnectionPtr node = CSNetwork::Instance()->FindConnection(protocol.nodeId);
		if (node == NULL)
		{
			return;
		}

		node->SendPacket(protocol.packet);
		return;
	}

	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		return;
	}

	zone->SendProtocol(protocol);
}
