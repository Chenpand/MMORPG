#include "USRouter.h"
#include <CLCenterProtocol.h>
#include <CLSysProtocol.h>
#include <CLGateProtocol.h>

#include "USNetwork.h"

void USRouter::OnConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = USNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		ErrorLogStream << "can't find connection(" << id << ")." << LogStream::eos;
		return;
	}

	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
	if (servType == ST_MATCHSERVER)
	{
        InfoLogStream << "add Match connection(" << id << ")." << LogStream::eos;
		m_MatchConnection = conn;
	}
	else if (servType == ST_OBSERVE)
	{
		InfoLogStream << "add Observe connection(" << id << ")." << LogStream::eos;
		m_ObserveConnection = conn;
	}
}

void USRouter::OnDisconnected(UInt32 id)
{
	InfoLogStream << "connection(" << id << ") disconnect." << LogStream::eos;
	UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);

	if (servType == ST_MATCHSERVER)
	{
		InfoLogStream << "del Match connection(" << id << ")." << LogStream::eos;
		m_MatchConnection = nullptr;
	}
	else if (servType == ST_OBSERVE)
	{
		InfoLogStream << "del Observe connection(" << id << ")." << LogStream::eos;
		m_ObserveConnection = nullptr;
	}
}


bool USRouter::SendToMatch(Avalon::Packet* packet)
{
	if (m_MatchConnection == nullptr)
	{
		ErrorLogStream << "can't find Match connection" << LogStream::eos;
		return false;
	}

	return m_MatchConnection->SendPacket(packet);
}

bool USRouter::SendToMatch(Avalon::Protocol& protocol)
{
	if (m_MatchConnection == nullptr)
	{
		ErrorLogStream << "can't find Match connection." << LogStream::eos;
		return false;
	}

	bool ret = false;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		ret = m_MatchConnection->SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);

	return ret;
}


bool USRouter::SendToObserve(Avalon::Packet* packet)
{
	if (m_ObserveConnection == nullptr)
	{
		ErrorLogStream << "can't find Observe connection" << LogStream::eos;
		return false;
	}

	return m_ObserveConnection->SendPacket(packet);
}

bool USRouter::SendToObserve(Avalon::Protocol& protocol)
{
	if (m_ObserveConnection == nullptr)
	{
		ErrorLogStream << "can't find Observe connection." << LogStream::eos;
		return false;
	}

	bool ret = false;
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		ret = m_ObserveConnection->SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);

	return ret;
}