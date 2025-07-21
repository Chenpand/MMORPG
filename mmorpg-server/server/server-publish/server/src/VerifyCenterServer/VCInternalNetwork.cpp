#include "VCInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>
#include <CLRecordClient.h>
#include "VCApplication.h"
#include "VerifyServerMgr.h"
#include "VerifyClientMgr.h"

VCInternalNetwork::VCInternalNetwork()
{
}

VCInternalNetwork::~VCInternalNetwork()
{
}

bool VCInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if (!CLInternalNetwork::Startup(properties, addresses)) return false;

	return true;
}

void VCInternalNetwork::DispatchPacket(Avalon::Packet* packet)
{
	CLInternalNetwork::DispatchPacket(packet);
}

bool VCInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if (CLInternalNetwork::ExecutePacket(packet)) return true;

	return true;
}

bool VCInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	return false;
}

void VCInternalNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_WORLD || servType == ST_RELAY)
	{
		VerifyClient* client = new VerifyClient();
		client->Init(id, (ServerType)servType);
		VerifyClientMgr::Instance()->AddClient(client);
	}
}

void VCInternalNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_VERIFY)
	{
		VerifyServerMgr::Instance()->DelNode(id);
	}
	else if (servType == ST_WORLD || servType == ST_RELAY)
	{
		VerifyClientMgr::Instance()->DelClient(id);
	}
}

void VCInternalNetwork::OnShutdownService()
{
	VCApplication::Instance()->Exit();
}
