#include "RelaySession.h"
#include "MSInternalNetwork.h"

RelaySession::RelaySession()
{
	m_NodeId = 0;
	m_Abality = 0;
}

RelaySession::~RelaySession()
{

}

bool RelaySession::Init(Int32 id)
{
	m_NodeId = id;

	m_Conn = MSInternalNetwork::Instance()->FindConnection(id);
	if (m_Conn == NULL)
	{
		return false;
	}

	//m_Conn->SetVerified();

	return true;
}

void RelaySession::OnTick(const Avalon::Time& now)
{

}

void RelaySession::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn, protocol);
	}
}

void RelaySession::SendPacket(Avalon::Packet* packet)
{
	if (m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}