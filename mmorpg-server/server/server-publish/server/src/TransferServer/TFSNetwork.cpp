#include "TFSNetwork.h"

#include <CLSysProtocol.h>

#include "TFSApplication.h"
#include "GameZoneMgr.h"

TFSNetwork::TFSNetwork()
{

}

TFSNetwork::~TFSNetwork()
{

}

void TFSNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch (servtype)
	{
	default:
		CLInternalNetwork::DispatchPacket(packet);
		break;
	}
}

void TFSNetwork::OnConnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
}

void TFSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
}

void TFSNetwork::OnShutdownService()
{
	TFSApplication::Instance()->Exit();
}
