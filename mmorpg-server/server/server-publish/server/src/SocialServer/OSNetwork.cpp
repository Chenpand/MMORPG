#include "OSNetwork.h"

#include <CLSysProtocol.h>
#include "OSApplication.h"

OSNetwork::OSNetwork()
{
}

OSNetwork::~OSNetwork()
{
}

void OSNetwork::TransmitProtocol(UInt32 connId, UInt32 nodeId, Avalon::Protocol& protocol)
{
	Avalon::NetConnectionPtr conn = FindConnection(connId);
	if( conn != NULL )
	{
		CLProtocol::SysTransmitInNodes transmit;
		if(!protocol.Encode(transmit.packet)) return;
		transmit.destnode = nodeId;
		CL_SEND_PROTOCOL(conn, transmit);
	}
}

void OSNetwork::OnConnected(UInt32 id)
{
}

void OSNetwork::OnDisconnected(UInt32 id)
{
}

void OSNetwork::OnShutdownService()
{
	OSApplication::Instance()->Exit();
}
