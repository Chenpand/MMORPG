#include "MSInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>

#include "MSApplication.h"
#include "RelaySessionMgr.h"
#include "MSRouter.h"
#include "MatchServiceMgr.h"

MSInternalNetwork::MSInternalNetwork()
{
}

MSInternalNetwork::~MSInternalNetwork()
{
}

bool MSInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if (!CLInternalNetwork::Startup(properties, addresses)) return false;

	return true;
}

bool MSInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if (CLInternalNetwork::ExecutePacket(packet)) return true;

	return true;
}

bool MSInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	return false;
}

void MSInternalNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}
	else if (servType == ST_WORLD)
	{
		MSRouter::Instance()->OnConnected(id);
		MatchServiceMgr::Instance()->OnWorldServerConnected(id);
	}
	else if (servType == ST_UNION)
	{
		MSRouter::Instance()->OnConnected(id);
	}
}

void MSInternalNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
	else
	{
		MSRouter::Instance()->OnDisconnected(id);
	}
}

void MSInternalNetwork::OnShutdownService()
{
	MSApplication::Instance()->Exit();
}
