#include "BSRouter.h"
#include "BSNetwork.h"

#include <AvalonNetConnection.h>


namespace Router
{
	Avalon::NetConnectionPtr g_DungeonConn = NULL;

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (servType == ST_DUNGEON_SVR)
		{
			g_DungeonConn = BSNetwork::Instance()->FindConnection(id);
		}
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (servType == ST_DUNGEON_SVR)
		{
			g_DungeonConn = NULL;
		}
	}

	void SendToDungeon(Avalon::Protocol& protocol)
	{
		if (g_DungeonConn != NULL)
		{
			CL_SEND_PROTOCOL(g_DungeonConn, protocol);
		}
	}

	void SendToDungeon(Avalon::Packet* packet)
	{
		if (g_DungeonConn != NULL)
		{
			g_DungeonConn->SendPacket(packet);
		}
	}
}