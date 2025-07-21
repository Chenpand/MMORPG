#include "ServiceSession.h"

#include "DSNetwork.h"
#include <CLSysProtocol.h>

ServiceSession::ServiceSession()
	:m_Id(0)
{
}

ServiceSession::~ServiceSession()
{
}

bool ServiceSession::Init(UInt32 id)
{
	m_Conn = DSNetwork::Instance()->FindConnection(id);
	if(m_Conn == NULL) return false;
	m_Id = id;
	m_Key.serviceType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	m_Key.serviceId = CL_GET_SERVERID_FROM_NODEID(id);
	return true;
}

void ServiceSession::SendPacket(Avalon::Packet* packet)
{
	m_Conn->SendPacket(packet);
}

void ServiceSession::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

