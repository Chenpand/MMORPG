#include "OBSInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>

#include "OBSApplication.h"
#include "RelaySessionMgr.h"
#include "OBSRouter.h"
#include "ObserveRelaySessionMgr.h"


OBSInternalNetwork::OBSInternalNetwork()
{
}

OBSInternalNetwork::~OBSInternalNetwork()
{
}

bool OBSInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if (!CLInternalNetwork::Startup(properties, addresses)) return false;

	return true;
}

bool OBSInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if (CLInternalNetwork::ExecutePacket(packet)) return true;

	return true;
}

bool OBSInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	return false;
}

void OBSInternalNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}
	else if (servType == ST_OBSERVE_RELAY)
	{
		ObserveRelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}
	else if (servType == ST_WORLD)
	{
		OBSRouter::Instance()->OnConnected(id);
	}
	else if (servType == ST_UNION)
	{
		OBSRouter::Instance()->OnConnected(id);
	}
}

void OBSInternalNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
	else if (servType == ST_OBSERVE_RELAY)
	{
		ObserveRelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
	else
	{
		OBSRouter::Instance()->OnDisconnected(id);
	}
}

void OBSInternalNetwork::OnShutdownService()
{
	OBSApplication::Instance()->Exit();
}
