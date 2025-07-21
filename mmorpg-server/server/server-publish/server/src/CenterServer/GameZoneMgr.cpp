#include "GameZoneMgr.h"

#include <CLCenterProtocol.h>
#include "PunishmentMgr.h"

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

GameZone* GameZoneMgr::FindGameZoneByID(UInt32 zoneId)
{
	GameZoneMap::iterator iter = m_IdZones.find(zoneId);
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

	CLProtocol::CenterNotifyConnected notify;
	zone->SendProtocol(notify);

	PunishmentMgr::Instance()->OnGameZoneConnected(zone);
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
