#include "TCSSysHandler.h"

#include "TCSNetwork.h"
#include "GameZoneMgr.h"
#include "TCSPlayerMgr.h"
#include "TCSGMSystem.h"
#include "TCSceneMgr.h"
#include "TCSTeamMgr.h"
#include "TCSApplication.h"

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol)
{
	TCSGMSystem::Instance()->ExecuteCommand(protocol.playerid, protocol.cmdname, protocol.params);
}

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	auto zoneid = CL_GET_ZONEID_FROM_NODEID(protocol.destnode);
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(zoneid);
	if (!zone)
	{
		DebugLogStream << "not find zone destnode: " << protocol.destnode << " zone: " << zoneid << LogStream::eos;
		return;
	}

	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol)
{
	auto protocolid = protocol.packet ? protocol.packet->GetID() : 0;
	//UInt32 nodeid = protocol.GetConnID();

	TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.playerId);
	if (NULL == player)
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find player(" << protocol.playerId << ") when trans protocol(" << protocolid << ")." << LogStream::eos;
		return;
	}

	if (TCSApplication::Instance()->IsLogTransProtocol())
	{
		//DebugLogStream << "[TCS_PROTOCOL] player(" << protocol.playerId << ") trans protocol(" << protocolid << ") from node(" << nodeid << ") to node(" << player->GetConnId() << ")." << LogStream::eos;
	}

	player->SendPacket(protocol.packet);
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

void HandleProtocol(CLProtocol::SysTransmitPlayerToTeamCopy& protocol)
{
	TCSPlayerMgr::Instance()->ExecuteProtocol(protocol);
}

void HandleProtocol(CLProtocol::GateNotifyLeaveConnect& protocol)
{
	TCSPlayerMgr::Instance()->AddExpire(protocol.roleId, protocol.leaveTime);
}

void HandleProtocol(CLProtocol::SceneSyncChat& protocol)
{
	switch (protocol.channel)
	{
	case CHAT_CHANNEL_TEAM_COPY_PREPARE:
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(protocol.objid);
		if (NULL == player)
		{
			break;
		}

		TCSceneMgr::Instance()->BroadcastProtocol(protocol, player->GetTeamType());
		break;
	}
	case CHAT_CHANNEL_TEAM_COPY_TEAM:
	{
		UInt32 teamId = TCSTeamMgr::Instance()->GetPlayerTeamId(protocol.objid);
		if (teamId != 0)
		{
			TCSTeamMgr::Instance()->BroadcastToTeam(protocol, teamId);
		}
		break;
	}
	case CHAT_CHANNEL_TEAM_COPY_SQUAD:
	{
		UInt32 teamId = TCSTeamMgr::Instance()->GetPlayerTeamId(protocol.objid);
		if (teamId != 0)
		{
			TCSTeamMgr::Instance()->BroadcastToSquad(protocol, teamId, protocol.objid);
		}
		break;
	}
	default:
		break;
	}
}

