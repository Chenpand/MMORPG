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

	//�����Ĵ����߼�
	//register to match server  ��ȻCLInternalNetwork::ConnectToServers()��������match serverʱ�ᷢ�� SYS_REGISTER_REQ
	//��relay serverҪ������ĵ�ַ��Ϣ�Լ�����������Ϣע��� match server
	//���·���������ӦЭ��MATCHSERVER_START_GAME_RES�з��� relay server�ĵ�ַ��Ϣ�Լ�������Ϣ

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
		//���·���û��ʹ��SYS_REGISTER_REQ/SYS_REGISTER_RESЭ�����ע����
	}
		return true;		//return true ��ʾ����Ҫ��һ��������
	default:
		break;
	}


	return false; //should be false here!
}

void RLInternalNetwork::register2matchsvr()
{
	//��ʱ���ã�
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

	//����Ԥ���� TODO:


	return false;
}
