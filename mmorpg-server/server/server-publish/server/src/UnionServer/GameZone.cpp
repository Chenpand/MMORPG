#include "GameZone.h"

#include "USNetwork.h"
#include <CLSysProtocol.h>

GameZone::GameZone() : m_Id(0)
{
}

GameZone::~GameZone()
{
}

bool GameZone::Init(UInt32 id)
{
	m_Conn = USNetwork::Instance()->FindConnection(id);
	if(m_Conn == NULL) return false;
	m_Id = id;
	return true;
}

void GameZone::SendPacket(Avalon::Packet* packet)
{
	m_Conn->SendPacket(packet);
}

void GameZone::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GameZone::TransmitToWorld(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodesType req(packet);
	req.destnodetype = (UInt8)ST_WORLD;
	SendProtocol(req);
	req.Detach();
}

void GameZone::TransmitToWorld(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodesType req;
	req.destnodetype = (UInt8)ST_WORLD;
	if (protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

void GameZone::TransmitToScene(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodesType req(packet);
	req.destnodetype = (UInt8)ST_SCENE;
	SendProtocol(req);
	req.Detach();
}

void GameZone::TransmitToScene(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodesType req;
	req.destnodetype = (UInt8)ST_SCENE;
	if (protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

void GameZone::SendToPlayer(ObjID_t playerId, Avalon::Packet* packet)
{
	CLProtocol::SysTransmitToPlayer req(packet);
	req.playerId = playerId;
	SendProtocol(req);
	req.Detach();
}

void GameZone::SendToPlayer(ObjID_t playerId, Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitToPlayer req;
	req.playerId = playerId;
	if (protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

void GameZone::SendToScenePlayer(ObjID_t playerId, UInt32 sceneNodeId, Avalon::Packet * packet)
{
	CLProtocol::SysTransmitPlayerProtocolInNode msg;
	msg.playerId = playerId;
	msg.nodeId = sceneNodeId;
	msg.packet = packet;
	SendProtocol(msg);
	msg.Detach();
}

void GameZone::SendToScenePlayer(ObjID_t playerId, UInt32 sceneNodeId, Avalon::Protocol & protocol)
{
	CLProtocol::SysTransmitPlayerProtocolInNode msg;
	msg.playerId = playerId;
	msg.nodeId = sceneNodeId;
	if (protocol.Encode(msg.packet))
	{
		SendProtocol(msg);
	}
}

