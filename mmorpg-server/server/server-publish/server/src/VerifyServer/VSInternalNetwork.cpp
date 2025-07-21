#include "VSInternalNetwork.h"

#include <AvalonThread.h>
#include <CLProtocolDefine.h>
#include <CLRecordClient.h>
#include <CLVerifyProtocol.h>
#include "VSApplication.h"
#include "VSRouter.h"

VSInternalNetwork::VSInternalNetwork()
{
}

VSInternalNetwork::~VSInternalNetwork()
{
}

bool VSInternalNetwork::Startup(Avalon::PropertySet *properties, CLNetAddress *addresses)
{
	if (!CLInternalNetwork::Startup(properties, addresses)) return false;

	return true;
}

void VSInternalNetwork::DispatchPacket(Avalon::Packet* packet)
{
	CLInternalNetwork::DispatchPacket(packet);
}

bool VSInternalNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if (CLInternalNetwork::ExecutePacket(packet)) return true;

	return true;
}

bool VSInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;

	return false;
}

void VSInternalNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_VERIFY_CENTER)
	{
		// 去中心服务器注册
		CLProtocol::VerifyCenterRegisterReq req;
		std::map<UInt32, Avalon::SockAddr> addrs;
		VSApplication::Instance()->GetNetAddress()->GetInternalAddresses(ST_VERIFY, addrs);
		req.addr = addrs[NODE_ID];
		req.ability = VSApplication::Instance()->GetAbility();
		Router::SendToVerifyCenter(req);
	}
}

void VSInternalNetwork::OnDisconnected(UInt32 id)
{
	Router::OnDisconnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_VERIFY_CENTER)
	{

	}
}

void VSInternalNetwork::OnShutdownService()
{
	VSApplication::Instance()->Exit();
}
