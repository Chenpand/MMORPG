#include "CenterSysHandler.h"

#include <CLGateProtocol.h>
#include <CLGlobalActivityProtocol.h>
#include "GameZoneMgr.h"
#include "CSApplication.h"


void HandleProtocol(CLProtocol::SysTransmitInNodes& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.destnode);
	if (!zone)
	{
		ErrorLogStream << "Can not find admin node(" << protocol.destnode << ")!" << LogStream::eos;
		return;
	}

	zone->SendPacket(protocol.packet);
}

void HandleProtocol(CLProtocol::SysTransmitPlayerFormNodesToGlobal& protocol)
{
	GameZone* zone = GameZoneMgr::Instance()->FindGameZone(protocol.destNode);
	if (zone == NULL)
	{
		GameZone* adminZone = GameZoneMgr::Instance()->FindGameZone(protocol.playerInfo.sourceNode[ST_ADMIN]);
		if (!adminZone)
		{
			ErrorLogStream << "Can not find admin node(" << protocol.playerInfo.sourceNode[ST_ADMIN] << ")!" << LogStream::eos;
			return;
		}

		CLProtocol::GASNonExistent gasProtocol;

		CLProtocol::GateTransmitPacket gateProtocol;
		gateProtocol.id = protocol.playerInfo.roleId;
		gasProtocol.Encode(gateProtocol.packet);

		CLProtocol::SysTransmitInNodes trans;
		trans.destnode = protocol.playerInfo.sourceNode[ST_GATE];
		gateProtocol.Encode(trans.packet);

		adminZone->SendProtocol(trans);

		ErrorLogStream << "Can not find dest node(" << protocol.destNode << ")!" << LogStream::eos;
		return;
	}

	protocol.playerInfo.sourceNode[ST_CENTER] = NODE_ID;
	zone->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::SysTransmitFromGlobalToNode& protocol)
{
	DebugLogStream << "Transmit protocol(" << protocol.GetProtocolID() << ") from global to dest server(" << protocol.destServer << ")..." << LogStream::eos;

	switch ((ServerType)protocol.destServer)
	{
	case ST_WORLD:
		GameZoneMgr::Instance()->BroadcastToGameZone(protocol); break;
	case ST_GATE:
		GameZoneMgr::Instance()->BroadcastToGameZone(protocol); break;

	default:
		break;
	}
}