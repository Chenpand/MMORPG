#include "ObserveRelaySession.h"
#include "OBSInternalNetwork.h"

ObserveRelaySession::ObserveRelaySession()
{
	m_NodeId = 0;
	m_Abality = 0;
}

ObserveRelaySession::~ObserveRelaySession()
{

}

bool ObserveRelaySession::Init(Int32 id)
{
	m_NodeId = id;

	m_Conn = OBSInternalNetwork::Instance()->FindConnection(id);
	if (m_Conn == NULL)
	{
		return false;
	}

	//m_Conn->SetVerified();

	return true;
}

void ObserveRelaySession::OnTick(const Avalon::Time& now)
{

}

void ObserveRelaySession::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn, protocol);
	}
}

void ObserveRelaySession::SendPacket(Avalon::Packet* packet)
{
	if (m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}