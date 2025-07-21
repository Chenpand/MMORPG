#include "WSServiceSwitchMgr.h"
#include <CLSysProtocol.h>
#include "WSRouter.h"
#include "WSNetwork.h"

WSServiceSwitchMgr::WSServiceSwitchMgr()
{
	m_autoBattleMode = 0;
}

void WSServiceSwitchMgr::OnOpenService(ServiceType type)
{
	CLProtocol::SysServiceSwitchChangeStatus protocol;
	protocol.type = (UInt8)type;
	protocol.open = 1;
	Router::BroadcastToScene(protocol);
	Router::BroadcastToGate(protocol);
}

void WSServiceSwitchMgr::OnCloseService(ServiceType type)
{
	CLProtocol::SysServiceSwitchChangeStatus protocol;
	protocol.type = (UInt8)type;
	protocol.open = 0;
	Router::BroadcastToScene(protocol);
	Router::BroadcastToGate(protocol);
}

void WSServiceSwitchMgr::OnSceneConnected(UInt32 id)
{
	auto conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		return;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	for (UInt32 i = 0; i < sizeof(m_serviceConfigs) / sizeof(m_serviceConfigs[0]); i++)
	{
		auto& config = m_serviceConfigs[i];
		if (config.open)
		{
			continue;
		}

		CLProtocol::SysServiceSwitchChangeStatus protocol;
		protocol.type = (UInt8)i;
		protocol.open = 0;

		if (protocol.Encode(packet))
		{
			conn->SendPacket(packet);
		}
	}

	Avalon::Packet::Destroy(packet);
}

void WSServiceSwitchMgr::OnGateConnected(UInt32 id)
{
	auto conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		return;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	for (UInt32 i = 0; i < sizeof(m_serviceConfigs) / sizeof(m_serviceConfigs[0]); i++)
	{
		auto& config = m_serviceConfigs[i];
		if (config.open)
		{
			continue;
		}

		CLProtocol::SysServiceSwitchChangeStatus protocol;
		protocol.type = (UInt8)i;
		protocol.open = 0;

		if (protocol.Encode(packet))
		{
			conn->SendPacket(packet);
		}
	}

	Avalon::Packet::Destroy(packet);
}