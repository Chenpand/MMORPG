#include "BSNetwork.h"

#include "BSApplication.h"
#include "GameZoneMgr.h"
#include "BSRoomMgr.h"
#include "BattleScene.h"
#include "BattleSceneMgr.h"
#include "RelaySessionMgr.h"
#include "BSRouter.h"
#include "BSLostBattleMgr.h"

BSNetwork::BSNetwork()
{
	//m_RelayConn = NULL;
}

BSNetwork::~BSNetwork()
{

}

void BSNetwork::OnConnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnConnected(id);
	}
	else if(servType == ST_BSCENE)
	{
		BattleSceneMgr::Instance()->OnSceneServerConnected(id);
		BSLostBattleMgr::Instance()->OnSceneServerConnected(id);
	}
	else if (servType == ST_RELAY)
	{
		//m_RelayConn = FindConnection(id);
		RelaySessionMgr::Instance()->OnRelayServerConnected(id);
	}

	Router::OnConnected(id);
}

void BSNetwork::OnDisconnected(UInt32 id)
{
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if(servType == ST_ADMIN)
	{
		GameZoneMgr::Instance()->OnDisconnected(id);
		BSRoomMgr::Instance()->OnDisconnect(id);
	}
	else if (servType == ST_BSCENE)
	{
		BattleSceneMgr::Instance()->OnSceneServerDisconnected(id);
		BSLostBattleMgr::Instance()->OnSceneServerDisconnected(id);
	}
	else if (servType == ST_RELAY)
	{
		//m_RelayConn = NULL;
		RelaySessionMgr::Instance()->OnRelayServerDisconnected(id);
	}
}

void BSNetwork::OnShutdownService()
{
	BSApplication::Instance()->Exit();
}

//void BSNetwork::SendToRelay(Avalon::Protocol& protocol)
//{
//	if (m_RelayConn != NULL)
//	{
//		CL_SEND_PROTOCOL(m_RelayConn, protocol);
//	}
//}
//
//void BSNetwork::SendToRelay(Avalon::Packet* packet)
//{
//	if (m_RelayConn != NULL)
//	{
//		m_RelayConn->SendPacket(packet);
//	}
//}

void BSNetwork::SendToBScene(UInt32 nodeId, Avalon::Protocol& protocol)
{
	auto conn = this->FindConnection(nodeId);
	if (conn == NULL)
	{
		InfoLogStream << "scene node id: " << nodeId << " can't find connection" << LogStream::eos;
		return;
	}

	InfoLogStream << "scene node id: " << nodeId << ", msg id:" << protocol.GetProtocolID() << LogStream::eos;
	CL_SEND_PROTOCOL(conn, protocol);
}

void BSNetwork::SendToBScene(UInt32 nodeId, Avalon::Packet* packet)
{
	auto conn = this->FindConnection(nodeId);
	if (conn == NULL)
	{
		InfoLogStream << "scene node id: " << nodeId << " can't find connection" << LogStream::eos;
		return;
	}

	InfoLogStream << "scene node id: " << nodeId << ", msg id:" << packet->GetID() << LogStream::eos;
	conn->SendPacket(packet);
}
