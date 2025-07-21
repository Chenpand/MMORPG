#include "SceneSession.h"

#include "DSNetwork.h"
#include <CLSysProtocol.h>

SceneSession::SceneSession()
	:m_Id(0)
{
}

SceneSession::~SceneSession()
{
}

bool SceneSession::Init(UInt32 id)
{
	m_Conn = DSNetwork::Instance()->FindConnection(id);
	if(m_Conn == NULL) return false;
	m_Id = id;
	return true;
}

void SceneSession::SendPacket(Avalon::Packet* packet)
{
	m_Conn->SendPacket(packet);
}

void SceneSession::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

