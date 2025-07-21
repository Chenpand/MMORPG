#include "CLVerifyNode.h"
#include <CLNetwork.h>

CLVerifyNode::CLVerifyNode()
{
	m_Id = 0;
	m_Abality = 0;
}

CLVerifyNode::~CLVerifyNode()
{

}

bool CLVerifyNode::Init(Int32 id, Avalon::SockAddr& addr, Avalon::NetConnectionPtr conn)
{
	if (conn == NULL)
	{
		return false;
	}

	m_Id = id;
	m_Addr = addr;
	m_Conn = conn;

	return true;
}

void CLVerifyNode::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Conn != NULL)
	{
		CL_SEND_PROTOCOL(m_Conn, protocol);
	}
}

void CLVerifyNode::SendPacket(Avalon::Packet* packet)
{
	if (m_Conn != NULL)
	{
		m_Conn->SendPacket(packet);
	}
}