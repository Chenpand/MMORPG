#include "USNetwork.h"

#include <CLRecordClient.h>
#include <CLSysProtocol.h>
#include <CLUnionServerProtocol.h>

#include "USApplication.h"
#include "GameZoneMgr.h"
#include "USRouter.h"

USNetwork::USNetwork()
{

}

USNetwork::~USNetwork()
{

}

void USNetwork::DispatchPacket(Avalon::Packet* packet)
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

void USNetwork::OnConnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
	auto zone = GameZoneMgr::Instance()->FindGameZone(id);
	if (zone != nullptr)
	{
		CLProtocol::UnionNotifyOpenFunction notify;
		notify.openFun.insert(notify.openFun.begin(), USApplication::Instance()->GeOpenFunction().begin(), USApplication::Instance()->GeOpenFunction().end());
		zone->SendProtocol(notify);
	}
	USRouter::Instance()->OnConnected(id);
}

void USNetwork::OnDisconnected(UInt32 id)
{
	UInt32 serverType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (serverType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
	}
	USRouter::Instance()->OnDisconnected(id);
}

void USNetwork::OnShutdownService()
{
	USApplication::Instance()->Exit();
}
