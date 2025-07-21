#include "GASInternalNetwork.h"

#include <CLRecordClient.h>
#include "GASApplication.h"
#include "GASGameZoneMgr.h"
#include "ActivityMgr.h"

GASInternalNetwork::GASInternalNetwork()
{
}

GASInternalNetwork::~GASInternalNetwork()
{
}

void GASInternalNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch (servtype)
	{
	case ST_RECORD:
		CLRecordClient::Instance()->OnPacketRecved(packet);
		break;
	default:
		CLInternalNetwork::DispatchPacket(packet);
		break;
	}
}

void GASInternalNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_CENTER)
	{
		GASGameZoneMgr::Instance()->OnConnected(id);
		ActivityMgr::Instance()->OnConnected(id);
	}
}

void GASInternalNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_CENTER)
	{
		GASGameZoneMgr::Instance()->OnDisconnected(id);
	}
}

void GASInternalNetwork::OnShutdownService()
{
	GASApplication::Instance()->Exit();
}
