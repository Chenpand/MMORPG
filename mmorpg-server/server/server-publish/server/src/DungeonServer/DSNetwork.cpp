#include "DSNetwork.h"

#include "DSApplication.h"
#include "RelaySessionMgr.h"
#include "ServiceSessionMgr.h"
#include "SceneSessionMgr.h"

DSNetwork::DSNetwork()
{
}

DSNetwork::~DSNetwork()
{
}

void DSNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);

	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}
	else if (servType == ST_TEAMCOPY || servType == ST_BATTLE)
	{
		ServiceSessionMgr::Instance()->OnConnected(id);
	}
	else if (servType == ST_SCENE)
	{
		SceneSessionMgr::Instance()->OnConnected(id);
	}
}

void DSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
	else if (servType == ST_TEAMCOPY)
	{
		ServiceSessionMgr::Instance()->OnDisconnected(id);
	}
	else if (servType == ST_SCENE)
	{
		SceneSessionMgr::Instance()->OnDisconnected(id);
	}
}

void DSNetwork::OnShutdownService()
{
	DSApplication::Instance()->Exit();
}
