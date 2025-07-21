#include "ObserveHandler.h"

#include <CLErrorCode.h>
#include <CLObserveProtocol.h>

#include "RelaySessionMgr.h"
#include "ObserveRelaySessionMgr.h"
#include "OBSRouter.h"
#include "OBSInternalNetwork.h"
#include "OBSRaceMgr.h"



void HandleProtocol(CLProtocol::SysTransmitRaceProtocolInNode& protocol)
{
    Avalon::NetConnectionPtr node = OBSInternalNetwork::Instance()->FindConnection(protocol.nodeId);
    if (node == NULL)
    {
        UInt32 packetId = protocol.packet ? protocol.packet->GetID() : 0;
        ErrorLogStream << "can not find connection(" << protocol.nodeId << ") when transmit race packet("
            << packetId << ")." << LogStream::eos;
        return;
    }

	node->SendPacket(protocol.packet);
}


void HandleProtocol(CLProtocol::UnionChampionObserveReq& protocol)
{
	InfoLogStream << "player(" << protocol.roleID << ") request observer race(" << protocol.raceID << ")." << LogStream::eos;

	auto race = OBSRaceMgr::Instance()->FindRace(protocol.raceID);
	if (race == nullptr)
	{
		InfoLogStream << "player(" << protocol.roleID << ") request observer race(" << protocol.raceID << "), race unexist." << LogStream::eos;
		CLProtocol::SceneChampionObserveRes req;
		req.roleID = protocol.roleID;
		req.addr.SetPort(0);
		req.addr.SetIP(0);
		req.raceID = protocol.raceID;
		OBSRouter::Instance()->SendToUnion(req);
	}

	//根据负载分配观战relay，并预增加负载
	auto relay = ObserveRelaySessionMgr::Instance()->SelectRelayServer();
	if (relay == nullptr)
	{
		InfoLogStream << "player(" << protocol.roleID << ") request observer race(" << protocol.raceID << "), no useful relayserver." << LogStream::eos;
		return;
	}

	//检查该比赛是否已在选出的观战Relay中创建
	if (!race->HaveOBSRelaySession(relay->GetNodeID()))
	{
		InfoLogStream << "player(" << protocol.roleID << ") request observer race(" << protocol.raceID << "), start create race in relay(" << relay->GetNodeID() << ")." << LogStream::eos;

		CLProtocol::ObserveRealyCreateRaceReq req;
		req.raceID = race->GetID();
		req.packet = race->GetInitPacket();
		req.frames = race->GetAllFrames();
		relay->SendProtocol(req);
		req.Detach();
		race->AddOBSRelay(relay->GetNodeID());
	}

	CLProtocol::ObserveRealyAddObservePlayerReq  req;
	req.raceID = protocol.raceID;
	req.roleID = protocol.roleID;
	relay->SendProtocol(req);
	
}

void HandleProtocol(CLProtocol::ObserveRealyAddObservePlayerRes & protocol)
{
	CLProtocol::SceneChampionObserveRes req;
	req.roleID = protocol.roleID;
	req.addr = protocol.addr;
	req.raceID = protocol.raceID;
	OBSRouter::Instance()->SendToUnion(req);
}

void HandleProtocol(CLProtocol::ObserveRealyCreateRaceRes & protocol)
{
	auto race = OBSRaceMgr::Instance()->FindRace(protocol.raceID);
	if (race == nullptr)
	{
		ErrorLogStream << "receive ObserveRealyCreateRaceRes,but can't find race" << protocol.raceID << LogStream::eos;
	}
	if (protocol.result == ErrorCode::SUCCESS)
	{
		//race->AddOBSRelay(protocol.GetConnID());
	}
}

void HandleProtocol(CLProtocol::ObserveStartRace& protocol)
{
	OBSRaceMgr::Instance()->OnRaceStart(protocol.raceID, protocol.GetConnID(), protocol.packet);
	protocol.Detach();
}

void HandleProtocol(CLProtocol::ObserveSyncFrame& protocol)
{
	OBSRaceMgr::Instance()->OnSyncFrame(protocol.raceId, protocol.frames);
}

void HandleProtocol(CLProtocol::ObserveCancelOverload& protocol)
{
	auto session = ObserveRelaySessionMgr::Instance()->FindSessionByNodeId(protocol.GetConnID());
	if (session == nullptr)
	{
		ErrorLogStream << "receive ObserveCancelOverload,but can't find session" << protocol.GetConnID() << LogStream::eos;
	}
	session->SetAbality(session->GetAbality() + 1);
}


void HandleProtocol(CLProtocol::RelayAbalitySync& protocol)
{
	auto session = ObserveRelaySessionMgr::Instance()->FindSessionByNodeId(protocol.GetConnID());
	if (session == nullptr)
	{
		ErrorLogStream << "receive ObserveCancelOverload,but can't find session" << protocol.GetConnID() << LogStream::eos;
	}
	session->SetAbality(protocol.abality);
}
