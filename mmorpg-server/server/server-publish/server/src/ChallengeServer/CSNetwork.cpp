#include "CSNetwork.h"

#include "CSApplication.h"
#include "GameZoneMgr.h"
#include "ChallengeMgr.h"

CSNetwork::CSNetwork()
{
}

CSNetwork::~CSNetwork()
{
}

void CSNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
	else if(servType == ST_WSCENE)
	{
		ChallengeMgr::Instance()->OnSceneServerConnected(id);
	}
}

void CSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
	else if(servType == ST_WSCENE)
	{
		ChallengeMgr::Instance()->OnSceneServerDisconnected(id);
	}
}

void CSNetwork::OnShutdownService()
{
	CSApplication::Instance()->Exit();
}
