#include "WSNetwork.h"

#include "CLProtocolDefine.h"
#include "CLRecordClient.h"
#include "WSApplication.h"

#include "WSRouter.h"
#include "WSSceneMgr.h"
#include "WSPlayerMgr.h"
#include "WSSortListMgr.h"
#include "WSActivityMgr.h"
#include "GameParamMgr.h"
#include "RelaySessionMgr.h"
#include "WSServiceSwitchMgr.h"
#include "WSDungeonConfigMgr.h"
#include "WSSeasonMgr.h"
#include "FigureStatueMgr.h"

#include <CLWorldLevelProtocol.h>

WSNetwork::WSNetwork()
{
	RegisterTransferProtocols();
}

WSNetwork::~WSNetwork()
{
}

bool WSNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet)) return true;
	return false;
}

void WSNetwork::DispatchPacket(Avalon::Packet* packet)
{
	UInt32 servtype = CL_GET_SERVERTYPE_FROM_PROTOCOLID(packet->GetID());

	switch(servtype)
	{
	case ST_RECORD:
		{
			CLRecordClient::Instance()->OnPacketRecved(packet);
		}
		break;
	default:
		{
			CLInternalNetwork::DispatchPacket(packet);
		}
		break;
	}
}

void WSNetwork::OnConnected(UInt32 id)
{
	Router::OnConnected(id);

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_SCENE)
	{
		WSSceneMgr::Instance()->OnSceneConnected(id);
		WSSortListMgr::Instance()->OnSceneConnected(id);
		WSActivityMgr::Instance()->OnSceneConnected(id);
		WSServiceSwitchMgr::Instance()->OnSceneConnected(id);
		WSDungeonConfigMgr::Instance()->OnSceneConnected(id);
		WSSeasonMgr::Instance()->OnSceneConnected(id);
		FigureStatueMgr::Instance()->OnSceneConnected(id);
		GameParamMgr::Instance()->OnSceneConnected(id);
		
		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
		if(conn != NULL)
		{
			CLProtocol::WorldNotifyWorldLevelToS	notify;
			notify.uWorldLevel = WORLD_LEVEL;
			CL_SEND_PROTOCOL(conn, notify);
		}
	}
	else if (servType == ST_RELAY)
	{
		if (!RelaySessionMgr::Instance()->OnRelayServerConnected(id))
		{
			Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
			if (conn != NULL)
			{
				conn->Close();
			}
		}
	}
	else if (servType == ST_GATE)
	{
		WSActivityMgr::Instance()->OnGateConnected(id);
		GameParamMgr::Instance()->OnGateConnected(id);
		WSServiceSwitchMgr::Instance()->OnGateConnected(id);
	}
	else if (servType == ST_ADMIN)
	{
		GameParamMgr::Instance()->OnAdminConnected(id);
	}
}

void WSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_SCENE)
	{
		WSSceneMgr::Instance()->OnSceneDisconnected(id);
		WSPlayerMgr::Instance()->OnSceneDisconnected(id);
	}
	else if (servType == ST_RELAY)
	{
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}

	Router::OnDisconnected(id);
}

void WSNetwork::OnShutdownService()
{
	WSApplication::Instance()->Exit();
}

UInt32 WSNetwork::SelectRandSceneServer()
{
	class SceneServerVisitor : public Avalon::NetConnectionVisitor
	{
	public:
		bool operator()(Avalon::NetConnection* conn)
		{
			UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(conn->GetID());
			if (servType == ST_SCENE)
			{
				vecConn.push_back(conn->GetID());
			}

			return true;
		}

		std::vector<UInt32> vecConn;
	};
	SceneServerVisitor vistor;
	WSNetwork::Instance()->VisitConnections(vistor);

	if (vistor.vecConn.empty())
	{
		return 0;
	}

	return vistor.vecConn[Avalon::Random::RandBetween(0, vistor.vecConn.size() - 1)];
}
