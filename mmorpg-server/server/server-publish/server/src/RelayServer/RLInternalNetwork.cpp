#include "CLProtocolDefine.h"
#include "RLInternalNetwork.h"
#include "CLGameSessionProtocol.h"
#include "RLApplication.h"
#include "RelaySvrRouter.h"
#include "RLPlayerMgr.h"



RLInternalNetwork::RLInternalNetwork()
{
	m_reconnect = false;
}

RLInternalNetwork::~RLInternalNetwork()
{

}

void RLInternalNetwork::OnShutdownService()
{
	RLApplication::Instance()->Exit();
}

Avalon::NetConnection* RLInternalNetwork::CreateConnection()
{
	return new RLInternalConnection(this);
}

void RLInternalNetwork::OnConnected(UInt32 id)
{
	//
	Router::OnConnected(id);

	//其他的处理逻辑
	//register to match server  虽然CLInternalNetwork::ConnectToServers()在连接上match server时会发送 SYS_REGISTER_REQ
	//但relay server要把自身的地址信息以及承载能力信息注册给 match server
	//最新方案：在响应协议MATCHSERVER_START_GAME_RES中返回 relay server的地址信息以及承载信息

// 	if (ST_MATCHSERVER == CL_GET_SERVERTYPE_FROM_NODEID(id))
// 	{
// 		register2matchsvr();
// 	}
	
	CLProtocol::MatchSvrRelayRegister protocol;
	protocol.ability = RLPlayerMgr::Instance()->get_ablity();
	protocol.isRestart = (m_reconnect? 0: 1);
	
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (ST_BATTLE == servType)
	{
		InfoLogStream << "in RLInternalNetwork::OnConnected, register to battle server with isRestart(" << (UInt16)protocol.isRestart << ") ability(" << protocol.ability << ")" << LogStream::eos;
		Router::SendToBattleSvr(protocol);
	}
	else if (ST_DUNGEON_SVR == servType)
	{
		InfoLogStream << "in RLInternalNetwork::OnConnected, register to dungeon server with isRestart(" << (UInt16)protocol.isRestart << ")" << LogStream::eos;
		Router::SendToDungeonSvr(protocol);
	}
	else
	{
		InfoLogStream << "in RLInternalNetwork::OnConnected, register to match server with isRestart(" << (UInt16)protocol.isRestart << ")" << LogStream::eos;
		Router::SendToMatchSvr(protocol);
	}
}

void RLInternalNetwork::OnDisconnected(UInt32 id)
{
	Router::OnDisconnected(id);

	//
	m_reconnect = true;
}

bool RLInternalNetwork::PreHandlePacket(CLNetConnection* conn, Avalon::Packet* packet)
{
	if (CLInternalNetwork::PreHandlePacket(conn, packet))
	{
		return true;
	}

	//
	switch (packet->GetID())
	{
	case CLProtocol::SYS_REGISTER_REQ :		//
		break;
	case CLProtocol::SYS_REGISTER_RET:		//
	{
		//最新方案没有使用SYS_REGISTER_REQ/SYS_REGISTER_RES协议进行注册了
	}
		return true;		//return true 表示不需要进一步处理了
	default:
		break;
	}


	return false; //should be false here!
}

void RLInternalNetwork::register2matchsvr()
{
	//暂时不用，
}



RLInternalConnection::RLInternalConnection(CLNetwork* network) : CLInternalConnection(network)
{

}

RLInternalConnection::~RLInternalConnection()
{

}

bool RLInternalConnection::PreHandlePacket(Avalon::Packet* packet)
{

	if (CLInternalConnection::PreHandlePacket(packet))
	{
		return true;
	}

	//更多预处理 TODO:


	return false;
}
