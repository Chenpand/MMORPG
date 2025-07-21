#pragma once

#include <CLDefine.h>
#include <AvalonProtocol.h>

namespace Router
{
	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	 *@brief ���͵�DungeonServer
	 */
	void SendToDungeon(Avalon::Protocol& protocol);
	void SendToDungeon(Avalon::Packet* packet);
}