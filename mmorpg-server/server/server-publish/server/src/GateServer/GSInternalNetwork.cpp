#include "GSInternalNetwork.h"
#include <CLRecordClient.h>

#include "GSApplication.h"
#include "GSRouter.h"
#include "GSSceneMgr.h"

GSInternalConnection::GSInternalConnection(CLNetwork* network)
	:CLInternalConnection(network)
{
}

GSInternalConnection::~GSInternalConnection()
{
}

bool GSInternalConnection::PreHandlePacket(Avalon::Packet *packet)
{
	if(CLInternalConnection::PreHandlePacket(packet)) return true;

	return false;
}



GSInternalNetwork::GSInternalNetwork()
{
}

GSInternalNetwork::~GSInternalNetwork()
{
}

void GSInternalNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch(servtype)
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

Avalon::NetConnection* GSInternalNetwork::CreateConnection()
{
	return new GSInternalConnection(this);
}

void GSInternalNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);
}

void GSInternalNetwork::OnDisconnected(UInt32 id)
{
	Router::OnDisconnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_SCENE || servType == ST_WSCENE || servType == ST_BSCENE)
	{
		GSSceneMgr::Instance()->OnSceneDisconnected(id);
	}
}

void GSInternalNetwork::OnShutdownService()
{
	GSApplication::Instance()->Exit();
}
