#include "RSInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>
#include <CLRecordClient.h>
#include "RSApplication.h"

RSInternalNetwork::RSInternalNetwork()
{
}

RSInternalNetwork::~RSInternalNetwork()
{
}

bool RSInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if (!CLInternalNetwork::Startup(properties, addresses)) return false;

	return true;
}

void RSInternalNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch (servtype)
	{
	case ST_RECORD:
		{
			CLRecordClient::Instance()->OnPacketRecved(packet);
		}
		break;
	default:
		{
			CLInternalNetwork::DispatchPacket(packet);
		}
		break;
	}
}

bool RSInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if (CLInternalNetwork::ExecutePacket(packet)) return true;

	return true;
}

bool RSInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	return false;
}

void RSInternalNetwork::OnConnected(UInt32 id)
{
	/*UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}
	else
	{
		MSRouter::Instance()->OnConnected(id);
	}*/
	//AddVerifiedConnection()
}

void RSInternalNetwork::OnDisconnected(UInt32 id)
{
	/*UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
	else
	{
		MSRouter::Instance()->OnDisconnected(id);
	}*/
}

void RSInternalNetwork::OnShutdownService()
{
	RSApplication::Instance()->Exit();
}
