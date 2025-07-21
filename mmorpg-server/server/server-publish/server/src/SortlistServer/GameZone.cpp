#include "GameZone.h"

#include "Network.h"
#include <CLSysProtocol.h>

GameZone::GameZone()
	:m_Id(0)
{
}

GameZone::~GameZone()
{
}

bool GameZone::Init(UInt32 id)
{
	m_Conn = Network::Instance()->FindConnection(id);
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

void GameZone::TransmitToPlayer(ObjID_t playerId, Avalon::Packet* packet)
{
	CLProtocol::SysTransmitToPlayer req(packet);
	req.playerId = playerId;
	SendProtocol(req);
	req.Detach();
}

void GameZone::TransmitToPlayer(ObjID_t playerId, Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitToPlayer req;
	req.playerId = playerId;
	if(protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

void GameZone::TransmitToWorld(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes req;
	req.destnode = CL_MAKE_NODEID(GetZoneID(), ST_WORLD, 0);
	if(protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

