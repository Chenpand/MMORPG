#include "GameZone.h"

#include "CSNetwork.h"
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
	m_Conn = CSNetwork::Instance()->FindConnection(id);
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

void GameZone::TransmitToWorld(Avalon::Protocol& protocol)
{
	Protocol::SysTransmitInNodes req;
	req.destnode = CL_MAKE_NODEID(GetZoneID(), ST_WORLD, 0);
	if(protocol.Encode(req.packet))
	{
		SendProtocol(req);
	}
}

