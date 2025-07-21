#include "VerifyClient.h"
#include "VCInternalNetwork.h"

bool VerifyClient::Init(Int32 id, ServerType type)
{
	m_Id = id;
	m_ServerType = type;

	m_Conn = VCInternalNetwork::Instance()->FindConnection(id);
	if (m_Conn == NULL)
	{
		return false;
	}

	return true;
}

void VerifyClient::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn, protocol);
	}
}

void VerifyClient::SendPacket(Avalon::Packet* packet)
{
	if (m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}