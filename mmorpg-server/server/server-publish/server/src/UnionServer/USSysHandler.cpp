#include "USSysHandler.h"

#include "USGMSystem.h"
#include "USNetwork.h"
#include "GameZoneMgr.h"
#include "USPlayerMgr.h"
#include "USceneMgr.h"
#include "USApplication.h"
#include "UnionServiceMgr.h"

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol)
{
	USGMSystem::Instance()->ExecuteCommand(protocol.playerid, protocol.cmdname, protocol.params);
}

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.destnode);
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		DebugLogStream << "not find zone  destnode : " << protocol.destnode << "zone : " << zoneid << LogStream::eos;
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerProtocolInNode& protocol)
{
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.nodeId);
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerToUnion& protocol)
{
	UnionServiceMgr::Instance()->OnReceivePlayerProtocol(protocol);
}

void HandleProtocol(CLProtocol::GateNotifyLeaveConnect& protocol)
{
	InfoLogStream << "receive player leave connect,player id is " << protocol.roleId << LogStream::eos;
	UnionServiceMgr::Instance()->OnPlayerDisconnect(protocol.roleId);
}

void HandleProtocol(CLProtocol::UnionSceneIsReady & protocol)
{
	InfoLogStream << "receive scene ready,conn id is " << protocol.GetConnID() << "seq id is " << protocol.id << LogStream::eos;
	UnionServiceMgr::Instance()->OnSceneReady(protocol);
}

void HandleProtocol(CLProtocol::UnionWorldChgZoneID & protocol)
{
	InfoLogStream << "receive UnionWorldChgZoneID ,new zone id is " << protocol.newZoneID << LogStream::eos;
	std::string str;
	for (auto id : protocol.oldZoneIDs)
	{
		UnionServiceMgr::Instance()->OnWorldChgNewZoneID(id, protocol.newZoneID);
		str = str + "," + std::to_string(id);
	}
	InfoLogStream << "receive UnionWorldChgZoneID ,old zone ids are " << str << LogStream::eos;
}

void HandleProtocol(CLProtocol::UnionNotifyActivityInfo& protocol)
{
	UnionServiceMgr::Instance()->OnActivityNotify(protocol);
}
