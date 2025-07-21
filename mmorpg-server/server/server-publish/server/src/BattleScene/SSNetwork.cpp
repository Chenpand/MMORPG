#include "SSNetwork.h"

#include <CLRecordClient.h>
#include <CLProtocolDefine.h>
#include <CLReplayProtocol.h>

#include "SSRouter.h"
#include "SceneMgr.h"
#include "PlayerMgr.h"
#include "SSApplication.h"

SSNetwork::SSNetwork()
{
}

SSNetwork::~SSNetwork()
{
}

void SSNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch(servtype)
	{
	case ST_RECORD:
		{
			if(packet->GetID() != CLProtocol::RECORD_NOTIFY_OTHER_DISCONNECTED)
			{
				CLRecordClient::Instance()->OnPacketRecved(packet);
			}
			else
			{
				CLInternalNetwork::DispatchPacket(packet);
			}
		}
		break;
	default:
		{
			CLInternalNetwork::DispatchPacket(packet);
		}
		break;
	}
}

void SSNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_GATE)
	{
		SceneMgr::Instance()->OnGateConnected(id);
	}
	else if (servType == ST_CROSS)
	{
		SceneMgr::Instance()->OnCrossConnected(id);
	}
	else if (servType == ST_BATTLE)
	{
		SceneMgr::Instance()->OnBattleConnected(id);
	}
	else if (servType == ST_REPLAY)
	{
		class NotiyReplayPlayerOnlineVisit : public PlayerVisitor
		{
		public:
			NotiyReplayPlayerOnlineVisit() {}

			bool operator()(Player* player)
			{
				CLProtocol::ReplayReportPlayerOnline online;
				online.roleId = player->GetID();
				Router::SendToReplay(online);
				return true;
			}
		};
		NotiyReplayPlayerOnlineVisit vistor;
		PlayerMgr::Instance()->VisitNormalPlayers(vistor);
	}
}

void SSNetwork::OnDisconnected(UInt32 id)
{
	Router::OnDisconnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_GATE && SERVER_TYPE == ST_SCENE)
	{
		PlayerMgr::Instance()->OnGateDisconnected(id);
	}
}

void SSNetwork::OnShutdownService()
{
	//SSApplication::Instance()->Exit();
}
