#include "DPNetwork.h"

#include "DPApplication.h"
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
	if(servType == ST_RECORD)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
}

void Network::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_RECORD)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
}

void Network::OnShutdownService()
{
	Application::Instance()->Exit();
}
