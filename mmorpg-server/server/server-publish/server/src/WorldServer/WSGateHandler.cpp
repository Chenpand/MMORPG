#include "WSGateHandler.h"
#include "WSPlayerMgr.h"


void HandleProtocol(CLProtocol::GateTransmitPacket& protocol)
{
	WSPlayerMgr::Instance()->ExecutePacket(protocol.id, protocol.packet);
}

void HandleProtocol(CLProtocol::GateNotifyDisconnect& protocol)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.id);
	if (!player)
	{
		return;
	}

	player->OnDisconnect();
}

void HandleProtocol(CLProtocol::GateNotifyReconnectSuccess& protocol)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.id);
	if (!player)
	{
		return;
	}

	player->OnReconnectSuccess();
}