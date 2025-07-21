#include "ASSysHandler.h"

#include "ASInternalNetwork.h"

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	// 如果是要发到wscene，那么需要转发到challengeserver 
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.destnode);
	if (serverType == ST_WSCENE)
	{
		Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnectionByType(ST_CROSS);
		if (node != NULL)
		{
			CL_SEND_PROTOCOL(node, protocol);
		}
		return;
	}
	else if (serverType == ST_CENTER)
	{
		Avalon::NetConnectionPtr centerNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_CENTER);
		if (centerNode == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_CENTER << ") when transmit player packet." << LogStream::eos;
			return;
		}

		centerNode->SendPacket(protocol.packet);
		return;
	}
	else if (serverType == ST_WORLD)
	{
		Avalon::NetConnectionPtr worldNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_WORLD);
		if (worldNode == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_WORLD << ") when transmit player packet." << LogStream::eos;
			return;
		}

		worldNode->SendPacket(protocol.packet);
		return;
	}
	else if (serverType == ST_BATTLE || serverType == ST_BSCENE)
	{
		Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnectionByType(ST_BATTLE);
		if (node == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_BATTLE << ") when transmit player packet." << LogStream::eos;
			return;
		}

		node->SendPacket(protocol.packet);
		return;
	}
	else if (serverType == ST_TEAMCOPY)
	{
		Avalon::NetConnectionPtr teamCopyNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_TEAMCOPY);
		if (teamCopyNode == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_TEAMCOPY << ") when transmit player packet." << LogStream::eos;
			return;
		}

		teamCopyNode->SendPacket(protocol.packet);
		return;
	}

	Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnection(protocol.destnode);
	if(node == NULL)
	{
#ifdef _DEBUG
		ErrorLogStream << "can not find connection(" << protocol.destnode << ") when transmit packet." << LogStream::eos;
#endif
		return;
	}

	node->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol)
{
	if (protocol.destnodetype == (UInt8)ST_SCENE)
	{
		ASInternalNetwork::Instance()->Broadcast(ST_SCENE, protocol.packet);
		return;
	}
	else if (protocol.destnodetype == (UInt8)ST_WORLD)
	{
		Avalon::NetConnectionPtr worldNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_WORLD);
		if (worldNode == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_WORLD << ") when transmit player packet." << LogStream::eos;
			return;
		}

		worldNode->SendPacket(protocol.packet);
	}
}

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol)
{
	// 如果是要发到wscene，那么需要转发到challengeserver 
	auto serverType = CL_GET_SERVERTYPE_FROM_NODEID(protocol.nodeId);
	if (serverType == ST_WSCENE)
	{
		Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnectionByType(ST_CROSS);
		if (node != NULL)
		{
			node->SendPacket(protocol.packet);
		}
		return;
	}

	if (serverType == ST_BSCENE)
	{
		Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnectionByType(ST_BATTLE);
		if (node != NULL)
		{
			node->SendPacket(protocol.packet);
		}
		return;
	}

	Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindConnection(protocol.nodeId);
	if(node == NULL)
	{
		ErrorLogStream << "can not find connection(" << protocol.nodeId << ") when transmit player packet." << LogStream::eos;
		return;
	}

	CL_SEND_PROTOCOL(node, protocol);
}

void HandleProtocol(CLProtocol::SysTransmitFromGlobalToNode& protocol)
{
	DebugLogStream << "Transmit protocol(" << protocol.GetProtocolID() << ") from global to dest server(" << protocol.destServer << ")..." << LogStream::eos;

	switch ((ServerType)protocol.destServer)
	{
	case ST_GATE:
	{
		class VisitGateConnection : public Avalon::NetConnectionVisitor
		{
		public:
			VisitGateConnection(Avalon::Packet* packet) : m_Packet(packet), m_HasSendFlag(false){}

			bool operator()(Avalon::NetConnection* conn)
			{
				if (CL_GET_SERVERTYPE_FROM_NODEID(conn->GetID()) == ST_GATE)
				{
					conn->SendPacket(m_Packet);
					m_HasSendFlag = true;
				}
				return true;
			}

			bool HasSend() const { return m_HasSendFlag; }

		private:
			Avalon::Packet* m_Packet;
			bool m_HasSendFlag;
		};

		VisitGateConnection visitor(protocol.packet);
		ASInternalNetwork::Instance()->VisitConnections(visitor);

		if (!visitor.HasSend())
		{
#ifdef _DEBUG
			ErrorLogStream << "can not find connection(" << protocol.destServer << ") when transmit packet." << LogStream::eos;
#endif
		}

		break;
	}

	case ST_WORLD:
	{
		Avalon::NetConnectionPtr worldNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_WORLD);
		if (worldNode == NULL)
		{
			ErrorLogStream << "can not find connection(" << (UInt32)ST_WORLD << ") when transmit player packet." << LogStream::eos;
			return;
		}

		worldNode->SendPacket(protocol.packet);

		break;
	}

	default:
		break;
	}
}

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol)
{
	protocol.playerInfo.sourceNode[ST_ADMIN] = NODE_ID;

	Avalon::NetConnectionPtr worldNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_WORLD);
	if (worldNode == NULL)
	{
		ErrorLogStream << "can not find connection(" << (UInt32)ST_WORLD << ") when transmit player packet." << LogStream::eos;
		return;
	}

	protocol.playerInfo.sourceNode[ST_WORLD] = worldNode->GetID();

	Avalon::NetConnectionPtr centerNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_CENTER);
	if (centerNode == NULL)
	{
		ErrorLogStream << "can not find connection(" << (UInt32)ST_CENTER << ") when transmit player packet." << LogStream::eos;
		return;
	}

	CL_SEND_PROTOCOL(centerNode, protocol);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerToTeamCopy& protocol)
{
	Avalon::NetConnectionPtr teamCopyNode = ASInternalNetwork::Instance()->FindConnectionByType(ST_TEAMCOPY);
	if (teamCopyNode == NULL)
	{
		ErrorLogStream << "can not find connection(" << (UInt32)ST_TEAMCOPY << ") when transmit player packet." << LogStream::eos;
		return;
	}

	CL_SEND_PROTOCOL(teamCopyNode, protocol);
}
void HandleProtocol(CLProtocol::SysTransmitPlayerToUnion& protocol)
{
	auto funType = static_cast<UnionServerFunctionType>(protocol.funType);
	if (funType == USFT_NONE)
	{
		//funType = static_cast<UnionServerFunctionType>(CL_GET_FUNCTIONTYPE_FROM_PROTOCOLID(protocol.packet->GetID()));
	}
	else if (funType == USFT_ALL)
	{
		std::vector<Avalon::NetConnectionPtr> connVec;
		ASInternalNetwork::Instance()->FindConnectionByType(ST_UNION, connVec);
		for (auto &iter : connVec)
		{
			CL_SEND_PROTOCOL(iter, protocol);
		}
		return;
	}

	Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindUnionConnectionByFun(funType);
	if (node == NULL)
	{
		ErrorLogStream << "can not find connection(servertype:" << static_cast<UInt32>(ST_UNION) << " funType:" << static_cast<UInt32>(funType) << ") when transmit player packet." << LogStream::eos;
		return;
	}

	CL_SEND_PROTOCOL(node, protocol);
}


void HandleProtocol(CLProtocol::UnionNotifyOpenFunction& protocol)
{
	ASInternalNetwork::Instance()->OnUnionServerNotifyOpenFunction(protocol.GetConnID(), protocol.openFun);
}


void HandleProtocol(CLProtocol::SysTransmitToUnion& protocol)
{
	auto funType = static_cast<UnionServerFunctionType>(protocol.funType);
	if (funType == USFT_NONE)
	{
		funType =  static_cast<UnionServerFunctionType>(CL_GET_FUNCTIONTYPE_FROM_PROTOCOLID(protocol.packet->GetID()));
	}
	else if (funType == USFT_ALL)
	{
		std::vector<Avalon::NetConnectionPtr> connVec;
		ASInternalNetwork::Instance()->FindConnectionByType(ST_UNION, connVec);
		for (auto &iter : connVec)
		{
			iter->SendPacket(protocol.packet);
		}
		return;
	}

	Avalon::NetConnectionPtr node = ASInternalNetwork::Instance()->FindUnionConnectionByFun(funType);
	if (node == NULL)
	{
		ErrorLogStream << "can not find connection(servertype:" << static_cast<UInt32>(ST_UNION) << " funType:" << static_cast<UInt32>(funType) << ") when transmit packet." << LogStream::eos;
		return;
	}

	node->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransferProtocol& protocol)
{
	std::vector<Avalon::NetConnectionPtr> connVec;
	ASInternalNetwork::Instance()->FindConnectionByType(ST_TRANSFER, connVec);

	if (connVec.empty())
	{
		ErrorLogStream << "connection empty (" << (UInt32)ST_TRANSFER << ") when transmit player packet." << LogStream::eos;
		return;
	}

	UInt64 destId = protocol.tHeader.destObj != 0 ? protocol.tHeader.destObj : protocol.tHeader.destZone;
	UInt64 model = destId % UInt64(connVec.size());
	if (connVec[model] == NULL)
	{
		ErrorLogStream << "connection null (" << (UInt32)ST_TRANSFER << ") when transmit player packet." << LogStream::eos;
		return;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		connVec[model]->SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}