#include "WSChangeZoneHandler.h"

#include <CLGateProtocol.h>
#include "WSPlayerMgr.h"
#include "WSNetwork.h"

void HandleProtocol(CLProtocol::GateChangeZoneCheckRet& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		player->SendToScene(protocol);
	}
	else
	{
		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(protocol.srcGateId);
		if(conn != NULL)
		{
			CLProtocol::WorldChangeZoneNotifyClient notify;
			notify.playerId = protocol.playerId;
			notify.dstZoneId = protocol.zoneId;
			notify.attr = protocol.gateAddr;
			memmove(notify.key, protocol.key, 20);

			Avalon::Packet* packet = Avalon::Packet::Create();
			if(notify.Encode(packet))
			{
				CLProtocol::GateTransmitPacket req(packet);
				req.id = protocol.playerId;
				CL_SEND_PROTOCOL(conn, req);
				req.Detach();
			}
			Avalon::Packet::Destroy(packet);
		}
	}
}

void HandleProtocol(CLProtocol::WorldChangeZoneNotify& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	CLProtocol::WorldChangeZoneNotifyClient notify;
	notify.playerId = protocol.playerId;
	notify.dstZoneId = protocol.dstZoneId;
	notify.attr = protocol.attr;
	memmove(notify.key, protocol.key, 20);
	player->SendProtocol(notify);

	GameInfoLogStream("Player") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") change to zone "
		<< protocol.dstZoneId << LogStream::eos;

	player->LeaveGame();
	WSPlayerMgr::Instance()->RemovePlayer(player);
	CL_SAFE_DELETE(player);
}

