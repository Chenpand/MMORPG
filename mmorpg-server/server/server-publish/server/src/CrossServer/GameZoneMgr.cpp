#include "GameZoneMgr.h"
#include "CSGuildBattleMgr.h"
#include <CLCenterProtocol.h>

GameZoneMgr::GameZoneMgr()
{
}

GameZoneMgr::~GameZoneMgr()
{
}

bool GameZoneMgr::Init()
{
	return true;
}

void GameZoneMgr::Final()
{
	CLDeleteVisitor<GameZone> delVisitor;
	m_Zones.VisitObjects(delVisitor);
	m_Zones.Clear();

	m_IdZones.clear();
}

bool GameZoneMgr::AddGameZone(GameZone* zone)
{
	if(!m_Zones.AddObject(zone))
	{
		return false;
	}

	if(!m_IdZones.insert(std::make_pair(zone->GetZoneID(), zone)).second)
	{
		m_Zones.RemoveObject(zone);
		return false;
	}
	return true;
}

void GameZoneMgr::RemoveGameZone(GameZone* zone)
{
	m_Zones.RemoveObject(zone);
	m_IdZones.erase(zone->GetZoneID());
}

GameZone* GameZoneMgr::FindGameZone(UInt32 id)
{
	return m_Zones.FindObject(id);
}

GameZone* GameZoneMgr::FindGameZoneByID(UInt32 id)
{
	GameZoneMap::iterator iter = m_IdZones.find(id);
	if(iter != m_IdZones.end()) return iter->second;
	return NULL;
}

void GameZoneMgr::VisitGameZones(GameZoneVisitor& visitor)
{
	m_Zones.VisitObjects(visitor);
}

void GameZoneMgr::BroadcastToGameZone(Avalon::Packet* packet)
{
	class BroadcastVisitor : public GameZoneVisitor
	{
	public:
		BroadcastVisitor(Avalon::Packet* packet)
			:m_pPacket(packet){}

		bool operator()(GameZone* zone)
		{
			zone->SendPacket(m_pPacket);
			return true;
		}
	private:
		Avalon::Packet*	m_pPacket;
	};
	BroadcastVisitor visitor(packet);
	VisitGameZones(visitor);
}

void GameZoneMgr::BroadcastToGameZone(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		BroadcastToGameZone(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GameZoneMgr::TransmitToWorld(Avalon::Protocol& protocol)
{
	class TransmitToWorldVisitor : public GameZoneVisitor
	{
	public:
		TransmitToWorldVisitor(Avalon::Protocol& protocol)
			:m_Protocol(protocol){}

		bool operator()(GameZone* zone)
		{
			zone->TransmitToWorld(m_Protocol);
			return true;
		}
	private:
		Avalon::Protocol& m_Protocol;
	};
	TransmitToWorldVisitor visitor(protocol);
	VisitGameZones(visitor);
}

void GameZoneMgr::TransmitToScene(Avalon::Protocol& protocol)
{
	class TransmitToSceneVisitor : public GameZoneVisitor
	{
	public:
		TransmitToSceneVisitor(Avalon::Protocol& protocol)
			:m_Protocol(protocol){}
		
		bool operator()(GameZone* zone)
		{
			zone->TransmitToScene(m_Protocol);
			return true;
		}
	private:
		Avalon::Protocol& m_Protocol;
	};

	TransmitToSceneVisitor visitor(protocol);
	VisitGameZones(visitor);
}

void GameZoneMgr::TransmitToPlayer(ObjID_t playerId, Avalon::Protocol& protocol)
{
	class TransmitToPlayerVisitor : public GameZoneVisitor
	{
	public:
		TransmitToPlayerVisitor(ObjID_t playerId, Avalon::Protocol& protocol)
			:m_PlayerId(playerId), m_Protocol(protocol){}

		bool operator()(GameZone* zone)
		{
			zone->SendToPlayer(m_PlayerId, m_Protocol);
			return true;
		}
	private:
		ObjID_t m_PlayerId;
		Avalon::Protocol& m_Protocol;
	};

	TransmitToPlayerVisitor visitor(playerId, protocol);
	VisitGameZones(visitor);
}

void GameZoneMgr::OnTick(const Avalon::Time& now)
{
}

void GameZoneMgr::OnConnected(UInt32 id)
{
	GameZone* zone = new GameZone();
	if(!zone->Init(id))
	{
		CL_SAFE_DELETE(zone);
		return;
	}
	if(!AddGameZone(zone))
	{
		CL_SAFE_DELETE(zone);
		return;
	}
}

void GameZoneMgr::OnDisconnected(UInt32 id)
{
	GameZone* zone = FindGameZone(id);
	if(zone != NULL)
	{
		RemoveGameZone(zone);
		CL_SAFE_DELETE(zone);
	}
}
