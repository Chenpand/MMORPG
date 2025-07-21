
#include <CLSysProtocol.h>
#include <CLGameSessionProtocol.h>
#include "RelaySvrRouter.h"
#include "RLInternalNetwork.h"

namespace Router
{
	//使用全局变量保存到服务器的链接，避免每次发包的时候要通过id来查找到相应server的链接
	Avalon::NetConnectionPtr g_WorlSvrdConn = NULL;				//到world server的内部链接
	Avalon::NetConnectionPtr g_MatchSvrConn = NULL;				//到match server的内部链接
	Avalon::NetConnectionPtr g_ReplaySvrConn = NULL;			//到replay server的内部链接
	Avalon::NetConnectionPtr g_BattleSvrConn = NULL;			//到battle server的内部链接
	Avalon::NetConnectionPtr g_DungeonSvrConn = NULL;			//到dungeon server的内部链接
	Avalon::NetConnectionPtr g_ObserveSvrConn = NULL;			//到Observe server的内部链接

	// 先简单处理
	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (ST_MATCHSERVER == servType)
		{
			g_MatchSvrConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else if (ST_WORLD == servType)
		{
			g_WorlSvrdConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else if (ST_REPLAY == servType)
		{
			g_ReplaySvrConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else if (ST_BATTLE == servType)
		{
			g_BattleSvrConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else if (ST_DUNGEON_SVR == servType)
		{
			g_DungeonSvrConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else if (ST_OBSERVE ==  servType)
		{
			g_ObserveSvrConn = RLInternalNetwork::Instance()->FindConnection(id);
		}
		else
		{
			//unknown server type
		}

		return;
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (ST_MATCHSERVER == servType)
		{
			g_MatchSvrConn = NULL;
		}
		else if (ST_WORLD == servType)
		{
			g_WorlSvrdConn = NULL;
		}
		else if (ST_REPLAY == servType)
		{
			g_ReplaySvrConn = NULL;
		}
		else if (ST_BATTLE == servType)
		{
			g_BattleSvrConn = NULL;
		}
		else if (ST_DUNGEON_SVR == servType)
		{
			g_DungeonSvrConn = NULL;
		}
		else if (ST_OBSERVE == servType)
		{
			g_ObserveSvrConn = NULL;
		}
		else
		{
			//unknown server type
		}

		return;

	}

	//send to match server
	void SendToMatchSvr(Avalon::Protocol& protocol)
	{
		//InfoLogStream << "in SendToMatchSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Match Server" << LogStream::eos;
		if (g_MatchSvrConn != NULL)
		{
			InfoLogStream << "in SendToMatchSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Match Server" << LogStream::eos;
			CL_SEND_PROTOCOL(g_MatchSvrConn, protocol);
		}
		else
		{
			SendToBattleSvr(protocol);
			//InfoLogStream << "in SendToMatchSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Match Server, but conn is null" << LogStream::eos;
		}
	}

	void SendToMatchSvr(Avalon::Packet* packet)
	{
		InfoLogStream << "in SendToMatchSvr to send packet to match server " << LogStream::eos;
		if (g_MatchSvrConn != NULL && packet != NULL)
		{
			InfoLogStream << "in SendToMatchSvr, send packet to match server, packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			if (!g_MatchSvrConn->SendPacket(packet))
			{
				ErrorLogStream << "in SendToMatchSvr, send packet to match server failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}
		
		}
		else
		{
			SendToBattleSvr(packet);
			//InfoLogStream << "in SendToMatchSvr to send packet to match server, but conn or packet is null" << LogStream::eos;
		}
	}

	void SendToReplaySvr(Avalon::Packet* packet)
	{
		if (g_ReplaySvrConn != NULL && packet != NULL)
		{
			if (!g_ReplaySvrConn->SendPacket(packet))
			{
				ErrorLogStream << "send packet failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}
		}
	}

	void SendToReplaySvr(Avalon::Protocol& protocol)
	{
		if (g_ReplaySvrConn != NULL)
		{
			CL_SEND_PROTOCOL(g_ReplaySvrConn, protocol);
		}
	}

	//send to world server
	void SendToWorldSvr(UInt32 nodeId, Avalon::Protocol& protocol)
	{
		CLProtocol::SysTransmitRaceProtocolInNode trans;
		trans.nodeId = nodeId;
		if (protocol.Encode(trans.packet))
		{
			SendToMatchSvr(trans);
		}
	}

	void SendToWorldSvr(UInt32 nodeId, Avalon::Packet* packet)
	{
		CLProtocol::SysTransmitRaceProtocolInNode trans;
		trans.nodeId = nodeId;
		trans.packet = packet;
		SendToMatchSvr(trans);
	}

	void SendToBattleSvr(Avalon::Protocol& protocol)
	{
		if (g_BattleSvrConn != NULL)
		{
			InfoLogStream << "in SendToBattleSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Battle Server" << LogStream::eos;
			CL_SEND_PROTOCOL(g_BattleSvrConn, protocol);
		}
		else
		{
			InfoLogStream << "in SendToBattleSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Battle Server, but conn is null" << LogStream::eos;
		}
	}

	void SendToBattleSvr(Avalon::Packet* packet)
	{
		InfoLogStream << "in SendToBattleSvr to send packet to battle server " << LogStream::eos;
		if (g_BattleSvrConn != NULL && packet != NULL)
		{
			InfoLogStream << "in SendToBattleSvr, send packet to battle server, packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			if (!g_BattleSvrConn->SendPacket(packet))
			{
				ErrorLogStream << "in SendToBattleSvr, send packet to battle server failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}

		}
		else
		{
			InfoLogStream << "in SendToBattleSvr to send packet to battle server, but conn or packet is null" << LogStream::eos;
		}
	}

	void SendToDungeonSvr(Avalon::Protocol& protocol)
	{
		if (g_DungeonSvrConn != NULL)
		{
			InfoLogStream << "in SendToDungeonSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Dungeon Server" << LogStream::eos;
			CL_SEND_PROTOCOL(g_DungeonSvrConn, protocol);
		}
		else
		{
			InfoLogStream << "in SendToDungeonSvr to send protocol(id:" << protocol.GetProtocolID() << ")  to Dungeon Server, but conn is null" << LogStream::eos;
		}
	}

	void SendToDungeonSvr(Avalon::Packet* packet)
	{
		InfoLogStream << "in SendToDungeonSvr to send packet to Dungeon server " << LogStream::eos;
		if (g_DungeonSvrConn != NULL && packet != NULL)
		{
			InfoLogStream << "in SendToDungeonSvr, send packet to Dungeon server, packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			if (!g_DungeonSvrConn->SendPacket(packet))
			{
				ErrorLogStream << "in SendToDungeonSvr, send packet to Dungeon server failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}

		}
		else
		{
			InfoLogStream << "in SendToDungeonSvr to send packet to battle server, but conn or packet is null" << LogStream::eos;
		}
	}	

	void SendToObserve(Avalon::Protocol& protocol)
	{
		if (g_ObserveSvrConn != NULL)
		{
			InfoLogStream << "in SendToObserve to send protocol(id:" << protocol.GetProtocolID() << ")  to Observe Server" << LogStream::eos;
			CL_SEND_PROTOCOL(g_ObserveSvrConn, protocol);
		}
		else
		{
			InfoLogStream << "in SendToObserve to send protocol(id:" << protocol.GetProtocolID() << ")  to Observe Server, but conn is null" << LogStream::eos;
		}
	}

	void SendToObserve(Avalon::Packet* packet)
	{
		InfoLogStream << "in SendToObserve to send packet to Observe server " << LogStream::eos;
		if (g_ObserveSvrConn != NULL && packet != NULL)
		{
			InfoLogStream << "in SendToObserve, send packet to Observe server, packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			if (!g_ObserveSvrConn->SendPacket(packet))
			{
				ErrorLogStream << "in SendToObserve, send packet to Observe server failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}

		}
		else
		{
			InfoLogStream << "in SendToObserve to send packet to Observe server, but conn or packet is null" << LogStream::eos;
		}
	}
}// end of namespace Router