#include "VerifyClientMgr.h"
#include "VerifyServerMgr.h"
#include <CLVerifyProtocol.h>

VerifyClient* VerifyClientMgr::FindClient(UInt32 id)
{
	auto itr = m_Clients.find(id);
	return itr == m_Clients.end() ? NULL : itr->second;
}

void VerifyClientMgr::AddClient(VerifyClient* client)
{
	if (!client)
	{
		return;
	}

	m_Clients[client->GetID()] = client;

	InfoLogStream << "add new verify client(" << client->GetID() << ")." << LogStream::eos;

	// 通知所有的验证服务器节点
	CLProtocol::VerifyCenterNotifyVSInfo notify;
	notify.nodes = VerifyServerMgr::Instance()->GetAllNodes();
	client->SendProtocol(notify);
}

void VerifyClientMgr::DelClient(UInt32 id)
{
	auto itr = m_Clients.find(id);
	if (itr == m_Clients.end())
	{
		return;
	}

	delete itr->second;
	m_Clients.erase(itr);

	InfoLogStream << "delete verify client(" << id << ")." << LogStream::eos;
}

void VerifyClientMgr::Broadcast(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (!protocol.Encode(packet))
	{
		Avalon::Packet::Destroy(packet);
		return;
	}

	Broadcast(packet);
	Avalon::Packet::Destroy(packet);
}

void VerifyClientMgr::Broadcast(Avalon::Packet* packet)
{
	for (auto itr : m_Clients)
	{
		auto client = itr.second;
		if (client)
		{
			client->SendPacket(packet);
		}
	}
}