#include "GameZone.h"

#include <CLSysProtocol.h>
#include "CSNetwork.h"

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
	if(m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
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
