#include "WSSysHandler.h"
#include <AvalonLogger.h>
#include "CLRecordClient.h"
#include "WSRouter.h"
#include "WSPlayerMgr.h"
#include "WSNetwork.h"
#include "WorldGMSystem.h"

void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	Avalon::NetConnectionPtr node = WSNetwork::Instance()->FindConnection(protocol.destnode);
	if(node == NULL)
	{
		ErrorLogStream << "can not find connection(" << protocol.destnode
			<< ") when transmit packet." << LogStream::eos;
		return;
	}

	node->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransmitInNodesType& protocol)
{
	WSNetwork::Instance()->Broadcast(ServerType(protocol.destnodetype), protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransmitToPlayer& protocol)
{
	auto protocolid = protocol.packet ? protocol.packet->GetID() : 0;
	UInt32 nodeid = protocol.GetConnID();

	if (protocol.packet->GetID() == 500502)
	{
		return;
	}

	{
		// 记录团本消息
		if (WSApplication::Instance()->IsLogTeamCopyProtocol())
		{
			UInt32 servType = CL_GET_SERVERTYPE_FROM_PROTOCOLID(protocolid);
			UInt32 funcType = (protocolid % CL_SERVERTYPE_OFFSET_IN_PROTOCOLID) / 100;
			if (servType == ST_TEAMCOPY || servType == ST_DUNGEON_SVR || funcType == CLProtocol::FUNCTION_DUNGEON)
			{
				InfoLogStream << "[TCS_PROTOCOL] player(" << protocol.playerId << ") trans teamcopy protocol(" << protocol.packet->GetID() << ") from node(" << nodeid << ")." << LogStream::eos;
			}
		}
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		player->SendPacket(protocol.packet);
	}
	else
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find player(" << protocol.playerId << ") when trans protocol(" << protocolid << ")." << LogStream::eos;
	}
}

void HandleProtocol(CLProtocol::SysTransmitGmCommand& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerid);
	if(player != NULL)
	{
		GMSystem::Instance()->ExecuteCommand(player, protocol.cmdname, protocol.params);
	}
}

void HandleProtocol(CLProtocol::SysTransferProtocol& protocol)
{
	WSNetwork::Instance()->ExecuteTransferPacket(protocol.tHeader, protocol.packet);
}