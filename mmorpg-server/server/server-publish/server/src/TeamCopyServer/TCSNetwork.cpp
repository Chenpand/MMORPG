#include "TCSNetwork.h"

#include <CLRecordClient.h>
#include <CLSysProtocol.h>

#include "TCSApplication.h"
#include "GameZoneMgr.h"
#include "TCSDungeonMgr.h"
#include "TCSTeamMgr.h"

TCSNetwork::TCSNetwork()
{

}

TCSNetwork::~TCSNetwork()
{

}

void TCSNetwork::DispatchPacket(Avalon::Packet* packet)
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

void TCSNetwork::OnConnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
		TCSTeamMgr::Instance()->ValCfgSyncToScene(id);
	}
	else if (serverType == ST_DUNGEON_SVR)
	{
		TCSDungeonMgr::Instance()->OnConnected(id);
	}
}

void TCSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
	else if (serverType == ST_DUNGEON_SVR)
	{
		TCSDungeonMgr::Instance()->OnDisconnected(id);
	}
}

void TCSNetwork::OnShutdownService()
{
	TCSApplication::Instance()->Exit();
}
