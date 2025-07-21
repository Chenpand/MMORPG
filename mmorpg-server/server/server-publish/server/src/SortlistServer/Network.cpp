#include "Network.h"

#include "Application.h"
#include "GameZoneMgr.h"

Network::Network()
{
}

Network::~Network()
{
}

void Network::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
}

void Network::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
}

void Network::OnShutdownService()
{
	Application::Instance()->Exit();
}

bool Network::ExecutePlayerPacket(Avalon::Packet* packet, ObjID_t playerId) const
{
	return m_Executor.ExecutePacket(packet, playerId);
}
