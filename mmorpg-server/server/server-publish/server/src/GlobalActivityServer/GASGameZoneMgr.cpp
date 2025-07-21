#include "GASGameZoneMgr.h"
#include <CLCenterProtocol.h>

GASGameZoneMgr::GASGameZoneMgr()
{
}

GASGameZoneMgr::~GASGameZoneMgr()
{
}

void GASGameZoneMgr::Final()
{
	CLDeleteVisitor<GASGameZone> delVisitor;
	m_Zones.VisitObjects(delVisitor);
	m_Zones.Clear();
}

bool GASGameZoneMgr::AddGASGameZone(GASGameZone* zone)
{
	if(!m_Zones.AddObject(zone))
	{
		return false;
	}

	return true;
}

void GASGameZoneMgr::RemoveGASGameZone(GASGameZone* zone)
{
	m_Zones.RemoveObject(zone);
}

GASGameZone* GASGameZoneMgr::FindGASGameZone(UInt32 id)
{
	return m_Zones.FindObject(id);
}

void GASGameZoneMgr::VisitGASGameZones(GASGameZoneVisitor& visitor)
{
	m_Zones.VisitObjects(visitor);
}

void GASGameZoneMgr::BroadcastToGASGameZone(Avalon::Packet* packet)
{
	class BroadcastVisitor : public GASGameZoneVisitor
	{
	public:
		BroadcastVisitor(Avalon::Packet* packet)
			:m_pPacket(packet){}

		bool operator()(GASGameZone* zone)
		{
			zone->SendPacket(m_pPacket);
			return true;
		}
	private:
		Avalon::Packet*	m_pPacket;
	};
	BroadcastVisitor visitor(packet);
	VisitGASGameZones(visitor);
}

void GASGameZoneMgr::BroadcastToGASGameZone(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		BroadcastToGASGameZone(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GASGameZoneMgr::BroadcastToWorld(Avalon::Protocol& protocol)
{
	class TransmitToWorldVisitor : public GASGameZoneVisitor
	{
	public:
		TransmitToWorldVisitor(Avalon::Protocol& protocol)
			:m_Protocol(protocol){}

		bool operator()(GASGameZone* zone)
		{
			zone->BroadcastToWorld(m_Protocol);
			return true;
		}
	private:
		Avalon::Protocol& m_Protocol;
	};
	TransmitToWorldVisitor visitor(protocol);
	VisitGASGameZones(visitor);
}

void GASGameZoneMgr::BroadcastToGate(Avalon::Protocol& protocol)
{
	class TransmitToGateVisitor : public GASGameZoneVisitor
	{
	public:
		TransmitToGateVisitor(Avalon::Protocol& protocol)
			:m_Protocol(protocol){}

		bool operator()(GASGameZone* zone)
		{
			zone->BroadcastToGate(m_Protocol);
			return true;
		}
	private:
		Avalon::Protocol& m_Protocol;
	};

	TransmitToGateVisitor visitor(protocol);
	VisitGASGameZones(visitor);
}

void GASGameZoneMgr::OnConnected(UInt32 id)
{
	GASGameZone* zone = new GASGameZone();
	if(!zone->Init(id))
	{
		ErrorLogStream << "Zone(" << zone->GetID() << ") init failed!" << LogStream::eos;
		CL_SAFE_DELETE(zone);
		return;
	}
	if(!AddGASGameZone(zone))
	{
		ErrorLogStream << "Zone(" << zone->GetID() << ") add failed!" << LogStream::eos;
		CL_SAFE_DELETE(zone);
		return;
	}

	DebugLogStream << "Success added zone(" << zone->GetID() << ")." << LogStream::eos;
}

void GASGameZoneMgr::OnDisconnected(UInt32 id)
{
	GASGameZone* zone = FindGASGameZone(id);
	if(zone != NULL)
	{
		DebugLogStream << "Zone(" << zone->GetID() << ") disconnected." << LogStream::eos;
		RemoveGASGameZone(zone);
		CL_SAFE_DELETE(zone);
	}
}
