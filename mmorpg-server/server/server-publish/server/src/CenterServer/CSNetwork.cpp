#include "CSNetwork.h"

#include "CSApplication.h"
#include "GameZoneMgr.h"

CSNetwork::CSNetwork()
{
}

CSNetwork::~CSNetwork()
{
}

void CSNetwork::OnConnected(UInt32 id)
{
	GameZoneMgr::Instance()->OnConnected(id);
}

void CSNetwork::OnDisconnected(UInt32 id)
{
	GameZoneMgr::Instance()->OnDisconnected(id);
}

void CSNetwork::OnShutdownService()
{
	CSApplication::Instance()->Exit();
}
