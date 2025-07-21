#include "GMClientMgr.h"

GMClientMgr::GMClientMgr()
{
}

GMClientMgr::~GMClientMgr()
{
}

bool GMClientMgr::Init()
{
	return true;
}

void GMClientMgr::Final()
{
	CLDeleteVisitor<GMClient> delVisitor;
	m_Clients.VisitObjects(delVisitor);
	m_Clients.Clear();
}

bool GMClientMgr::AddGMClient(GMClient* client)
{
	return m_Clients.AddObject(client);
}

void GMClientMgr::RemoveGMClient(GMClient* client)
{
	m_Clients.RemoveObject(client);
}

GMClient* GMClientMgr::FindGMClient(UInt32 id)
{
	return m_Clients.FindObject(id);
}

void GMClientMgr::VisitGMClients(GMClientVisitor& visitor)
{
	m_Clients.VisitObjects(visitor);
}

void GMClientMgr::BroadcastToGMClient(Avalon::Packet* packet)
{
	class BroadcastVisitor : public GMClientVisitor
	{
	public:
		BroadcastVisitor(Avalon::Packet* packet)
			:m_pPacket(packet){}

		bool operator()(GMClient* client)
		{
			client->SendPacket(m_pPacket);
			return true;
		}
	private:
		Avalon::Packet*	m_pPacket;
	};
	BroadcastVisitor visitor(packet);
	VisitGMClients(visitor);
}

void GMClientMgr::BroadcastToGMClient(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		BroadcastToGMClient(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GMClientMgr::OnTick(const Avalon::Time& now)
{
}

void GMClientMgr::OnConnected(UInt32 id)
{
	GMClient* client = new GMClient();
	if(!client->Init(id))
	{
		CL_SAFE_DELETE(client);
		return;
	}
	if(!AddGMClient(client))
	{
		CL_SAFE_DELETE(client);
		return;
	}
}

void GMClientMgr::OnDisconnected(UInt32 id)
{
	GMClient* client = FindGMClient(id);
	if(client != NULL)
	{
		RemoveGMClient(client);
		CL_SAFE_DELETE(client);
	}
}
