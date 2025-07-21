#include "OBSRaceMgr.h"

#include <CLObserveProtocol.h>

#include "RelaySessionMgr.h"
#include "ObserveRelaySessionMgr.h"


void OBSRaceMgr::Init()
{
}

void OBSRaceMgr::Final()
{
}

void OBSRaceMgr::OnTick(const Avalon::Time& now)
{
	for (auto& iter: m_RaceMap)
	{
		iter.second.OnTick(now);
	}
}

void OBSRaceMgr::OnRaceStart(UInt64 raceID, UInt32 nodeID, Avalon::Packet* packet)
{
	if (FindRace(raceID) != nullptr)
	{
		ErrorLogStream << "race already started" << raceID << LogStream::eos;
	}
	m_RaceMap[raceID].Init(nodeID);
	m_RaceMap[raceID].SetID(raceID);
	m_RaceMap[raceID].SetInitPacket(packet);

	auto session = RelaySessionMgr::Instance()->FindSessionByNodeId(nodeID);
	if (session == nullptr)
	{
		ErrorLogStream << "Relay not exists:" << nodeID << LogStream::eos;
	}
	CLProtocol::RelayStartReceiveRaceFrame req;
	req.raceID = raceID;
	session->SendProtocol(req);
}

OBSRace* OBSRaceMgr::FindRace(UInt64 raceID)
{
	auto iter = m_RaceMap.find(raceID);
	if (iter == m_RaceMap.end())
	{
		return nullptr;
	}
	return &m_RaceMap[raceID];
}

void OBSRaceMgr::OnSyncFrame(UInt64 raceID, std::vector<CLProtocol::Frame>& frames)
{
	auto race = FindRace(raceID);
	if (race == nullptr)
	{
		ErrorLogStream << "race not exists:" << raceID << LogStream::eos;
	}
	race->OnSyncFrame(frames);
}

void OBSRace::Init(UInt32 nodeID)
{
	m_NodeID = nodeID;
}

void OBSRace::OnTick(const Avalon::Time& now)
{
	if (m_UnSyncFrames.empty())
	{
		return;
	}
	CLProtocol::ObserveSyncFrame sync;
	sync.raceId = GetID();
	sync.frames = m_UnSyncFrames;
	for (auto nodeID : m_OBSRealyNodeSet)
	{
		auto session = ObserveRelaySessionMgr::Instance()->FindSessionByNodeId(nodeID);
		if (session == nullptr)
		{
			ErrorLogStream << "OBSRelay session not exists" << nodeID << LogStream::eos;
			continue;;
		}
		session->SendProtocol(sync);
	}
	m_AllFrames.insert(m_AllFrames.end(), m_UnSyncFrames.begin(), m_UnSyncFrames.end());
	m_UnSyncFrames.clear();
}

void OBSRace::OnSyncFrame(std::vector<CLProtocol::Frame>& frames)
{
	m_UnSyncFrames.insert(m_UnSyncFrames.end(),frames.begin(), frames.end());
}

bool OBSRace::HaveOBSRelaySession(UInt32 nodeID)
{
	if (m_OBSRealyNodeSet.count(nodeID) > 0)
	{
		return true;
	}
	return false;
}

std::vector<CLProtocol::Frame>& OBSRace::GetAllFrames()
{
	return m_AllFrames;
}

void OBSRace::AddOBSRelay(UInt32 nodeID)
{
	if (HaveOBSRelaySession(nodeID) > 0)
	{
		return;
	}
	m_OBSRealyNodeSet.insert(nodeID);
}

void OBSRace::SendUnSyncFrame()
{


}

