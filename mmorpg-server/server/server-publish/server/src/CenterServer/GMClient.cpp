#include "GMClient.h"
#include "GMNetwork.h"

GMClient::GMClient()
{
	m_Id = 0;
}

GMClient::~GMClient()
{
}

bool GMClient::Init(UInt32 id)
{
	m_Conn = GMNetwork::Instance()->FindConnection(id);
	if(m_Conn == NULL) return false;
	m_Id = id;
	return true;
}

void GMClient::SetChatMonitorServers(const std::vector<UInt32>& servers)
{
	m_ChatMonitorServers.insert(servers.begin(), servers.end());
}

void GMClient::SendPacket(Avalon::Packet* packet)
{
	m_Conn->SendPacket(packet);
}

void GMClient::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}
