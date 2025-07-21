#include "GateSession.h"
#include "ASInternalNetwork.h"

GateSession::GateSession()
{
	m_Id = 0;
	m_PlayerNum = 0;
	m_MaxPlayerNum = 0;
}

GateSession::~GateSession()
{
}

bool GateSession::Init(UInt32 connid)
{
	m_Conn = ASInternalNetwork::Instance()->FindConnection(connid);
	if(m_Conn == NULL) return false;

	m_Id = connid;

	return true;
}

UInt32 GateSession::GetOnlinePercent() const
{
	if(m_MaxPlayerNum == 0) return 0;

	return m_PlayerNum * 100 / m_MaxPlayerNum;
}

void GateSession::SendProtocol(Avalon::Protocol &protocol)
{
	if(m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn,protocol);
	}
}

void GateSession::SendPacket(Avalon::Packet* packet)
{
	if(m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}
